/*
  ==================================================================================================

  Firmware for the slave mcu of the rc transmitter.
  Target mcu: Atmega328p

  (c) 2020-2021 buk7456  
  buk7456 at gmail dot com
  
  Released under the MIT Licence
  
  Tested to compile on Arduino IDE 1.8.9 or later.
  The code should compile with no warnings even with -Wall.

  ==================================================================================================
*/


#include <SPI.h>
#include "LoRa.h"
#include "crc8.h"
#include <EEPROM.h>
#include "NonBlockingRtttl.h"

// Pins 

#define PIN_LCD_BACKLIGHT   6
#define PIN_BUZZER          9

#define PIN_SWC_LOWER_POS   A5
#define PIN_SWC_UPPER_POS   A4 
#define PIN_SWE             A3
#define PIN_SWF             A2

#define PIN_POWER_OFF_SENSE A1
#define PIN_POWER_LATCH     A0

//--------------- Freq allocation --------------------

/* LPD433 Band ITU region 1
The frequencies in this UHF band lie between 433.05Mhz and 434.790Mhz with 25kHz separation for a
total of 69 freq channels. Channel_1 is 433.075 Mhz and Channel_69 is 434.775Mhz. 
All our communications have to occur on any of these 69 channels. 
*/

/* Frequency list to pick from. The separation here is 300kHz (250kHz total lora bw + 
50kHz guard band.*/
uint32_t freqList[] = {433175000, 433475000, 433775000, 434075000, 434375000, 434675000};

uint8_t fhss_schema[3] = {0, 1, 2}; /* Index in freqList. Frequencies to use for hopping. 
These are changed when we receive a bind command from the master mcu. This schema also gets stored 
to eeprom so we don't have to rebind each time we switch on the transmitter. */

uint8_t idx_fhss_schema = 0; 

//-------------- EEprom stuff --------------------
#define EE_INITFLAG         0xBB 
#define EE_ADR_INIT_FLAG    0
#define EE_ADR_TX_ID        1
#define EE_ADR_RX_ID        2
#define EE_ADR_FHSS_SCHEMA  3

//-------------- Audio ---------------------------
enum{  
  AUDIO_NONE = 0, 
  
  AUDIO_BATTERYWARN, 
  AUDIO_THROTTLEWARN, 
  AUDIO_TIMERELAPSED, 
  AUDIO_INACTIVITY, 
  AUDIO_TELEMWARN,
  AUDIO_BIND_SUCCESS,
  AUDIO_TRIM_AIL, 
  AUDIO_TRIM_ELE, 
  AUDIO_TRIM_THR, 
  AUDIO_TRIM_RUD,
  AUDIO_TRIM_MOVED,
  AUDIO_TRIM_MODE_ENTERED,
  AUDIO_TRIM_MODE_EXITED,
  
  AUDIO_SWITCHMOVED,
  
  AUDIO_KEYTONE
};

uint8_t audioToPlay = AUDIO_NONE;

//Sounds in rtttl format
const char* battLowSound = "txbattlow:d=4,o=5,b=290:4p,4c6,32p,4a#,32p,4g.";
const char* warnSound = "warn:d=4,o=4,b=160:4e5";
const char* shortBeepSound = "shortBeep:d=4,o=4,b=250:16p,16d5";
const char* timerElapsedSound = "timerElapsed:d=4,o=5,b=210:16p,16b6,16p,8b6";
const char* trimAilSound = "ail:d=4,o=4,b=160:16a5";
const char* trimEleSound = "ele:d=4,o=4,b=160:16b5";
const char* trimThrSound = "thr:d=4,o=4,b=160:16c6";
const char* trimRudSound = "rud:d=4,o=4,b=160:16d6";
const char* inactivitySound = "idle:d=4,o=5,b=100:8p,32c5,32g4,32c5"; 
const char* telemWarnSound = "telemWarn:d=4,o=5,b=90:8p,8a#4";
const char* bindSound = "bind:d=4,o=5,b=75:32d#5,32g5,32a#5,16d6";
const char* trimEnteredSound = "trimEnter:d=4,o=5,b=120:32d#5,32g5,32a5";
const char* trimExitedSound = "trimExit:d=4,o=5,b=120:32a5,32g5,32d#5";
const char* trimMovedSound = "trimMove:d=4,o=4,b=250:16a#7";


//------------------------------------------------

