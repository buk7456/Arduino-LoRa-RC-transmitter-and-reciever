/***************************************************************************************************
* Code for receiver microcontroller
* BUK 2020
* buk7456@gmail.com

* Tested to compile on Arduino IDE 1.8.9 or later
* Sketch should compile without warnings even with -WALL
***************************************************************************************************/

//Comment to disable serial print out
// #define DEBUG

#include <SPI.h>
#include "LoRa.h"
#include "crc8.h"
#include <EEPROM.h>

//----------------------------------------------
//Pins
#define CH1PIN 2
#define CH2PIN 5
#define CH3PIN 3
#define CH4PIN 4
#define CH5PIN A5
#define CH6PIN A4
#define CH7PIN A3
#define CH8PIN A2
#define CH9PIN A1
#define CH10PIN A0
#define GREEN_LED_PIN 7
#define ORANGE_LED_PIN 6

//-----------------------------------------------
#define SERVO_PULSE_RANGE 1000
#define SERVO_MAX_ANGLE 100 

#include <Servo.h>
Servo servoCh1, servoCh2, servoCh3, servoCh4, servoCh5, servoCh6, servoCh7, servoCh8;

long lowerLimMicroSec, upperLimMicroSec;

//--------------------------------------------------
enum{PWM_SERVO = 1, PWM_NORM = 0};
uint8_t PWM_Mode_Ch3 = PWM_SERVO;

unsigned long lastValidPacketMillis = 0;

uint8_t msgBuff[20]; //buffer for incoming message bytes

long ch1to8Vals[8] = {0,0,0,0,0,0,0,0};
uint8_t digitalChVal[2] = {0,0}; 

long ch1to8Failsafes[8] = {0,0,0,0,0,0,0,0};
bool failsafeReceived = false;

long validPacketCount = 0; //debug
int rssi = 0; //debug

uint8_t transmitterID = 0xFF; //settable during bind

#define BIND_TIMEOUT  100 //in ms. Should be low to enable fast startup after a brownout in flight

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

//-------------- EEprom stuff --------------------
#define EE_INITFLAG         0xBA 
#define EE_ADR_INIT_FLAG    0
#define EE_ADR_TX_ID        1
#define EE_ADR_FHSS_SCHEMA  2

//--------------- Function Declarations ----------

void readBindPacket();
void readFlyModePacket();
void hop();
void writeOutputs();
void printDebugData();

//==================================================================================================
void setup()
{ 
  // EEPROM init
  if (EEPROM.read(EE_ADR_INIT_FLAG) != EE_INITFLAG)
  {
    EEPROM.write(EE_ADR_TX_ID, transmitterID);
    EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
    EEPROM.write(EE_ADR_INIT_FLAG, EE_INITFLAG);
  }
  
  // Read from EEPROM
  transmitterID = EEPROM.read(EE_ADR_TX_ID);
  EEPROM.get(EE_ADR_FHSS_SCHEMA, fhss_schema);
  
  // setup pins
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, HIGH);
  pinMode(ORANGE_LED_PIN, OUTPUT);
  pinMode(CH9PIN, OUTPUT);
  pinMode(CH10PIN, OUTPUT);
  
  //calc servo ranges
  long _microsec = SERVO_PULSE_RANGE / 2;
  _microsec *= SERVO_MAX_ANGLE;
  _microsec /= 90;
  lowerLimMicroSec = 1500 - _microsec;  
  upperLimMicroSec = 1500 + _microsec;
  
#if defined (DEBUG)
  Serial.begin(115200);
#endif

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
      digitalWrite(GREEN_LED_PIN, ledState);
      digitalWrite(ORANGE_LED_PIN,ledState);
      ledState = !ledState;
      delay(500);
    }
  }
  
  //listen for bind
  readBindPacket(); //blocking
  
  //Wait for failsafe transimission before proceeding
  while(failsafeReceived == false)
  {
    readFlyModePacket();
    delay(2);
  }

  //Attach servos
  servoCh1.attach(CH1PIN);
  servoCh2.attach(CH2PIN);
  if(PWM_Mode_Ch3 == PWM_SERVO)
    servoCh3.attach(CH3PIN);
  servoCh4.attach(CH4PIN);
  servoCh5.attach(CH5PIN);
  servoCh6.attach(CH6PIN);
  servoCh7.attach(CH7PIN);
  servoCh8.attach(CH8PIN);
}

//====================================== MAIN LOOP =================================================
void loop()
{
  readFlyModePacket();
  
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

#if defined (DEBUG)
  printDebugData();
#endif
}

