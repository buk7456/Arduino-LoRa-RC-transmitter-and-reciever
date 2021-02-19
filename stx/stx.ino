/***************************************************************************************************
* Code for slave transmiter microcontroller 
* BUK 2020
* buk7456@gmail.com

* Tested to compile on Arduino IDE 1.8.9 or later
* Sketch should compile without warnings even with -WALL
***************************************************************************************************/

#include <SPI.h>
#include "LoRa.h"
#include "crc8.h"
#include <EEPROM.h>
#include "NonBlockingRtttl.h"

// Pins 
#define PIN_BUZZER         9
#define PIN_LCD_BACKLIGHT  6
#define PIN_SWC_UPPER_POS  A4 
#define PIN_SWC_LOWER_POS  A5 

//--------------- Freq allocation --------------------

/* LPD433 Band ITU region 1
The frequencies in this UHF band lie between 433.05Mhz and 434.790Mhz with 25kHz separation for a
total of 69 freq channels. Channel_1 is 433.075 Mhz and Channel_69 is 434.775Mhz. 
All our communications have to occur on any of these 69 channels. 
*/

/* Frequency list to pick from. The separation here is 300kHz (250kHz total lora bw + 
25kHz headroom on each sides.*/
uint32_t freqList[] = {433175000, 433475000, 433775000, 434075000, 434375000, 434675000};

uint8_t fhss_schema[3] = {0, 1, 2}; /* Index in freqList. Frequencies to use for hopping. 
These are changed when we receive a bind command from the master mcu. This schema also gets stored 
to eeprom so we don't have to rebind each time we switch on the transmitter. */

uint8_t ptr_fhss_schema = 0; 

//-------------- EEprom stuff --------------------
#define EE_INITFLAG         0xBB 
#define EE_ADR_INIT_FLAG    0
#define EE_ADR_TX_ID        1
#define EE_ADR_RX_ID        2
#define EE_ADR_FHSS_SCHEMA  3

//-------------- Audio ---------------------------
enum{  
  AUDIO_NONE = 0, 
  AUDIO_BATTERYWARN, AUDIO_THROTTLEWARN, AUDIO_TIMERELAPSED, AUDIO_INACTIVITY, AUDIO_TELEMWARN,
  AUDIO_SWITCHMOVED, AUDIO_TRIM_AIL, AUDIO_TRIM_ELE, AUDIO_TRIM_THR, AUDIO_TRIM_RUD,
  AUDIO_KEYTONE
};

uint8_t audioToPlay = AUDIO_NONE;

//Sounds in rtttl format
const char* battLowSound = "txbattlow:d=4,o=5,b=290:4c6,32p,4a#,32p,4g.";
const char* warnSound = "warn:d=4,o=4,b=160:4b5";
const char* shortBeepSound = "shortBeep:d=4,o=4,b=250:16c#7";
const char* timerElapsedSound = "timerElapsed:d=4,o=5,b=210:16b6,16p,8b6";
const char* trimAilSound = "ail:d=4,o=4,b=160:16a5";
const char* trimEleSound = "ele:d=4,o=4,b=160:16b5";
const char* trimThrSound = "thr:d=4,o=4,b=160:16c6";
const char* trimRudSound = "rud:d=4,o=4,b=160:16d6";
const char* inactivitySound = "idle:d=4,o=5,b=100:32c5,32g4,32c5"; 
const char* telemWarnSound = "telemWarn:d=4,o=5,b=90:8a#4";

//------------------------------------------------

uint8_t transmitterID = 0; //set on bind

uint8_t receiverID = 128;

bool radioInitialised = false;
unsigned long totalPacketsSent = 0;

bool bindRequestPending = false;
uint8_t bindStatus = 0; //1 on success, 2 on fail

uint8_t rfStatus = 0;
uint8_t lcdBacklightState = 0;
uint8_t rfPowerLevel;

uint8_t statusFlag;

bool hasUnsentServoData = false;
uint16_t ch1to8[8];

enum {MODE_BIND, MODE_SERVO_DATA, MODE_RECEIVE_TELEM};
uint8_t operatingMode = MODE_SERVO_DATA;