#define MAX_PACKET_SIZE  19
uint8_t packet[MAX_PACKET_SIZE];

enum{
  PAC_BIND                   = 0x0,
  PAC_ACK_BIND               = 0x1,
  PAC_READ_OUTPUT_CH_CONFIG  = 0x2,
  PAC_SET_OUTPUT_CH_CONFIG   = 0x3,
  PAC_ACK_OUTPUT_CH_CONFIG   = 0x4,
  PAC_RC_DATA                = 0x5,
  PAC_TELEMETRY              = 0x6,
};

uint8_t transmitterID = 0; //set on bind

uint8_t receiverID = 0;

bool radioInitialised = false;
unsigned long totalPacketsSent = 0;

bool isRequestingBind = false;

uint8_t bindStatusCode = 0; //1 on success, 2 on fail

bool rfEnabled = false;

uint8_t idxRFPowerLevel = 0;

bool isFailsafeData = false;

bool isReadOutputChConfig = false;
bool isSetOutputChConfig = false;

bool hasPendingRCData = false;
uint16_t ch1to9[9];

enum {
  MODE_BIND, 
  MODE_RC_DATA, 
  MODE_GET_TELEM,
  MODE_GET_RECEIVER_CONFIG,
  MODE_SEND_RECEIVER_CONFIG
};
uint8_t operatingMode = MODE_RC_DATA;

bool requestPoweroff = false;

uint8_t outputChConfig[9];
bool gotOutputChConfig = false;

uint8_t receiverConfigStatusCode = 0; //1 on success, 2 on fail

unsigned long telemModeEntryTime = 0;

#define TIMEOUT_MODE_GET_TELEM   25

bool isRequestingTelemetry = false; 

uint8_t receiverPacketRate = 0;

uint16_t telem_volts = 0x0FFF;  // in 10mV, sent by receiver with 12bits.  0x0FFF "No data"


void doSerialCommunication();
void readPowerSwitch();
void powerOff();
void playTones();
void doRfCommunication();
void hop();
void bind();
void transmitRCdata();
void transmitReceiverConfig();
void getReceiverConfig();
uint8_t buildPacket(uint8_t srcID, uint8_t destID, uint8_t dataIdentifier, uint8_t *dataBuff, uint8_t dataLen);
bool checkPacket(uint8_t srcID, uint8_t destID, uint8_t dataIdentifier, uint8_t *packetBuff, uint8_t packetSize);

//==================================================================================================

void setup()
{
  pinMode(PIN_POWER_LATCH, OUTPUT);
  digitalWrite(PIN_POWER_LATCH, HIGH);
  
  pinMode(PIN_POWER_OFF_SENSE, INPUT);
  
  pinMode(PIN_LCD_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_LCD_BACKLIGHT, HIGH);
  
  pinMode(PIN_BUZZER, OUTPUT);
  
  pinMode(PIN_SWC_UPPER_POS, INPUT_PULLUP);
  pinMode(PIN_SWC_LOWER_POS, INPUT_PULLUP);
  pinMode(PIN_SWF, INPUT_PULLUP);
  pinMode(PIN_SWE, INPUT_PULLUP);
  
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
  
  //init serial port
  Serial.begin(115200);
  delay(200);
  
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

  ///----------- PLAY TONES -------------------------------
  playTones();

  ///----------- RF COMMUNICATIONS ------------------------
  doRfCommunication();

}

//==================================================================================================

