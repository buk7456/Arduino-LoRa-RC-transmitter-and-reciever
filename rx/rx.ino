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

#define PIN_EXTV_SENSE A0

#define PIN_LED_GREEN  7
#define PIN_LED_ORANGE 6

//--------------- Freq allocation --------------------

/* LPD433 Band ITU region 1
The frequencies in this UHF band lie between 433.05Mhz and 434.790Mhz with 25kHz separation for a
total of 69 freq channels. Channel_1 is 433.075 Mhz and Channel_69 is 434.775Mhz. 
All our communications have to occur on any of these 69 channels. 
*/

/* Frequency list to pick from. The separation here is 300kHz (250kHz total lora bw + 
50kHz guard band.*/
uint32_t freqList[] = {433175000, 433475000, 433775000, 434075000, 434375000, 434675000};
//bind is also transmitted on freqList[0]

uint8_t fhss_schema[3] = {0, 1, 2}; /* Index in freqList. Frequencies to use for hopping. 
These are changed when we receive a bind command from the transmitter. This schema also gets stored 
to eeprom so we don't have to rebind each time we power on. */

uint8_t idx_fhss_schema = 0; 

#define MAX_LISTEN_TIME_ON_HOP_CHANNEL 112 //in ms. If no packet received within this time, we hop 

//--------------------------------------------------

uint8_t transmitterID = 0; //settable during bind
uint8_t receiverID = 0;    //randomly generated on bind

uint8_t idxRFPowerLevel = 0;

#define MAX_PACKET_SIZE  19
uint8_t Packet[MAX_PACKET_SIZE];

enum{
  PAC_BIND                   = 0x0,
  PAC_ACK_BIND               = 0x1,
  PAC_READ_OUTPUT_CH_CONFIG  = 0x2,
  PAC_SET_OUTPUT_CH_CONFIG   = 0x3,
  PAC_ACK_OUTPUT_CH_CONFIG   = 0x4,
  PAC_RC_DATA                = 0x5,
  PAC_TELEMETRY              = 0x6,
};


bool isRequestingTelemetry = false;
uint16_t externalVolts = 0; //in millivolts
uint16_t telem_volts = 0x0FFF;     // in 10mV, sent by receiver with 12bits.  0x0FFF "No data"

bool failsafeEverBeenReceived = false;

uint32_t rcPacketCount = 0;
uint32_t lastRCPacketMillis = 0;

int ch1to9Vals[9];
int ch1to9Failsafes[9];

uint8_t OutputChConfig[9]; //0 digital, 1 Servo, 2 PWM

uint8_t maxOutputChConfig[9];

//Declare an array of servo objects
Servo myServo[9];

//Declare an output pins array
int myOutputPins[9] = {PIN_CH1, PIN_CH2, PIN_CH3, PIN_CH4, PIN_CH5, PIN_CH6, PIN_CH7, PIN_CH8, PIN_CH9};

//-------------- EEprom stuff --------------------

#define EE_INITFLAG         0xBB 

#define EE_ADR_INIT_FLAG    0
#define EE_ADR_TX_ID        1
#define EE_ADR_RX_ID        2
#define EE_ADR_FHSS_SCHEMA  3
#define EE_ADR_RX_CH_CONFIG 20


//--------------- Function Declarations ----------

void bind();
void hop();
void sendTelemetry();
void writeOutputs();
void getExternalVoltage();
uint8_t buildPacket(uint8_t srcID, uint8_t destID, uint8_t dataIdentifier, uint8_t *dataBuff, uint8_t dataLen);
bool checkPacket(uint8_t srcID, uint8_t destID, uint8_t dataIdentifier, uint8_t *packetBuff, uint8_t packetSize);
uint8_t getMaxOutputChConfig(int pin);

//==================================================================================================