//----telemetry----
unsigned long telemModeEntryTime = 0;
#define TELEM_MODE_TIMEOUT  22 //in milliseconds
bool telemetryRequest = false; 
uint8_t receiverPacketRate = 0;

uint16_t telem_volts = 0x0FFF;     // in 10mV, sent by receiver with 12bits.  0x0FFF "No data"
//----------------

void doSerialCommunication();
void playTones();
void hop();
void bind();
void transmitServoData();
void receiveTelemetry();
uint16_t joinBytes(uint8_t _highByte, uint8_t _lowByte);

//==================================================================================================

void setup()
{
  // EEPROM init
  if (EEPROM.read(EE_ADR_INIT_FLAG) != EE_INITFLAG)
  {
    EEPROM.write(EE_ADR_TX_ID, transmitterID);
    EEPROM.write(EE_ADR_RX_ID, receiverID);
    EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
    EEPROM.write(EE_ADR_INIT_FLAG, EE_INITFLAG);
  }
  
  // Read from EEPROM
  transmitterID = EEPROM.read(EE_ADR_TX_ID);
  receiverID = EEPROM.read(EE_ADR_RX_ID);
  EEPROM.get(EE_ADR_FHSS_SCHEMA, fhss_schema);
  
  //setup pins
  pinMode(PIN_LCD_BACKLIGHT, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_SWC_UPPER_POS, INPUT_PULLUP);
  pinMode(PIN_SWC_LOWER_POS, INPUT_PULLUP);
  
  //init serial port
  Serial.begin(115200);
  delay(500);
  
  //setup lora module
  LoRa.setPins(10, 8); 
  if (LoRa.begin(freqList[0]))
  {
    LoRa.setSpreadingFactor(7); 
    LoRa.setCodingRate4(5);
    LoRa.setSignalBandwidth(250E3);
    radioInitialised = true;
  }
  else
    radioInitialised = false;
  
}

//========================================= MAIN ==================================================
void loop()
{
  /// ---------- SERIAL COMMUNICATIONS -----------------
  doSerialCommunication();

  ///----------- CONTROL LCD BACKLIGHT --------------------
  digitalWrite(PIN_LCD_BACKLIGHT, lcdBacklightState);
  
  ///----------- PLAY TONES -------------------------------
  playTones();

  ///----------- RF COMMUNICATIONS ------------------------
  if(radioInitialised)
  {
    //set power level
    static uint8_t lastPowerLevel = 0xFF;
    if(rfPowerLevel != lastPowerLevel && !LoRa.isTransmitting())
    {
      lastPowerLevel = rfPowerLevel;
      uint8_t power_dBm[5] = {3, 7, 10, 14, 17}; //2mW, 5mW, 10mW, 25mW, 50mW
      LoRa.sleep();
      LoRa.setTxPower(power_dBm[rfPowerLevel]);
      LoRa.idle();
    }
    
    //set to bind mode if bind request pending
    if(bindRequestPending && !LoRa.isTransmitting())
    {
      operatingMode = MODE_BIND;
      bindRequestPending = false;
    }
    
    //switch to receive mode on telemetry request
    if(telemetryRequest && operatingMode == MODE_SERVO_DATA && hasUnsentServoData == false)
    {
      operatingMode = MODE_RECEIVE_TELEM;
      telemModeEntryTime = millis();
    }

    //state machine
    switch (operatingMode)
    {
      case MODE_BIND:
        bind();
        break;
        
      case MODE_SERVO_DATA:
        transmitServoData();
        break;
        
      case MODE_RECEIVE_TELEM:
        receiveTelemetry();
        if(millis() - telemModeEntryTime > TELEM_MODE_TIMEOUT) //timeout
        {
          hop();
          operatingMode = MODE_SERVO_DATA;
        }
        break;
    }
  }
}

//==================================================================================================