void doSerialCommunication()
{
  /* 
    MASTER MCU TO SLAVE MCU COMMUNICATION  
    
    - The transmit length is always fixed at 24 bytes.
    - General format is as below.
    ---------------------------------------------------------------
      Description |  Status0  Status1   Audio   Data       CRC8
      Size        |  1 byte    1 byte   1 byte  20 bytes   1 byte
      Offset      |  0         1        2       3          23
    ---------------------------------------------------------------
  */

  /* Status0 
      bit0-2 RF power level
      bit3   RF enabled
      bit4   Backlight
      bit5   Power off
  */
  
  /* Status1
      bit0  failsafe data
      bit1  write receiver config
      bit2  get receiver config 
      bit3  get telemetry
      bit4  enter bind mode 
  */


  ///--------- CHECK IF READY ------------------
  const uint8_t msgLength = 24;
  if (Serial.available() < msgLength)
  {
    return;
  }
  
  /// -------- READ INTO TEMP BUFFER ------------
  
  uint8_t tmpBuff[msgLength]; 

  uint8_t cntr = 0;
  while (Serial.available() > 0)
  {
    if (cntr < msgLength) 
    {
      tmpBuff[cntr] = Serial.read();
      cntr++;
    }
    else //Discard any extra data
      Serial.read();
  }
  
  ///------ CHECK CRC ---------------------------
  if(tmpBuff[msgLength - 1] != crc8Maxim(tmpBuff, msgLength - 1))
  {
    return;
  }
  
  ///------ EXTRACT -----------------------------
  
  //--- status byte 0 ---
 
  uint8_t status0 = tmpBuff[0];
  
  idxRFPowerLevel = status0 & 0x07;
  
  rfEnabled = (status0 >> 3) & 0x01; 
  
  digitalWrite(PIN_LCD_BACKLIGHT, (status0 >> 4) & 0x01);
  
  if((status0 >> 5) & 0x01)
    powerOff();
  
  //--- status byte 1 ---
  
  uint8_t status1 = tmpBuff[1];

  if((status1 >> 4) & 0x01)
  {
    isRequestingBind = true;
  }
  else if((status1 >> 2) & 0x01)
  {
    isReadOutputChConfig = true;
  }
  else if((status1 >> 1) & 0x01)
  {
    isSetOutputChConfig = true;
    for(uint8_t i = 0; i < 9; i++)
      outputChConfig[i] = tmpBuff[3 + i];
  }
  else
  {
    isFailsafeData = status1 & 0x01;
    
    uint8_t prevState_TelemetryRequest = isRequestingTelemetry;
    isRequestingTelemetry = (status1 >> 3) & 0x01;
    if(prevState_TelemetryRequest == 1) //skip this data
      hasPendingRCData = false;
    else
    {
      hasPendingRCData = true;
      for(uint8_t i = 0; i < 9; i++)
        ch1to9[i] = (uint16_t)tmpBuff[3 + i * 2] << 8 | (uint16_t)tmpBuff[4 + i * 2]; //combine every two bytes
    }
  }
    

  //--- audio ---
  audioToPlay = tmpBuff[2]; 


  /// ----------- REPLY TO MASTER MCU -------------
  
  /* 
  SLAVE TO MASTER MCU SERIAL COMMUNICATION
  
  Byte0     Bit7     --> Got receiver channel configuration
            Bit 6    --> Request poweroff
            Bits 5,4 --> Bind status
            Bit 3    --> SwF
            Bit 2    --> SwE 
            Bits 1,0 --> 3pos switch (SwC) state
  
  Byte1     Receiver config status code
  Byte2     Transmitter packet rate
  Byte3     Packet rate at receiver side
  Byte4-5   Voltage telemetry
  Byte6-14  Receiver channel config Ch1 to Ch9
  Byte15    CRC8
  */

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
  uint8_t swCState = 2;
  if(!digitalRead(PIN_SWC_UPPER_POS)) 
    swCState = 0;
  else if(!digitalRead(PIN_SWC_LOWER_POS)) 
    swCState = 1;
  
  //read SwE and SwF
  bool swEEngaged = false;
  if(!digitalRead(PIN_SWE))
    swEEngaged = true;
  bool swFEngaged = false;
  if(!digitalRead(PIN_SWF)) 
    swFEngaged = true;
  
  //get power switch state
  readPowerSwitch();
  
  //send 
  uint8_t dataToSend[16];
  memset(dataToSend, 0, sizeof(dataToSend));
  
  dataToSend[0] |= (gotOutputChConfig & 0x01) << 7;
  dataToSend[0] |= (requestPoweroff & 0x01) << 6;
  dataToSend[0] |= (bindStatusCode & 0x03) << 4; 
  dataToSend[0] |= (swFEngaged & 0x01) << 3;
  dataToSend[0] |= (swEEngaged & 0x01) << 2;
  dataToSend[0] |= swCState & 0x03;
  
  dataToSend[1] = receiverConfigStatusCode;
  dataToSend[2] = txPktsPs;
  dataToSend[3] = receiverPacketRate;
  dataToSend[4] = (telem_volts >> 8) & 0xFF;
  dataToSend[5] = telem_volts & 0xFF;
  
  for(uint8_t i = 0; i < 9; i++)
    dataToSend[6 + i] = outputChConfig[i];
  
  dataToSend[15] = crc8Maxim(dataToSend, 15);
  
  Serial.write(dataToSend, sizeof(dataToSend));
  
  //reset flags
  bindStatusCode = 0;
  receiverConfigStatusCode = 0;
  gotOutputChConfig = false;
}

