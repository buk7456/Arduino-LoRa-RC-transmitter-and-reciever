/***************************************************************************************************
* Code for receiver microcontroller
* BUK 2020
* buk7456@gmail.com

* Tested to compile on Arduino IDE 1.8.9 or later
* Sketch should compile without warnings even with -WALL
***************************************************************************************************/

#include <SPI.h>
#include "LoRa.h"
#include "crc8.h"
#include <EEPROM.h>

#include <Servo.h>
Servo servoCh1, servoCh2, servoCh3, servoCh4, servoCh5, servoCh6, servoCh7, servoCh8;

//Pins
#define PIN_CH1    2
#define PIN_CH2    5
#define PIN_CH3    3
#define PIN_CH4    4
#define PIN_CH5    A5
#define PIN_CH6    A4
#define PIN_CH7    A3
#define PIN_CH8    A2
#define PIN_CH9    A1
#define PIN_CH10   A0

#define PIN_LED_GREEN  7
#define PIN_LED_ORANGE 6

//--------------- Freq allocation --------------------

/* LPD433 Band ITU region 1
The frequencies in this UHF band lie between 433.05Mhz and 434.790Mhz with 25kHz separation for a
total of 69 freq channels. Channel_1 is 433.075 Mhz and Channel_69 is 434.775Mhz. 
All our communications have to occur on any of these 69 channels. 
*/

/* Frequency list to pick from. The separation here is 300kHz (250kHz total lora bw + 
25kHz headroom on each sides.*/
uint32_t freqList[] = {433175000, 433475000, 433775000, 434075000, 434375000, 434675000};
//bind is also transmitted on freqList[0]

uint8_t fhss_schema[3] = {0, 1, 2}; /* Index in freqList. Frequencies to use for hopping. 
These are changed when we receive a bind command from the transmitter. This schema also gets stored 
to eeprom so we don't have to rebind each time we power on. */

uint8_t ptr_fhss_schema = 0; 

#define MAX_LISTEN_TIME_ON_HOP_CHANNEL 100 //in ms. If no packet received within this time, we hop 

//--------------------------------------------------

uint16_t telem_volts = 1200;     // in 10mV, sent by receiver with 12bits.  0x0FFF "No data"

unsigned long lastValidPacketMillis = 0;

int ch1to8Vals[8] = {0,0,0,0,0,0,0,0};
uint8_t digitalChVal[2] = {0,0}; 

long ch1to8Failsafes[8] = {0,0,0,0,0,0,0,0};
bool failsafeReceived = false;

long validPacketCount = 0;
int rssi = 0;

bool telemetryRequest = false;

uint8_t transmitterID = 0xFF; //settable during bind
uint8_t receiverID = 128; //randomly generated on bind

uint8_t rfPowerLevel = 0;

//-------------- EEprom stuff --------------------

#define EE_INITFLAG         0xBB 
#define EE_ADR_INIT_FLAG    0
#define EE_ADR_TX_ID        1
#define EE_ADR_RX_ID        2
#define EE_ADR_FHSS_SCHEMA  3

//--------------- Function Declarations ----------