void doSerialCommunication()
{
  /** Master to Slave MCU communication format as below
  byte 0 - First Status byte 
    bit7 - 0
    bit6 - backlight 1 on, 0 off
    bit5 - bind state 1 bind 0 operate
    bit4 - RF module, 1 on, 0 off 
    bit3 - DigChA, 1 on, 0 off
    bit2 - DigChB, 1 on, 0 off
    bit1 - Failsafe, 1 means failsafe data, 0 normal data
    bit0 - Reserved
  byte 1 - Second status byte
    bit7 - 0
    bits 6 to 4 reserved
    bit3 - telemetry request
    bits 2 to 0 RF Power level (3bits)
  byte 2 - Sound to play
  byte 3 to 18 - Ch1 thru 8 data (2 bytes per channel, total 16 bytes)
  byte 19 - CRC8
  */
  
  const uint8_t msgLength = 20;
  if (Serial.available() < msgLength)
  {
    return;
  }
  
  uint8_t _data[msgLength]; 

  uint8_t cntr = 0;
  while (Serial.available() > 0)
  {
    if (cntr < msgLength) 
    {
      _data[cntr] = Serial.read();
      cntr++;
    }
    else //Discard any extra data
      Serial.read();
  }
  
  //Check if valid
  bool hasValidSerialMsg = false;
  if(_data[msgLength - 1] == crc8Maxim(_data, msgLength - 1))
  {
    hasValidSerialMsg = true;
    //---- extract ----
    
    lcdBacklightState = (_data[0] >> 6) & 0x01;
    
    if((_data[0] >> 5) & 0x01)
      bindRequestPending = true;

    rfStatus = (_data[0] >> 4) & 0x01;
    
    //ChA, ChB, failsafe flag
    statusFlag = (_data[0] >> 1) & 0x07;
    
    rfPowerLevel = _data[1] & 0x07;
    
    audioToPlay = _data[2];
    
    //channel data
    for (uint8_t i = 0; i < sizeof(ch1to8)/sizeof(ch1to8[0]); i++)
      ch1to8[i] = joinBytes(_data[3 + i*2], _data[4 + i*2]);
    hasUnsentServoData = true;
    
    bool lastTelemetryRequestState = telemetryRequest;
    telemetryRequest = (_data[1] >> 3) & 0x01;
    if(telemetryRequest == false && lastTelemetryRequestState == true) //skip
      hasUnsentServoData = false;
  }
  
  /** Slave to Master mcu serial communication
  Byte0   Bits 3 to 2 --> Bind status, Bits 1 to 0 --> 3pos switch state
  Byte1   Transmitter packet rate
  Byte2   Packet rate at receiver side
  Byte3-4 Voltage telemetry
  Byte5   CRC8
  */

  if(hasValidSerialMsg) 
  {
    //calc transmitted packets per second
    static uint8_t txPktsPs = 0;
    static unsigned long lastTotalPacketsSent = 0;
    static unsigned long pktsPrevCalcMillis = 0;
    unsigned long ttElapsed = millis() - pktsPrevCalcMillis;
    if (ttElapsed >= 1000)
    {
      pktsPrevCalcMillis = millis();
      unsigned long pps = (totalPacketsSent - lastTotalPacketsSent) * 1000;
      pps /= ttElapsed;
      txPktsPs = pps & 0xFF;
      lastTotalPacketsSent = totalPacketsSent;
    }
    
    // read the 3 position switch. upperPos is 0, lowerPos is 1, midPos is 2
    uint8_t SwCState = 2;
    if(!digitalRead(PIN_SWC_UPPER_POS)) 
      SwCState = 0;
    else if(!digitalRead(PIN_SWC_LOWER_POS)) 
      SwCState = 1;
    
    //send 
    uint8_t _dataToSend[6];
    _dataToSend[0] = (bindStatus << 2) | SwCState;
    _dataToSend[1] = txPktsPs;
    _dataToSend[2] = receiverPacketRate;
    _dataToSend[3] = (telem_volts >> 8) & 0xFF;
    _dataToSend[4] = telem_volts & 0xFF;
    _dataToSend[5] = crc8Maxim(_dataToSend, 5);
    
    Serial.write(_dataToSend, 6);
    
    //reset flags
    bindStatus = 0;
  }
}

//--------------------------------------------------------------------------------------------------