void setup()
{ 
  // initialise values
  for(uint8_t i = 0; i < 9; ++i)
  {
    ch1to9Vals[i] = 0;
    ch1to9Failsafes[i] = 0;
    
    OutputChConfig[i] = 1;
    maxOutputChConfig[i] = getMaxOutputChConfig(myOutputPins[i]);
  }

  // EEPROM init
  if (EEPROM.read(EE_ADR_INIT_FLAG) != EE_INITFLAG)
  {
    EEPROM.write(EE_ADR_TX_ID, transmitterID);
    EEPROM.write(EE_ADR_RX_ID, receiverID);
    EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
    EEPROM.write(EE_ADR_INIT_FLAG, EE_INITFLAG);
    EEPROM.put(EE_ADR_RX_CH_CONFIG, OutputChConfig);
  }
  
  // Read from EEPROM
  transmitterID = EEPROM.read(EE_ADR_TX_ID);
  receiverID = EEPROM.read(EE_ADR_RX_ID);
  EEPROM.get(EE_ADR_FHSS_SCHEMA, fhss_schema);
  EEPROM.get(EE_ADR_RX_CH_CONFIG, OutputChConfig);
  
  // setup pins
  pinMode(PIN_LED_GREEN, OUTPUT);
  digitalWrite(PIN_LED_GREEN, HIGH);
  pinMode(PIN_LED_ORANGE, OUTPUT);

  //use analog reference internal 1.1V
  analogReference(INTERNAL);
  
  //setup lora module
  delay(100);
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
  
  //bind
  bind();
  
}