//--------------------------------------------------------------------------------------------------

void readPowerSwitch()
{  
  static bool switchTimerInitiated = false;
  static uint32_t swStartTime = 0;
  
  requestPoweroff = false;
  
  if(digitalRead(PIN_POWER_OFF_SENSE) == HIGH)
  {
    if(!switchTimerInitiated)
    {
      swStartTime = millis();
      switchTimerInitiated = true;
    }
    if(millis() - swStartTime > 500)
      requestPoweroff = true;
  }
  else
    switchTimerInitiated = false;
}

//--------------------------------------------------------------------------------------------------

void powerOff()
{
  //stop LoRa module
  if(radioInitialised)
  {
    while(LoRa.isTransmitting())
    {    
      delay(2);
    }
    LoRa.sleep();
    delay(5);
  }
  
  //pull power pin
  pinMode(PIN_POWER_LATCH, INPUT);
  while(1)
  {
  }
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
        
      case AUDIO_BIND_SUCCESS:
        rtttl::begin(PIN_BUZZER, bindSound);
        break;
        
      case AUDIO_TRIM_MOVED:
        rtttl::begin(PIN_BUZZER, trimMovedSound);
        break;
        
      case AUDIO_TRIM_MODE_ENTERED:
        rtttl::begin(PIN_BUZZER, trimEnteredSound);
        break;
        
      case AUDIO_TRIM_MODE_EXITED:
        rtttl::begin(PIN_BUZZER, trimExitedSound);
        break;
    }
  }
  else //Playback. Automatically stops once all notes have been played
    rtttl::play();
}

//==================================================================================================

void doRfCommunication()
{
  if(!radioInitialised)
  {
    return;
  }
  
  if(!LoRa.isTransmitting())
  {
    //--- set power level ---
    
    static uint8_t prevIdxRFPowerLevel = 0xFF;
    if(idxRFPowerLevel != prevIdxRFPowerLevel)
    {
      prevIdxRFPowerLevel = idxRFPowerLevel;
      uint8_t power_dBm[5] = {3, 7, 10, 14, 17}; //2mW, 5mW, 10mW, 25mW, 50mW
      LoRa.sleep();
      LoRa.setTxPower(power_dBm[idxRFPowerLevel]);
      LoRa.idle();
    }
    
    //--- Change modes ---
    if(isRequestingBind)
    {
      operatingMode = MODE_BIND;
      isRequestingBind = false;
    }
    else if(isReadOutputChConfig && operatingMode != MODE_BIND)
    {
      operatingMode = MODE_GET_RECEIVER_CONFIG;
      isReadOutputChConfig = false;
    }
    else if(isSetOutputChConfig && operatingMode != MODE_BIND)
    {
      operatingMode = MODE_SEND_RECEIVER_CONFIG;
      isSetOutputChConfig = false;
    }
    else if(isRequestingTelemetry && operatingMode == MODE_RC_DATA && !hasPendingRCData)
    {
      operatingMode = MODE_GET_TELEM;
      telemModeEntryTime = millis();
    }
  }

  //state machine
  switch (operatingMode)
  {
    case MODE_BIND:
      bind();
      break;
      
    case MODE_GET_RECEIVER_CONFIG:
      getReceiverConfig();
      break;
      
    case MODE_SEND_RECEIVER_CONFIG:
      transmitReceiverConfig();
      break;
      
    case MODE_RC_DATA:
      transmitRCdata();
      break;
      
    case MODE_GET_TELEM:
      getTelemetry();
      if(millis() - telemModeEntryTime > TIMEOUT_MODE_GET_TELEM)
      {
        hop();
        operatingMode = MODE_RC_DATA;
      }
      break;
  }

}

//--------------------------------------------------------------------------------------------------

void hop()
{
  idx_fhss_schema++;
  if(idx_fhss_schema >= sizeof(fhss_schema)/sizeof(fhss_schema[0]))
    idx_fhss_schema = 0;
  
  uint8_t idx_freq = fhss_schema[idx_fhss_schema];
  if(idx_freq < sizeof(freqList)/sizeof(freqList[0])) //prevents invalid references
  {
    LoRa.sleep();
    LoRa.setFrequency(freqList[idx_freq]);
    LoRa.idle();
  }
}