uint16_t joinBytes(uint8_t _highByte, uint8_t _lowByte)
{
  uint16_t rslt;
  rslt = (uint16_t) _highByte;
  rslt <<= 8;
  rslt |= (uint16_t) _lowByte;
  return rslt;
}

//==================================================================================================

void playTones()
{
  static uint8_t lastAudioToPlay = AUDIO_NONE;
  if(audioToPlay != lastAudioToPlay) //init playback with the specified audio
  {
    lastAudioToPlay = audioToPlay;
    switch(audioToPlay)
    {
      case AUDIO_THROTTLEWARN:   
        rtttl::begin(PIN_BUZZER, warnSound); 
        break;
        
      case AUDIO_BATTERYWARN:    
        rtttl::begin(PIN_BUZZER, battLowSound); 
        break;
        
      case AUDIO_TIMERELAPSED:   
        rtttl::begin(PIN_BUZZER, timerElapsedSound); 
        break;
        
      case AUDIO_KEYTONE:        
      case AUDIO_SWITCHMOVED:    
        rtttl::begin(PIN_BUZZER, shortBeepSound); 
        break;
        
      case AUDIO_TRIM_AIL:
        rtttl::begin(PIN_BUZZER, trimAilSound); 
        break;
        
      case AUDIO_TRIM_ELE:
        rtttl::begin(PIN_BUZZER, trimEleSound); 
        break;
        
      case AUDIO_TRIM_THR:
        rtttl::begin(PIN_BUZZER, trimThrSound); 
        break;
        
      case AUDIO_TRIM_RUD:
        rtttl::begin(PIN_BUZZER, trimRudSound); 
        break;
        
      case AUDIO_INACTIVITY:
        rtttl::begin(PIN_BUZZER, inactivitySound);
        break;
        
      case AUDIO_TELEMWARN:
        rtttl::begin(PIN_BUZZER, telemWarnSound);
        break;
    }
  }
  else //Playback. Automatically stops once all notes have been played
    rtttl::play();
}

//==================================================================================================

void hop()
{
  ptr_fhss_schema++;
  if(ptr_fhss_schema >= sizeof(fhss_schema)/sizeof(fhss_schema[0]))
    ptr_fhss_schema = 0;
  
  uint8_t idx_freq = fhss_schema[ptr_fhss_schema];
  if(idx_freq < sizeof(freqList)/sizeof(freqList[0])) //prevents invalid references
  {
    LoRa.sleep();
    LoRa.setFrequency(freqList[idx_freq]);
    LoRa.idle();
  }
}

//==================================================================================================