//====================================== MAIN LOOP =================================================
void loop()
{
  //---------- READ INCOMING PACKET (NONBIND PACKETS) ---------- 
  
  static uint32_t timeOfLastPacket = millis();
  if(millis() - timeOfLastPacket > MAX_LISTEN_TIME_ON_HOP_CHANNEL)
  {
    timeOfLastPacket = millis();
    hop();
  }
  
  bool hasValidPacket = false;
  uint8_t packetType = 0xFF;
  uint8_t dataBuff[32];
  memset(dataBuff, 0, sizeof(dataBuff));
  
  int packetSize = LoRa.parsePacket();
  if (packetSize > 0) //received a packet
  {
    timeOfLastPacket = millis();
    
    //read into temporary buffer
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
    
    //hop frequency regardless
    hop();
    
    //check packet 
    if(checkPacket(transmitterID, receiverID, PAC_RC_DATA, msgBuff, packetSize))
    {
      if((msgBuff[2] & 0x0F) == 12)
      {
        hasValidPacket = true;
        packetType = PAC_RC_DATA;
        memcpy(dataBuff, msgBuff + 3, 12);
      }
    }
    else if(checkPacket(transmitterID, receiverID, PAC_READ_OUTPUT_CH_CONFIG, msgBuff, packetSize))
    {
      hasValidPacket = true;
      packetType = PAC_READ_OUTPUT_CH_CONFIG;
    }
    else if(checkPacket(transmitterID, receiverID, PAC_SET_OUTPUT_CH_CONFIG, msgBuff, packetSize))
    {
      if((msgBuff[2] & 0x0F) == 9)
      {
        hasValidPacket = true;
        packetType = PAC_SET_OUTPUT_CH_CONFIG;
        memcpy(dataBuff, msgBuff + 3, 9);
      }
    }
  }
  
  if(hasValidPacket)
  {
    hasValidPacket = false;
    
    switch(packetType)
    {
      case PAC_RC_DATA:
        {
          ++rcPacketCount;
          
          lastRCPacketMillis = millis();
          digitalWrite(PIN_LED_ORANGE, HIGH);
    
          //Decode
          int ch1to9Tmp[9];
          ch1to9Tmp[0] = ((uint16_t)dataBuff[0] << 2 & 0x3fc) |  ((uint16_t)dataBuff[1] >> 6 & 0x03); //ch1
          ch1to9Tmp[1] = ((uint16_t)dataBuff[1] << 4 & 0x3f0) |  ((uint16_t)dataBuff[2] >> 4 & 0x0f); //ch2
          ch1to9Tmp[2] = ((uint16_t)dataBuff[2] << 6 & 0x3c0) |  ((uint16_t)dataBuff[3] >> 2 & 0x3f); //ch3
          ch1to9Tmp[3] = ((uint16_t)dataBuff[3] << 8 & 0x300) |  ((uint16_t)dataBuff[4]      & 0xff); //ch4
          ch1to9Tmp[4] = ((uint16_t)dataBuff[5] << 2 & 0x3fc) |  ((uint16_t)dataBuff[6] >> 6 & 0x03); //ch5
          ch1to9Tmp[5] = ((uint16_t)dataBuff[6] << 4 & 0x3f0) |  ((uint16_t)dataBuff[7] >> 4 & 0x0f); //ch6
          ch1to9Tmp[6] = ((uint16_t)dataBuff[7] << 6 & 0x3c0) |  ((uint16_t)dataBuff[8] >> 2 & 0x3f); //ch7
          ch1to9Tmp[7] = ((uint16_t)dataBuff[8] << 8 & 0x300) |  ((uint16_t)dataBuff[9]     & 0xff);  //ch8
          ch1to9Tmp[8] = ((uint16_t)dataBuff[10] << 2 & 0x3fc) | ((uint16_t)dataBuff[11] >> 6 & 0x03); //ch9
          
          //Check if failsafe data. If so, dont modify outputs
          if((dataBuff[11] >> 4) & 0x01) //failsafe values
          {
            failsafeEverBeenReceived = true;
            for(int i = 0; i < 9; i++)
              ch1to9Failsafes[i] = ch1to9Tmp[i] - 500; //Center at 0 so range is -500 to 500
          }
          else //normal channel values
          {
            for(int i = 0; i < 9; i++)
              ch1to9Vals[i] = ch1to9Tmp[i] - 500; //Center at 0 so range is -500 to 500
          }
          
          //telemetry request
          isRequestingTelemetry = (dataBuff[11] >> 3) & 0x01;
          
          //rf power level
          idxRFPowerLevel = dataBuff[11] & 0x07;
          
          //if requested for receiver channel config
          if((dataBuff[11] >> 5) & 0x01)
          {
            
          }
        }
        break;
        
      case PAC_READ_OUTPUT_CH_CONFIG:
        {
          //reply with the configuration
          
          //encode as follows: high nibble --> max supported config, low nibble --> the present output config
          uint8_t _configData[9];
          for(uint8_t i = 0; i < 9; i++)
            _configData[i] = (maxOutputChConfig[i] << 4) | (OutputChConfig[i] & 0x0F);
          
          uint8_t _packetLen = buildPacket(receiverID, transmitterID, PAC_READ_OUTPUT_CH_CONFIG, _configData, sizeof(_configData));
          
          delay(2);
          if(LoRa.beginPacket())
          {
            LoRa.write(Packet, _packetLen);
            LoRa.endPacket(); //block until done transmitting
            hop();
          }
        }
        break;
      
      case PAC_SET_OUTPUT_CH_CONFIG:
        {
          //save config to eeprom. Changes can only be applied on boot.
          uint8_t _outputChannelConfig[9];
          memcpy(_outputChannelConfig, dataBuff, 9);
          EEPROM.put(EE_ADR_RX_CH_CONFIG, _outputChannelConfig);
          
          //reply 
          delay(2);
          uint8_t _packetLen = buildPacket(receiverID, transmitterID, PAC_ACK_OUTPUT_CH_CONFIG, NULL, 0);
          if(LoRa.beginPacket())
          {
            LoRa.write(Packet, _packetLen);
            LoRa.endPacket(); //block until done transmitting
            hop();
          }
        }
        break;
    }
  }
  
  //---------- TURN OFF LED TO INDICATE NO INCOMING RC DATA ---------- 
  
  if(millis() - lastRCPacketMillis > 100)
    digitalWrite(PIN_LED_ORANGE, LOW);
  
  //---------- SET POWER LEVEL ----------
  
  static uint8_t prevIdxRFPowerLevel = 0xFF;
  if(idxRFPowerLevel != prevIdxRFPowerLevel)
  {
    prevIdxRFPowerLevel = idxRFPowerLevel;
    uint8_t power_dBm[5] = {3, 7, 10, 14, 17}; //2mW, 5mW, 10mW, 25mW, 50mW
    LoRa.sleep();
    LoRa.setTxPower(power_dBm[idxRFPowerLevel]);
    LoRa.idle();
  }

  //---------- FAILSAFE ----------
  
  if(millis() - lastRCPacketMillis > 1500)
  {
    for(int i= 0; i < 9; i++)
    {
      if(ch1to9Failsafes[i] != 523) //ignore channels that have failsafe turned off.
        ch1to9Vals[i] = ch1to9Failsafes[i]; 
    }
  }

  //---------- SEND TO OUTPUT CHANNELS ---------- 
  
  writeOutputs();

  //---------- EXTERNAL VOLAGE ------------------
  
  getExternalVoltage();
  
  //---------- SEND TELEMETRY TO TRANSMITTER ---------- 
  
  if(isRequestingTelemetry)
  {
    sendTelemetry();
    isRequestingTelemetry = false;
  }

}