//--------------------------------------------------------------------------------------------------

void bind()
{
  static bool bindInitialised = false;
  static bool transmitInitiated = false;

  static unsigned long bindModeEntryTime = 0;
  static unsigned long bindAckEntryTime = 0;
  
  static bool isListeningForAck = false;
  
  #define TIMEOUT_MODE_BIND  4000 
  #define TIMEOUT_BIND_ACK   100  //Max time waiting for receiver's ack before retransmission
  
  /// INITIALISE
  if(!bindInitialised)
  {
    bindModeEntryTime = millis();

    //--- generate random transmitterID and fhss_schema
    
    randomSeed(millis()); //Seed PRNG
    
    transmitterID = random(0x01, 0xFF);
    
    memset(fhss_schema, 0xFF, sizeof(fhss_schema)); //clear schema
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

    //--- set to bind frequency
    LoRa.sleep();
    LoRa.setFrequency(freqList[0]);
    LoRa.idle();
    
    bindInitialised = true;
  }
  
  ///START TRANSMIT
  if(bindInitialised && !isListeningForAck && !transmitInitiated)
  {
    if(LoRa.beginPacket())
    {
      uint8_t _packetLen = buildPacket(transmitterID, 0x00, PAC_BIND, fhss_schema, sizeof(fhss_schema)/sizeof(fhss_schema[0]));
      LoRa.write(packet, _packetLen);
      LoRa.endPacket(true); //non-blocking
      delay(1);
      transmitInitiated = true;
    }
  }
  
  /// ON DONE TRANSMIT, LISTEN FOR REPLY OR TIMEOUT 
  if(!LoRa.isTransmitting())
  {
    if(!isListeningForAck)
    {
      isListeningForAck = true;
      bindAckEntryTime = millis();
    }
    
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) //received a packet
    {
      uint8_t msgBuff[30];
      memset(msgBuff, 0, sizeof(msgBuff));
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
      // Check if the packet is valid and extract the data
      if(checkPacket(0x00, transmitterID, PAC_ACK_BIND, msgBuff, packetSize))
      {
        //check length of data and receiverID range
        if((msgBuff[2] & 0x0F) == 1 && msgBuff[3] > 0x00)
        {
          bindStatusCode = 1; //bind success
          receiverID = msgBuff[3];
          
          //Save to eeprom
          EEPROM.write(EE_ADR_TX_ID, transmitterID);
          EEPROM.write(EE_ADR_RX_ID, receiverID);
          EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
          
          //clear flags
          bindInitialised = false;
          isListeningForAck = false;
          transmitInitiated = false;
          
          hop();
          operatingMode = MODE_RC_DATA;
          
          return;
        }
      }
    }
    
    if(isListeningForAck && millis() - bindAckEntryTime > TIMEOUT_BIND_ACK) //timeout of ack
    {
      transmitInitiated = false;
      isListeningForAck = false;
    }
    
    if(millis() - bindModeEntryTime > TIMEOUT_MODE_BIND) //timeout of bind
    {
      bindStatusCode = 2; //bind failed
      
      //restore so that we don't unintentionally unbind a bound receiver
      transmitterID = EEPROM.read(EE_ADR_TX_ID);
      receiverID = EEPROM.read(EE_ADR_RX_ID);
      EEPROM.get(EE_ADR_FHSS_SCHEMA, fhss_schema);
      
      //clear flags
      bindInitialised = false;
      isListeningForAck = false;
      transmitInitiated = false;
      
      hop();
      operatingMode = MODE_RC_DATA;
      
      return;
    }

  }
}

//--------------------------------------------------------------------------------------------------