void readBindPacket();
void readFlyModePacket();
void hop();
void sendTelemetry();
void writeOutputs();

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
  
  // setup pins
  pinMode(PIN_LED_GREEN, OUTPUT);
  digitalWrite(PIN_LED_GREEN, HIGH);
  pinMode(PIN_LED_ORANGE, OUTPUT);
  pinMode(PIN_CH9, OUTPUT);
  pinMode(PIN_CH10, OUTPUT);
  
  delay(100);
  
  //setup lora module
  LoRa.setPins(10, 8);
  if (LoRa.begin(freqList[0]))
  {
    LoRa.setSpreadingFactor(7);
    LoRa.setCodingRate4(5);
    LoRa.setSignalBandwidth(250E3);
  }
  else //failed to init. Perhaps module isn't plugged in
  {
    //flash both LEDs
    bool ledState = HIGH;
    while(1)
    {
      digitalWrite(PIN_LED_GREEN, ledState);
      digitalWrite(PIN_LED_ORANGE,ledState);
      ledState = !ledState;
      delay(500);
    }
  }
  
  //listen for bind
  readBindPacket();
  
  //Wait for failsafe transimission before proceeding
  while(failsafeReceived == false)
  {
    readFlyModePacket();
    delay(2);
  }

  //Attach servos
  servoCh1.attach(PIN_CH1);
  servoCh2.attach(PIN_CH2);
  servoCh3.attach(PIN_CH3);
  servoCh4.attach(PIN_CH4);
  servoCh5.attach(PIN_CH5);
  servoCh6.attach(PIN_CH6);
  servoCh7.attach(PIN_CH7);
  servoCh8.attach(PIN_CH8);
}

//====================================== MAIN LOOP =================================================
void loop()
{
  readFlyModePacket();
  
  sendTelemetry();

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
  
  //Handle failsafe 
  if(millis() - lastValidPacketMillis > 1500)
  {
    rssi = 0;
    for(int i= 0; i < 8; i++)
    {
      if(ch1to8Failsafes[i] != 523) //ignore channels that have failsafe turned off.
        ch1to8Vals[i] = ch1to8Failsafes[i]; 
    }
    digitalChVal[0] = 0; //turn off chA
  }
  
  writeOutputs();

}