//==================================================================================================

void bind()
{
  LoRa.sleep();
  LoRa.setFrequency(freqList[0]);
  LoRa.idle();
  
  const uint16_t BIND_LISTEN_TIMEOUT = 300;
  
  uint8_t msgBuff[30];
  memset(msgBuff, 0, sizeof(msgBuff));
  
  //---- listen for bind -----
  
  bool receivedBind = false;

  uint32_t stopTime = millis() + BIND_LISTEN_TIMEOUT;
  while(millis() < stopTime)
  {
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) //received a packet
    {
      //read into buffer
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
      
      // Check packet
      if( checkPacket(msgBuff[0], 0x00, PAC_BIND, msgBuff, packetSize) && msgBuff[0] > 0x00)
      {
        if((msgBuff[2] & 0x0F) == sizeof(fhss_schema)/sizeof(fhss_schema[0])) //check length 
        {
          receivedBind = true;
          break; //exit while loop
        }
      }
    }
    delay(2);
  }
  
  if(!receivedBind)
  {
    hop(); //set to operating frequencies
    return; //bail out
  }
  
  if(receivedBind)
  {
    //get transmitterID and hop channels
    transmitterID = msgBuff[0];
    for(uint8_t i = 0; i < (sizeof(fhss_schema)/sizeof(fhss_schema[0])); i++)
    {
      if(msgBuff[3 + i] < (sizeof(freqList)/sizeof(freqList[0]))) //prevents invalid references
        fhss_schema[i] = msgBuff[3 + i];
    }
    
    //save to eeprom
    EEPROM.write(EE_ADR_TX_ID, transmitterID);
    EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
    
    //---- send reply 
    
    uint8_t dataToSend[1]; 
    
    //generate random receiverID
    randomSeed(millis()); //Seed PRNG
    receiverID = random(0x01, 0xFF);
    EEPROM.write(EE_ADR_RX_ID, receiverID);
    dataToSend[0] = receiverID;
    delay(2);
    uint8_t _packetLen = buildPacket(0x00, transmitterID, PAC_ACK_BIND, dataToSend, sizeof(dataToSend));
    if(LoRa.beginPacket())
    {
      LoRa.write(Packet, _packetLen);
      LoRa.endPacket(); //blocking
    }

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
}

//==================================================================================================

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

//==================================================================================================

void sendTelemetry()
{
  //Calculate packets per second
  static uint32_t prevRCPacketCount = 0; 
  static uint32_t ttPrevMillis = 0;
  static uint8_t rcPacketsPerSecond = 0; 
  uint32_t ttElapsed = millis() - ttPrevMillis;
  if (ttElapsed >= 1000)
  {
    ttPrevMillis = millis();
    rcPacketsPerSecond = ((rcPacketCount - prevRCPacketCount) * 1000) / ttElapsed;
    prevRCPacketCount = rcPacketCount;
  }
  if(millis() - lastRCPacketMillis > 1000)
    rcPacketsPerSecond = 0;
  
  //prepare data and transmit
  
  uint8_t dataToSend[3];
  dataToSend[0] = rcPacketsPerSecond;
  
  if(externalVolts < 2000 || millis() < 5000UL)
    telem_volts = 0x0FFF;  //no data
  else
    telem_volts = externalVolts / 10; //convert to 10mV scale
  
  dataToSend[1] = (telem_volts >> 4) & 0xFF;
  dataToSend[2] = ((telem_volts << 4) & 0xF0);
  
  uint8_t _packetLen = buildPacket(receiverID, transmitterID, PAC_TELEMETRY, dataToSend, sizeof(dataToSend));
  
  delay(1);
  if(LoRa.beginPacket())
  {
    LoRa.write(Packet, _packetLen);
    LoRa.endPacket(); //block until done transmitting
    hop();
  }
}