//==================================================================================================
void readBindPacket()
{
  /*Air protocol Format  
    Byte0 - bit 7 to 1 transmitterID, bit 0 packet type 0 for bind, 
    Byte1toByteN<12 - hop channels, 
    Byte12 - crc8
  */
  
  LoRa.sleep();
  LoRa.setFrequency(freqList[0]);
  LoRa.idle();
  
  //listen for bind
  bool receivedBind = false;
  uint32_t stopTime = millis() + BIND_TIMEOUT;
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

  //get transmitterID
  transmitterID = (msgBuff[0] >> 1) & 0xFF;
  
  //get hop channels
  for(uint8_t k = 0; k < (sizeof(fhss_schema)/sizeof(fhss_schema[0])); k++)
  {
    if(msgBuff[1 + k] < (sizeof(freqList)/sizeof(freqList[0]))) //prevents invalid references
      fhss_schema[k] = msgBuff[1 + k];
  }
  
  //save to eeprom
  EEPROM.write(EE_ADR_TX_ID, transmitterID);
  EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
  
  //set to fly mode freq
  hop();
  
  //indicate a successful bind
  for(int i = 0; i < 3; i++) //flash led 3 times
  {
    digitalWrite(ORANGE_LED_PIN, HIGH);
    delay(200);
    digitalWrite(ORANGE_LED_PIN, LOW);
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
  /* Air protocol format
  
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
    abpf0000  CCCCCCCC
    ------------------------------------------------
    
    Servo Chs 1 to 8 encoded as 10 bits      
    a is digital chA bit
    b is digital ChB bit 
    p is PWM mode bit for ch3
    f is failsafe flag
    C is the CRC
  */
  
  static uint32_t timeOfLastPacket = millis();
  
  bool hasValidPacket = false;
  
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
    uint8_t computedCRC = crc8Maxim(msgBuff, 12);
    uint8_t receivedID = (msgBuff[0] >> 1) & 0xFF;
    if(crcQQ == computedCRC && receivedID == transmitterID && (msgBuff[0] & 0x01) == 1)
    {
      hasValidPacket = true;
      validPacketCount++;
      lastValidPacketMillis = millis();
    }
  }
  else if(millis() - timeOfLastPacket > MAX_LISTEN_TIME_ON_HOP_CHANNEL)
  {
    //reset
    timeOfLastPacket = millis();
    //hop
    hop();
  }
  
  
  if(hasValidPacket)
  {
    digitalWrite(ORANGE_LED_PIN, HIGH);
    
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
    digitalChVal[0] = (msgBuff[11] & 0x80) >> 7;
    digitalChVal[1] = (msgBuff[11] & 0x40) >> 6;
    
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
    
    //get pwm mode for Ch3. Only set once. If changed in transmitter, receiver should be restarted
    static bool _pwmModeInitialised = false;
    if(_pwmModeInitialised == false)
    {
      PWM_Mode_Ch3 = (msgBuff[11] & 0x20) >> 5;
      _pwmModeInitialised = true;
    }
  }
  else
  {
    //turn off orange LED
    if(millis() - lastValidPacketMillis > 50) 
      digitalWrite(ORANGE_LED_PIN, LOW);
  }
}

//==================================================================================================
void writeOutputs()
{ 
  int16_t ch1to8MicroSec[8];
  for(uint8_t i = 0; i<8; i++)
  {
    ch1to8MicroSec[i] = map(ch1to8Vals[i],-500,500,lowerLimMicroSec, upperLimMicroSec);
  }
  servoCh1.writeMicroseconds(ch1to8MicroSec[0]);
  servoCh2.writeMicroseconds(ch1to8MicroSec[1]);
  
  if(PWM_Mode_Ch3 == PWM_SERVO)
    servoCh3.writeMicroseconds(ch1to8MicroSec[2]);
  else if(PWM_Mode_Ch3 == PWM_NORM)
  {
    long qq = map(ch1to8Vals[2], -500, 500, 0, 255);
    uint8_t val = qq & 0xFF;
    analogWrite(CH3PIN, val);
  }
  
  servoCh4.writeMicroseconds(ch1to8MicroSec[3]);
  servoCh5.writeMicroseconds(ch1to8MicroSec[4]);
  servoCh6.writeMicroseconds(ch1to8MicroSec[5]);
  servoCh7.writeMicroseconds(ch1to8MicroSec[6]);
  servoCh8.writeMicroseconds(ch1to8MicroSec[7]);
  
  digitalWrite(CH9PIN,  digitalChVal[0]);
  digitalWrite(CH10PIN, digitalChVal[1]);
}

//=========================== DEBUG ================================================================
void printDebugData()
{
  //Calculate packets per second
  static unsigned long prevValidPacketCount = 0; 
  static unsigned long ttPrevMillis = 0;
  uint8_t validPacketsPerSecond; 
  unsigned long ttElapsed = millis() - ttPrevMillis;
  if (ttElapsed >= 1000)
  {
    ttPrevMillis = millis();
    unsigned long _validPPS = (validPacketCount - prevValidPacketCount) * 1000;
    _validPPS /= ttElapsed;
    validPacketsPerSecond = uint8_t(_validPPS);
    prevValidPacketCount = validPacketCount;
  }
  
  //print to serial
  static uint32_t serialLastPrintTT = millis();
  if(millis() - serialLastPrintTT >= 40)
  {
    serialLastPrintTT = millis();
    Serial.print("RSSI:");
    Serial.print(rssi);
    Serial.print(" PPS:");
    Serial.print(validPacketsPerSecond);
    Serial.print(" Ch: ");
    for(uint8_t i = 0; i < 8; i++)
    {
      Serial.print(ch1to8Vals[i]/5); 
      Serial.print(F(" "));
    }
    Serial.print(digitalChVal[0]);  //Ch9
    Serial.print(F(" "));
    Serial.print(digitalChVal[1]);  //Ch10
    Serial.println();
  }
}