//==================================================================================================
void readBindPacket()
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
  
  LoRa.sleep();
  LoRa.setFrequency(freqList[0]);
  LoRa.idle();
  
  uint8_t msgBuff[20]; 
  
  const uint16_t BIND_LISTEN_TIMEOUT = 300;
  const uint16_t BIND_ACK_TIMEOUT = 100;  
  
  //---- listen for bind -----
  
  bool receivedBind = false;

  uint32_t stopTime = millis() + BIND_LISTEN_TIMEOUT;
  while(millis() < stopTime)
  {
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) //received a packet
    {
      /// read packet
      uint16_t cntr = 0;
      bool msgBuffFull = false;
      while (LoRa.available() > 0) 
      {
        if(msgBuffFull == false) //read into msgBuff
        {
          msgBuff[cntr] = LoRa.read();
          cntr++;
          if (cntr >= (sizeof(msgBuff)/sizeof(msgBuff[0]))) //prevent array out of bounds
            msgBuffFull = true; 
        }
        else // discard any extra data
          LoRa.read(); 
      }
      /// Check packet
      uint8_t crcQQ = msgBuff[12];
      uint8_t computedCRC = crc8Maxim(msgBuff, 12);
      if(crcQQ == computedCRC && (msgBuff[0] & 0x01) == 0)
      {
        receivedBind = true;
        break; //exit loop
      }
    }
    delay(2);
  }
  
  if(receivedBind == false)
  {
    hop(); //set to operating frequencies
    return; //bail out
  }
  
  //-----------------------------

  //get transmitterID and hop channels
  transmitterID = (msgBuff[0] >> 1) & 0xFF;
  for(uint8_t k = 0; k < (sizeof(fhss_schema)/sizeof(fhss_schema[0])); k++)
  {
    if(msgBuff[1 + k] < (sizeof(freqList)/sizeof(freqList[0]))) //prevents invalid references
      fhss_schema[k] = msgBuff[1 + k];
  }
  //save to eeprom
  EEPROM.write(EE_ADR_TX_ID, transmitterID);
  EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
  
  //---- send reply 
  
  uint8_t dataToTransmit[7]; 
  memset(dataToTransmit, 0, sizeof(dataToTransmit));
  
  //generate random receiverID
  randomSeed(millis()); //Seed PRNG
  receiverID = random(128, 255) & 0xFF; //allowed IDs 128 to 255
  EEPROM.write(EE_ADR_RX_ID, receiverID);
  
  dataToTransmit[0] = receiverID;
  dataToTransmit[6] = crc8Maxim(dataToTransmit, 6);
  
  unsigned long bindAckExitTime = millis() + BIND_ACK_TIMEOUT;
  while(millis() < bindAckExitTime)
  {
    if(LoRa.beginPacket())
    {
      LoRa.write(dataToTransmit, 7);
      LoRa.endPacket(); //blocking
    }
    delay(5);
  }

  //set to fly mode freq
  hop();
  
  //indicate we received bind
  for(int i = 0; i < 3; i++) //flash led 3 times
  {
    digitalWrite(PIN_LED_ORANGE, HIGH);
    delay(200);
    digitalWrite(PIN_LED_ORANGE, LOW);
    delay(200);
  }
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
void readFlyModePacket()
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
  
  static uint32_t timeOfLastPacket = millis();
  
  bool hasValidPacket = false;
 
  uint8_t msgBuff[20]; 
  
  int packetSize = LoRa.parsePacket();
  if (packetSize > 0) //received a packet
  {
    //record time packet received
    timeOfLastPacket = millis();
    
    rssi = LoRa.packetRssi();
    
    /// read packet
    uint16_t cntr = 0;
    bool msgBuffFull = false;
    while (LoRa.available() > 0) 
    {
      if(msgBuffFull == false) //read into msgBuff
      {
        msgBuff[cntr] = LoRa.read();
        cntr++;
        if (cntr >= (sizeof(msgBuff)/sizeof(msgBuff[0]))) //prevent array out of bounds
          msgBuffFull = true; 
      }
      else // discard any extra data
        LoRa.read(); 
    }
    
    /// hop
    hop();
    
    /// Check if the received data is valid based on crc and transmitterID
    uint8_t crcQQ = msgBuff[12];
    uint8_t computedCRC = crc8Maxim(msgBuff, 12) ^ receiverID;
    uint8_t txID = (msgBuff[0] >> 1) & 0xFF;
    if(crcQQ == computedCRC && txID == transmitterID && (msgBuff[0] & 0x01) == 1)
    {
      hasValidPacket = true;
      validPacketCount++;
      lastValidPacketMillis = millis();
    }
  }
  else if(millis() - timeOfLastPacket > MAX_LISTEN_TIME_ON_HOP_CHANNEL)
  {
    timeOfLastPacket = millis();
    hop();
  }
  
  
  if(hasValidPacket)
  {
    digitalWrite(PIN_LED_ORANGE, HIGH);
    
    //-------- Decode ----------
    //Proportional channels
    long _ch1to8Tmp[8];
    _ch1to8Tmp[0] = ((uint16_t)msgBuff[1] << 2 & 0x3fc) | ((uint16_t)msgBuff[2] >> 6 & 0x03); //ch1
    _ch1to8Tmp[1] = ((uint16_t)msgBuff[2] << 4 & 0x3f0) | ((uint16_t)msgBuff[3] >> 4 & 0x0f); //ch2
    _ch1to8Tmp[2] = ((uint16_t)msgBuff[3] << 6 & 0x3c0) | ((uint16_t)msgBuff[4] >> 2 & 0x3f); //ch3
    _ch1to8Tmp[3] = ((uint16_t)msgBuff[4] << 8 & 0x300) | ((uint16_t)msgBuff[5]      & 0xff); //ch4
    _ch1to8Tmp[4] = ((uint16_t)msgBuff[6] << 2 & 0x3fc) | ((uint16_t)msgBuff[7] >> 6 & 0x03); //ch5
    _ch1to8Tmp[5] = ((uint16_t)msgBuff[7] << 4 & 0x3f0) | ((uint16_t)msgBuff[8] >> 4 & 0x0f); //ch6
    _ch1to8Tmp[6] = ((uint16_t)msgBuff[8] << 6 & 0x3c0) | ((uint16_t)msgBuff[9] >> 2 & 0x3f); //ch7
    _ch1to8Tmp[7] = ((uint16_t)msgBuff[9] << 8 & 0x300) | ((uint16_t)msgBuff[10]     & 0xff); //ch8
    
    //digital channels
    digitalChVal[0] = (msgBuff[11] & 0x40) >> 6;
    digitalChVal[1] = (msgBuff[11] & 0x20) >> 5;
    
    //Check if failsafe data. If so, dont modify outputs
    if((msgBuff[11] & 0x10) == 0x10) //failsafe values
    {
      failsafeReceived = true;
      for(int i=0; i<8; i++)
        ch1to8Failsafes[i] = _ch1to8Tmp[i] - 500; //Center at 0 so range is -500 to 500
    }
    else //normal channel values
    {
      for(int i=0; i<8; i++)
        ch1to8Vals[i] = _ch1to8Tmp[i] - 500; //Center at 0 so range is -500 to 500
    }
    
    //telemetry request
    telemetryRequest = (msgBuff[11] >> 3) & 0x01;
    
    //rf power level
    rfPowerLevel = msgBuff[11] & 0x07;
  }
  
  if(!hasValidPacket && (millis() - lastValidPacketMillis > 100))
  {
    //turn off orange LED
    digitalWrite(PIN_LED_ORANGE, LOW);
  }
}