void bind()
{
  /** Bind protocol Format 
  
    Transmitter to receiver
    -----------------------   
    Byte0     bit 7 to 1 transmitterID, bit0 is 0 for bind, 
    Byte1-11  hop channels, each channel 1 byte. If less channels, the rest of the bytes are 0 
    Byte12    CRC
    
    Receiver reply
    --------------
    Byte0     receiverID (allowed ID range 128 to 255 inclusive)
    Byte1-5   reserved
    Byte6     CRC
  */
  
  static bool bindInitialised = false;
  static uint8_t dataToTransmit[13]; 
  
  static unsigned long bindModeEntryTime = 0;
  static unsigned long bindAckEntryTime = 0;
  
  static bool isListeningForAck = false;
  
  const uint16_t BIND_MODE_TIMEOUT = 5000; 
  const uint16_t BIND_ACK_TIMEOUT  = 100; //Max time waiting for receiver's ack before retransmission
  
  /// INITIALISE BIND DATA
  if(bindInitialised == false)
  {
    bindModeEntryTime = millis();
    
    //--- clear fhss_schema
    memset(fhss_schema, 0xff, sizeof(fhss_schema));
    
    //--- generate random transmitterID and fhss_schema
    randomSeed(millis()); //Seed PRNG
    //transmitterID
    transmitterID = random(1, 127) & 0x7F; //allowed IDs 1 through 126 
    EEPROM.write(EE_ADR_TX_ID, transmitterID);
    //fhss_schema
    uint8_t idx = 0;
    while (idx < sizeof(fhss_schema)/sizeof(fhss_schema[0]))
    {
      uint8_t genVal = random(sizeof(freqList)/sizeof(freqList[0]));
      //check uniqueness
      bool unique = true;
      for(uint8_t k = 0; k < (sizeof(fhss_schema)/sizeof(fhss_schema[0])); k++)
      {
        if(genVal == fhss_schema[k]) //not unique
        {
          unique = false;
          break; //exit for loop
        }
      }
      if(unique)
      {
        fhss_schema[idx] = genVal; //add to fhss_schema
        idx++; //increment index
      }
    }
    EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
    
    //prepare data
    memset(dataToTransmit, 0, sizeof(dataToTransmit));
    dataToTransmit[0] = transmitterID << 1;
    for(uint8_t i = 0; i < sizeof(fhss_schema)/sizeof(fhss_schema[0]); i++)
      dataToTransmit[1 + i] = fhss_schema[i];
    dataToTransmit[12] = crc8Maxim(dataToTransmit, 12);
    
    //set to bind frequency
    LoRa.sleep();
    LoRa.setFrequency(freqList[0]);
    LoRa.idle();
    
    bindInitialised = true; 
  }

  //transmit bind
  static bool transmitInitiated = false;
  if(bindInitialised && isListeningForAck == false && transmitInitiated == false) 
  {
    if(LoRa.beginPacket())
    {
      LoRa.write(dataToTransmit, 13);
      LoRa.endPacket(true); //non-blocking
      delay(1);
      transmitInitiated = true;
    }
  }
  
  //listen for reply. If weve got a reply from receiver, exit
  if(!LoRa.isTransmitting())
  {
    if(isListeningForAck == false)
    {
      isListeningForAck = true;
      bindAckEntryTime = millis();
    }
    
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) //received a packet
    {
      uint8_t msgBuff[20];
      uint8_t cntr = 0;
      while (LoRa.available() > 0) 
      {
        if(cntr < (sizeof(msgBuff)/sizeof(msgBuff[0])))
        {
          msgBuff[cntr] = LoRa.read();
          cntr++;
        }
        else // discard any extra data
          LoRa.read(); 
      }
      
      // Check if the received data is valid based on crc and receiver ID
      uint8_t crcQQ = msgBuff[6];
      uint8_t computedCRC = crc8Maxim(msgBuff, 6);
      uint8_t rxID = msgBuff[0];
      if(crcQQ == computedCRC && rxID > 127) //have a valid message from a valid receiver
      {
        //Extract data
        receiverID = rxID;
        EEPROM.write(EE_ADR_RX_ID, receiverID);
        //Exit bind
        bindStatus = 1;
        bindInitialised = false;
        isListeningForAck = false;
        transmitInitiated = false;
        hop();
        operatingMode = MODE_SERVO_DATA;
      }
    }
    
    //timeout
    if(millis() - bindAckEntryTime > BIND_ACK_TIMEOUT)
    {
      isListeningForAck = false;
      transmitInitiated = false;
    }
  }
  
  //Timeout of bind
  if((millis() - bindModeEntryTime > BIND_MODE_TIMEOUT) && !LoRa.isTransmitting())
  {
    bindStatus = 2;
    bindInitialised = false;
    isListeningForAck = false;
    transmitInitiated = false;
    hop();
    operatingMode = MODE_SERVO_DATA;
  }
}

//==================================================================================================