void transmitRCdata()
{
  if(!hasPendingRCData)
    return;
  
  static bool transmitInitiated = false;
  static bool hopPending = false;
  
  if(!rfEnabled) //don't send anything
  {
    transmitInitiated = false;
    hopPending = false;
    hasPendingRCData = false;
    
    return;
  }

  /// START TRANSMIT
  if(!transmitInitiated) 
  {
    //encode  
    uint8_t dataToSend[12];
    memset(dataToSend, 0, sizeof(dataToSend));
    
    dataToSend[0]  = (ch1to9[0] >> 2) & 0xFF;
    dataToSend[1]  = (ch1to9[0] << 6 | ch1to9[1] >> 4) & 0xFF;
    dataToSend[2]  = (ch1to9[1] << 4 | ch1to9[2] >> 6) & 0xFF;
    dataToSend[3]  = (ch1to9[2] << 2 | ch1to9[3] >> 8) & 0xFF;
    dataToSend[4]  = ch1to9[3] & 0xFF;
    
    dataToSend[5]  = (ch1to9[4] >> 2) & 0xFF;
    dataToSend[6]  = (ch1to9[4] << 6 | ch1to9[5] >> 4) & 0xFF;
    dataToSend[7]  = (ch1to9[5] << 4 | ch1to9[6] >> 6) & 0xFF;
    dataToSend[8]  = (ch1to9[6] << 2 | ch1to9[7] >> 8) & 0xFF;
    dataToSend[9] = ch1to9[7] & 0xFF;
    
    dataToSend[10] = (ch1to9[8] >> 2) & 0xFF;
    dataToSend[11] = (ch1to9[8] << 6) & 0xC0;
    
    dataToSend[11] |= (isFailsafeData & 0x01) << 4;
    dataToSend[11] |= (isRequestingTelemetry & 0x01) << 3;
    dataToSend[11] |= idxRFPowerLevel & 0x07;

    uint8_t _packetLen = buildPacket(transmitterID, receiverID, PAC_RC_DATA, dataToSend, sizeof(dataToSend));

    if(LoRa.beginPacket())
    {
      LoRa.write(packet, _packetLen);
      LoRa.endPacket(true); //async
      delay(1);

      transmitInitiated = true;
      hopPending = true;
      totalPacketsSent++;
    }
  }
  
  /// ON TRANSMIT DONE
  if(!LoRa.isTransmitting())
  {
    hasPendingRCData = false;
    transmitInitiated = false;
    
    if(hopPending)
    {
      hop();
      hopPending = false;
    }
  }
}

//--------------------------------------------------------------------------------------------------