//==================================================================================================

void sendTelemetry()
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
  
  if(!telemetryRequest)
    return;
  
  telemetryRequest = false;
  
  //Calculate packets per second
  static unsigned long prevValidPacketCount = 0; 
  static unsigned long ttPrevMillis = 0;
  static uint8_t validPacketsPerSecond = 0; 
  unsigned long ttElapsed = millis() - ttPrevMillis;
  if (ttElapsed >= 1000)
  {
    ttPrevMillis = millis();
    unsigned long _validPPS = (validPacketCount - prevValidPacketCount) * 1000;
    _validPPS /= ttElapsed;
    validPacketsPerSecond = _validPPS & 0xFF;
    prevValidPacketCount = validPacketCount;
  }
  if(millis() - lastValidPacketMillis > 1000)
    validPacketsPerSecond = 0;
  
  //prepare data
  uint8_t payload[5];
  memset(payload, 0, sizeof(payload));
  payload[0] = receiverID;
  payload[1] = validPacketsPerSecond;
  payload[2] = (telem_volts >> 4) & 0xFF;
  payload[3] = ((telem_volts << 4) & 0xF0);
  payload[4] = crc8Maxim(payload, 4) ^ transmitterID;
  
  // transmit
  delay(1);
  if(LoRa.beginPacket())
  {
    LoRa.write(payload, 7);
    LoRa.endPacket(); //block until done transmitting
    hop();
  }
}

//==================================================================================================

void writeOutputs()
{ 
  int16_t ch1to8MicroSec[8];
  for(uint8_t i = 0; i<8; i++)
    ch1to8MicroSec[i] = map(ch1to8Vals[i], -500, 500, 1000, 2000);

  servoCh1.writeMicroseconds(ch1to8MicroSec[0]);
  servoCh2.writeMicroseconds(ch1to8MicroSec[1]);
  servoCh3.writeMicroseconds(ch1to8MicroSec[2]);
  servoCh4.writeMicroseconds(ch1to8MicroSec[3]);
  servoCh5.writeMicroseconds(ch1to8MicroSec[4]);
  servoCh6.writeMicroseconds(ch1to8MicroSec[5]);
  servoCh7.writeMicroseconds(ch1to8MicroSec[6]);
  servoCh8.writeMicroseconds(ch1to8MicroSec[7]);
  
  digitalWrite(PIN_CH9,  digitalChVal[0]);
  digitalWrite(PIN_CH10, digitalChVal[1]);
}