void transmitServoData()
{
  /** Transmitter to receiver message format
  ------------------------------------------------
  Byte0  
  bits 7 to 1 is TransmiterID, 
  bit0 is 1 for servo data
  ------------------------------------------------
  Byte1     Byte2     Byte3     Byte4     Byte5       
  11111111  11222222  22223333  33333344  44444444 
  ------------------------------------------------
  Byte6     Byte7     Byte8     Byte9     Byte10
  55555555  55666666  66667777  77777788  88888888
  ------------------------------------------------
  Byte11    Byte12
  0abftddd  CCCCCCCC
  ------------------------------------------------
  Servo Chs 1 to 8 encoded as 10 bits      
  a is digital chA bit
  b is digital ChB bit 
  f is failsafe flag
  t is telemetry request flag. 
  ddd is the current tx rf power level (3bits)
  CCCCCCCC is the 8 bit CRC xored with receiverID
  */
  
  if(!hasUnsentServoData)
    return;
  
  static bool transmitInitiated = false;
  static bool hopPending = false;
  
  if(rfStatus == 0) //don't send anything
  {
    transmitInitiated = false;
    hopPending = false;
    hasUnsentServoData = false;
    return;
  }

  //transmit
  if (!transmitInitiated) 
  {
    //------ encode  
    uint8_t payload[13];
    memset(payload, 0, sizeof(payload));
    
    payload[0] = transmitterID << 1 | 0x01; 
    
    payload[1] = (ch1to8[0] >> 2) & 0xFF;
    payload[2] = (ch1to8[0] << 6 | ch1to8[1] >> 4) & 0xFF;
    payload[3] = (ch1to8[1] << 4 | ch1to8[2] >> 6) & 0xFF;
    payload[4] = (ch1to8[2] << 2 | ch1to8[3] >> 8) & 0xFF;
    payload[5] = ch1to8[3] & 0xFF;
    payload[6] = (ch1to8[4] >> 2) & 0xFF;
    payload[7] = (ch1to8[4] << 6 | ch1to8[5] >> 4) & 0xFF;
    payload[8] = (ch1to8[5] << 4 | ch1to8[6] >> 6) & 0xFF;
    payload[9] = (ch1to8[6] << 2 | ch1to8[7] >> 8) & 0xFF;
    payload[10] = ch1to8[7] & 0xFF;
    
    payload[11] =  statusFlag << 4;
    payload[11] |= telemetryRequest << 3;
    payload[11] |= rfPowerLevel;
    
    payload[12] = crc8Maxim(payload, 12) ^ receiverID;
    
    //------- start transmit
    if(LoRa.beginPacket())
    {
      LoRa.write(payload, 13);
      LoRa.endPacket(true); //async
      delay(1);
      
      transmitInitiated = true;
      hopPending = true;
      totalPacketsSent++;
    }
  }
  
  //on transmit done
  if(!LoRa.isTransmitting())
  {
    hasUnsentServoData = false;
    transmitInitiated = false;
    
    if(hopPending)
    {
      hop();
      hopPending = false;
    }
  }
}

//==================================================================================================

void receiveTelemetry()
{
  /** Receiver to Transmitter message format
  ------------------------------------------
  Byte0    Receiver ID
  Byte1    Average packet rate
  
  Byte2       Byte3       
  vvvvvvvv    vvvv0000
  (v - voltage)
  
  Byte4    CRC8 XOR transmitterID
  */
  
  static unsigned long timeOfLastTelemReception = 0;

  int packetSize = LoRa.parsePacket();
  if (packetSize > 0) //received a packet
  {
    uint8_t msgBuff[20];
    uint8_t cntr = 0;
    while (LoRa.available() > 0) 
    {
      if(cntr < (sizeof(msgBuff)/sizeof(msgBuff[0])))
      {
        msgBuff[cntr] = LoRa.read();
        cntr++;
      }
      else // discard any extra data
        LoRa.read(); 
    }
    
    // Check if the received data is valid
    uint8_t rxID = msgBuff[0];
    uint8_t crcQQ = msgBuff[4];
    uint8_t computedCRC = crc8Maxim(msgBuff, 4) ^ transmitterID;
    if(crcQQ == computedCRC && rxID == receiverID) //have a valid message from bound receiver
    {
      timeOfLastTelemReception = millis();
      //-- Extract data --
      receiverPacketRate = msgBuff[1];
      telem_volts = ((uint16_t)msgBuff[2] << 4 & 0xFF0) | ((uint16_t)msgBuff[3] >> 4 & 0x0F);
    }
    
    hop();
    
    //Change mode
    operatingMode = MODE_SERVO_DATA;
  }

  //reset data if no telemetry received
  if(millis() - timeOfLastTelemReception > 3000)
  {
    receiverPacketRate = 0;
    telem_volts = 0x0FFF;
  }
}