//==================================================================================================

uint8_t buildPacket(uint8_t srcID, uint8_t destID, uint8_t dataIdentifier, uint8_t *dataBuff, uint8_t dataLen)
{
  // Builds Packet and returns its length
  
  Packet[0] = srcID;
  Packet[1] = destID;
  dataLen &= 0x0F; //limit
  Packet[2] = (dataIdentifier << 4) | dataLen;
  for(uint8_t i = 0; i < dataLen; i++)
  {
    Packet[3 + i] = *dataBuff;
    ++dataBuff;
  }
  Packet[3 + dataLen] = crc8Maxim(Packet, 3 + dataLen);
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

//==================================================================================================

void writeOutputs()
{ 
  if(!failsafeEverBeenReceived)
    return;
  
  static bool outputsInitialised = false;
  
  if(!outputsInitialised)
  {
    //setup outputs
    for(uint8_t i = 0; i < 9; i++)
    {
      if(OutputChConfig[i] == 0)
        pinMode(myOutputPins[i], OUTPUT);
      else if(OutputChConfig[i] == 1)
        myServo[i].attach(myOutputPins[i]);
    }
    outputsInitialised = true;
  }
  
  for(uint8_t i = 0; i < 9; i++)
  {
    if(OutputChConfig[i] == 0)     //digital mode
    {
      //range -500 to -250 becomes LOW
      //range -250 to 250 is ignored
      //range 250 to 500 becomes HIGH
      if(ch1to9Vals[i] <= -250)
        digitalWrite(myOutputPins[i], LOW);
      else if(ch1to9Vals[i] >= 250)
        digitalWrite(myOutputPins[i], HIGH);
    }
    else if(OutputChConfig[i] == 1) //servo mode
    {
      int val = map(ch1to9Vals[i], -500, 500, 1000, 2000);
      val = constrain(val, 1000, 2000);
      myServo[i].writeMicroseconds(val);
    }
    else if(OutputChConfig[i] == 2) //pwm mode
    {
      int val = map(ch1to9Vals[i], -500, 500, 0, 255);
      val = constrain(val, 0, 255);
      analogWrite(myOutputPins[i], val);
    }
  }
}

//==================================================================================================

uint8_t getMaxOutputChConfig(int pin)
{
  int pwmPins[] = {5, 6, 9, 10, 3, 11}; //on arduino uno
  
  uint8_t rslt = 1;
  
  //search through array
  for(uint8_t i = 0; i < (sizeof(pwmPins)/sizeof(pwmPins[0])); i++)
  {
    if(pwmPins[i] == pin)
    {
      rslt = 2;
      break;
    }
  }
  
  return rslt;
}

//==================================================================================================

void getExternalVoltage()
{
  /* 
  Apply smoothing to measurement using exponential recursive smoothing
  It works by subtracting out the mean each time, and adding in a new point. 
  _NUM_SAMPLES parameter defines number of samples to average over. Higher value results in slower
  response.
  Formula x = x - x/n + a/n  
  */
  static uint32_t _lastMillis = 0;
  if(millis() - _lastMillis < 10)
  {
    return;
  }
  _lastMillis = millis();
  
  const int _NUM_SAMPLES = 30;
  const int VFactor = 1627; //adjust this for correction. (##TODO possibly change this on transmitter side)
  long _millivolts = ((long)analogRead(PIN_EXTV_SENSE) * VFactor) / 100;
  _millivolts = ((long)externalVolts * (_NUM_SAMPLES - 1) + _millivolts) / _NUM_SAMPLES; 
  externalVolts = int(_millivolts); 
}