void getReceiverConfig()
{
  static bool transmitInitiated = false;
  static bool isListeningForReply = false;
  
  static uint32_t listenEntryTime = 0;
  const int maxListenTime = 40;
  
  static int retryCount = 0;
  const int maxRetries  = 5 * sizeof(fhss_schema) / sizeof(fhss_schema[0]);

  //Start transmit
  if(!transmitInitiated)
  {
    uint8_t _packetLen = buildPacket(transmitterID, receiverID, PAC_READ_OUTPUT_CH_CONFIG, NULL, 0);
    if(LoRa.beginPacket())
    {
      LoRa.write(packet, _packetLen);
      LoRa.endPacket(true); //async
      delay(1);

      transmitInitiated = true;
    }
  }
  
  //On transmit done, listen for reply
  if(!LoRa.isTransmitting())
  {
    if(!isListeningForReply)
    {
      hop();
      isListeningForReply = true;
      listenEntryTime = millis();
    }
    
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) //received a packet
    {
      uint8_t msgBuff[30];
      memset(msgBuff, 0, sizeof(msgBuff));
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
      
      hop();
      
      //Check if packet is valid and extract the data
      if(checkPacket(receiverID, transmitterID, PAC_READ_OUTPUT_CH_CONFIG, msgBuff, packetSize))
      {
        //check length
        if((msgBuff[2] & 0x0F) == 9) //9 bytes
        {
          gotOutputChConfig = true;
          for(uint8_t i = 0; i < 9; i++)
            outputChConfig[i] = msgBuff[3 + i];
          
          //Change mode
          operatingMode = MODE_RC_DATA;
          
          //reset
          isListeningForReply = false;
          transmitInitiated = false;
          retryCount = 0;
          
          //exit
          return;
        }
      }
    }
    
    //Retry if nothing was received
    if(isListeningForReply && (millis() - listenEntryTime) > maxListenTime)
    {
      isListeningForReply = false;
      transmitInitiated = false;
      ++retryCount;
      if(retryCount > maxRetries) //timeout
      {
        retryCount = 0;
        operatingMode = MODE_RC_DATA;
        hop();
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

void transmitReceiverConfig()
{
  static bool transmitInitiated = false;
  static bool isListeningForReply = false;
  
  static uint32_t listenEntryTime = 0;
  const int maxListenTime = 40;
  
  static int retryCount = 0;
  const int maxRetries  = 5 * sizeof(fhss_schema) / sizeof(fhss_schema[0]);

  //Start transmit
  if(!transmitInitiated)
  {
    uint8_t _packetLen = buildPacket(transmitterID, receiverID, PAC_SET_OUTPUT_CH_CONFIG, outputChConfig, sizeof(outputChConfig));
    if(LoRa.beginPacket())
    {
      LoRa.write(packet, _packetLen);
      LoRa.endPacket(true); //async
      delay(1);

      transmitInitiated = true;
    }
  }
  
  //On transmit done, listen for reply
  if(!LoRa.isTransmitting())
  {
    if(!isListeningForReply)
    {
      hop();
      isListeningForReply = true;
      listenEntryTime = millis();
    }
    
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) //received a packet
    {
      uint8_t msgBuff[30];
      memset(msgBuff, 0, sizeof(msgBuff));
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
      
      hop();
      
      //Check if packet is valid and extract the data
      if(checkPacket(receiverID, transmitterID, PAC_ACK_OUTPUT_CH_CONFIG, msgBuff, packetSize))
      {
        //indicate success
        receiverConfigStatusCode = 1;
        
        //Change mode
        operatingMode = MODE_RC_DATA;
        
        //reset
        isListeningForReply = false;
        transmitInitiated = false;
        retryCount = 0;
        
        //exit
        return;
      }
    }
    
    //Retry if nothing was received
    if(isListeningForReply && (millis() - listenEntryTime) > maxListenTime)
    {
      isListeningForReply = false;
      transmitInitiated = false;
      ++retryCount;
      if(retryCount > maxRetries) //timeout
      {
        retryCount = 0;
        //indicate a failure
        receiverConfigStatusCode = 2;
        
        operatingMode = MODE_RC_DATA;
        hop();
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

void getTelemetry()
{
  static unsigned long timeOfLastTelemReception = 0;

  int packetSize = LoRa.parsePacket();
  if (packetSize > 0) //received a packet
  {
    uint8_t msgBuff[30];
    memset(msgBuff, 0, sizeof(msgBuff));
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
    
    hop();
    
    //Check if packet is valid and extract the data
    if(checkPacket(receiverID, transmitterID, PAC_TELEMETRY, msgBuff, packetSize))
    {
      //check length
      if((msgBuff[2] & 0x0F) == 3) //3 bytes
      {
        timeOfLastTelemReception = millis();
        //extract
        receiverPacketRate = msgBuff[3];
        telem_volts = ((uint16_t)msgBuff[4] << 4 & 0xFF0) | ((uint16_t)msgBuff[5] >> 4 & 0x0F);
      }
    }
    
    //Change mode
    operatingMode = MODE_RC_DATA;
  }

  //reset data if no telemetry received
  if(millis() - timeOfLastTelemReception > 3000)
  {
    receiverPacketRate = 0;
    telem_volts = 0x0FFF;
  }
}

//--------------------------------------------------------------------------------------------------

uint8_t buildPacket(uint8_t srcID, uint8_t destID, uint8_t dataIdentifier, uint8_t *dataBuff, uint8_t dataLen)
{
  // Builds packet and returns its length
  
  packet[0] = srcID;
  packet[1] = destID;
  dataLen &= 0x0F; //limit
  packet[2] = (dataIdentifier << 4) | dataLen;
  for(uint8_t i = 0; i < dataLen; i++)
  {
    packet[3 + i] = *dataBuff;
    ++dataBuff;
  }
  packet[3 + dataLen] = crc8Maxim(packet, 3 + dataLen);
  return 4 + dataLen; 
}

//--------------------------------------------------------------------------------------------------

bool checkPacket(uint8_t srcID, uint8_t destID, uint8_t dataIdentifier, uint8_t *packetBuff, uint8_t packetSize)
{
  if(packetSize < 4 || packetSize > 19) //packet may be from other Lora radios
    return false;
  
  if(packetBuff[0] != srcID || packetBuff[1] != destID || (packetBuff[2] >> 4) != dataIdentifier)
    return false;
  
  //check packet crc
  uint8_t _crcQQ = packetBuff[packetSize - 1];
  uint8_t _computedCRC = crc8Maxim(packetBuff, packetSize - 1);
  if(_crcQQ != _computedCRC)
    return false;
  
  return true;
}
