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
const int buzzerPin = 9;
const int lcdBacklightPin = 6;
const int SwCUpperPosPin = A4; //3pos switch
const int SwCLowerPosPin = A5; //3pos switch

const int msgLength = 21;  //bytes in master's message including start and stop
uint8_t receivedData[msgLength];  //Holds the valid data from master mcu
bool hasValidSerialMsg = false;

uint8_t transmitterID; //got from master mcu

bool radioInitialised = false;
unsigned long radioTotalPackets = 0;

//--------------- binding ---------------------
//define the frequency to use for binding
const uint32_t bindFreq = 433000000;

//--------------- frequency hopping -----------
//Frequency list to pick from. The separation here is 250kHz. All must be within the specific ISM band
uint32_t freqList[] = {433250000, 433500000, 433750000, 434000000, 434250000, 434500000, 434750000};

uint8_t fhss_schema[3] = {0, 1, 2}; /* Index in freqList. Frequencies to use for hopping. 
These are changed when we receive a bind command from the master mcu. This schema also gets stored 
to eeprom so we don't have to rebind each time we switch on the transmitter. */

uint8_t ptr_fhss_schema = 0; 

//-------------- EEprom stuff --------------------
#define EE_INITFLAG         0xBA 
#define EE_ADR_INIT_FLAG    0
#define EE_ADR_FHSS_SCHEMA  2

//-------------- Audio --------------------------
enum{  
  AUDIO_NONE = 0, 
  AUDIO_BATTERYWARN, AUDIO_THROTTLEWARN, AUDIO_TIMERELAPSED,
  AUDIO_SWITCHMOVED, AUDIO_TRIMSELECTED,
  AUDIO_KEYTONE      
};

uint8_t audioToPlay = AUDIO_NONE;
uint8_t lastAudioToPlay = AUDIO_NONE;

//Sounds in rtttl format
const char* battLowSound = "battlow2:d=4,o=5,b=290:4c6,32p,4a#,32p,4g.";
const char* warnSound = "warn:d=4,o=4,b=160:4b5";
const char* shortBeepSound = "shortBeep:d=4,o=4,b=250:16c#7";
const char* timerElapsedSound = "timerElapsed:d=4,o=5,b=210:16b6,16p,8b6";
const char* trimSelectSound = "trimSelect:d=4,o=4,b=160:16c#5";

//==================================================================================================

void setup()
{
  // EEPROM init
  if (EEPROM.read(EE_ADR_INIT_FLAG) != EE_INITFLAG)
  {
    EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
    EEPROM.write(EE_ADR_INIT_FLAG, EE_INITFLAG);
  }
  
  // Read from EEPROM
  EEPROM.get(EE_ADR_FHSS_SCHEMA, fhss_schema);
  
  //seed random number generator
  randomSeed(analogRead(2));
  
  //Clear receivedData to prevent unexpected results
  memset(receivedData, 0, sizeof(receivedData)); 

  //setup pins
  pinMode(lcdBacklightPin,  OUTPUT);
  pinMode(buzzerPin,  OUTPUT);
  pinMode(SwCUpperPosPin, INPUT_PULLUP);
  pinMode(SwCLowerPosPin, INPUT_PULLUP);
  
  //init serial port
  Serial.begin(115200);
  delay(500);
  
  //setup lora module
  LoRa.setPins(10, 8); 
  if (LoRa.begin(bindFreq))
  {
    LoRa.setSpreadingFactor(7); //default is 7
  
    LoRa.setSignalBandwidth(250E3);
    // signalBandwidth - signal bandwidth in Hz, defaults to 125E3.
    // Supported values are 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3.
    
    LoRa.setCodingRate4(5);
    /*codingRateDenominator - denominator of the coding rate, defaults to 5
    Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. 
    The coding rate numerator is fixed at 4. */
  
    radioInitialised = true;
  }
  else
    radioInitialised = false;
  
}

//========================================= MAIN ==================================================
void loop()
{
  /// ---------- READ THE SERIAL DATA ---------------------
  getSerialData();

  ///----------- CONTROL LCD BACKLIGHT --------------------
  digitalWrite(lcdBacklightPin, (receivedData[1] >> 6) & 0x01);
  
  ///----------- SEND VIA RF MODULE -----------------------
  transmitterID = receivedData[19];
  
  if((receivedData[1] >> 5) & 0x01) //bind command received
    bind();  
  else 
    transmitServoData();
  
  ///----------- PLAY TONES ----------------------------
  playTones();
}

//==================================================================================================

void getSerialData()
{
  hasValidSerialMsg = false; //reset flag
  if (Serial.available() < msgLength)
  {
    return;
  }
  
  /** Master to Slave MCU communication format as below

  byte 0 - Start of message 0xBB
  byte 1 - Status byte 
      bit7 - always 0
      bit6 - Backlight: 1 on, 0 off
      bit5 - Bind Status: 1 bind phase, 0 operating phase
      bit4 - RF module: 1 on, 0 off 
      bit3 - DigChA: 1 on, 0 off
      bit2 - DigChB: 1 on, 0 off
      bit1 - PWM mode for Ch3: 1 means servo pwm, 0 ordinary pwm
      bit0 - Failsafe: 1 means failsafe data, 0 normal data
      
  byte 2 to 17 - Ch1 thru 8 data (2 bytes per channel, total 16 bytes)
  byte 18- Sound to play  (1 byte)
  byte 19- transmitterID
  byte 20- End of message 0xDD
  */

  uint8_t tmpBuff[64];
  memset(tmpBuff, 0, sizeof(tmpBuff)); 
  bool tmpBuffIsFull = false;
  uint8_t cntr = 0;

  while (Serial.available() > 0)
  {
    if (tmpBuffIsFull == false) //put data into tmpBuff
    {
      tmpBuff[cntr] = Serial.read();
      cntr++;
      if (cntr >= (sizeof(tmpBuff) / sizeof(tmpBuff[0]))) //prevent array out of bounds
        tmpBuffIsFull = true;
    }
    else //Discard any extra bytes
      Serial.read();
  }

  //Check for first occurence of start and stop bytes in tmpBuff
  uint8_t startByteIndex = 0, stopByteIndex = 0;
  for (uint8_t i = 0; i < (sizeof(tmpBuff) / sizeof(tmpBuff[0])); i++)
  {
    if (tmpBuff[i] == 0xBB)
      startByteIndex = i;
    else if (tmpBuff[i] == 0xDD)
    {
      stopByteIndex = i;
      break;
    }
  }

  //Copy to receivedData if good. Also send back pkts/sec and swc state
  if ((stopByteIndex - startByteIndex + 1) == msgLength)
  {
    hasValidSerialMsg = true;
    
    for (uint8_t i = 0; i < msgLength; i++)
      receivedData[i] = tmpBuff[i];
    
    //calc packets per second
    uint8_t pktsPerSecond = 0;
    static unsigned long lastPktsPerSecond = 0;
    static unsigned long pktsPrevCalcMillis = 0;
    unsigned long ttElapsed = millis() - pktsPrevCalcMillis;
    if (ttElapsed >= 1000)
    {
      pktsPrevCalcMillis = millis();
      unsigned long _radioPktsPS = (radioTotalPackets - lastPktsPerSecond) * 1000;
      _radioPktsPS /= ttElapsed;
      pktsPerSecond = _radioPktsPS & 0xff;
      lastPktsPerSecond = radioTotalPackets;
    }
    
    // read the 3 position switch. upperPos is 2, midPos is 0, lowerPos is 1
    uint8_t SwCState = ((!digitalRead(SwCUpperPosPin) << 1) | !digitalRead(SwCLowerPosPin)) & 0x03;

    //send back packets per sec and 3pos switch state
    uint8_t returnByte = (pktsPerSecond & 0x3F) | ((SwCState << 6) & 0xC0);
    Serial.write(returnByte); 
  }
}

//==================================================================================================

void playTones()
{
  audioToPlay = receivedData[18];
  if(audioToPlay != lastAudioToPlay) //init playback with the specified audio
  {
    lastAudioToPlay = audioToPlay;
    switch(audioToPlay)
    {
      case AUDIO_THROTTLEWARN:   
        rtttl::begin(buzzerPin, warnSound); 
        break;
      case AUDIO_BATTERYWARN:    
        rtttl::begin(buzzerPin, battLowSound); 
        break;
      case AUDIO_TIMERELAPSED:   
        rtttl::begin(buzzerPin, timerElapsedSound); 
        break;
      case AUDIO_KEYTONE:        
      case AUDIO_SWITCHMOVED:    
        rtttl::begin(buzzerPin, shortBeepSound); 
        break;
      case AUDIO_TRIMSELECTED:
        rtttl::begin(buzzerPin, trimSelectSound); 
        break;
    }
  }
  else //Playback. Automatically stops once all notes have been played
    rtttl::play();
}

//==================================================================================================

void bind()
{
  if(radioInitialised == false)
    return;
  
  //-------- clear fhss_schema ---------------------------
  memset(fhss_schema, 0xff, sizeof(fhss_schema));
  
  //-------- generate unique random fhss_schema ----------
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

  //-------- save fhss_schema to eeprom ----------
  EEPROM.put(EE_ADR_FHSS_SCHEMA, fhss_schema);
  
  //-------- transmit on bind frequency ----------
  //Air protocol Format  byte0 - transmitterID, byte1tobyteN<12 - hop channels, byte12 - crc8
  
  //set to bind freq
  LoRa.sleep();
  LoRa.setFrequency(bindFreq);
  LoRa.idle();
  
  //prepare data
  uint8_t dataToTransmit[13]; 
  memset(dataToTransmit, 0, sizeof(dataToTransmit));
  dataToTransmit[0] = transmitterID;
  for(uint8_t i = 0; i < sizeof(fhss_schema)/sizeof(fhss_schema[0]); i++)
    dataToTransmit[1 + i] = fhss_schema[i];
  dataToTransmit[12] = crc8Maxim(dataToTransmit, 12);
  
  //start transmission
  uint32_t stopTime = millis() + 2000;
  while(millis() < stopTime)
  {
    if (LoRa.beginPacket())
    {
      LoRa.write(dataToTransmit, 13);
      LoRa.endPacket(); //blocking until done transmitting
    }
    delay(5);
    //do other stuff as we wait
    getSerialData();
    playTones();
  }
  
  //--------- set to operating freq ---------------
  hop();
}

//==================================================================================================

void hop()
{
  if(radioInitialised == false)
    return;
  
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

void transmitServoData()
{
  uint8_t rfStatus = (receivedData[1] >> 4) & 0x01;
  
  if(radioInitialised == false || hasValidSerialMsg == false || rfStatus == 0)
    return;
  
  /* Air protocol format
    b0 
    Transmitter ID
    b1        b2        b3        b4        b5       
    11111111  11222222  22223333  33333344  44444444 
    b6        b7        b8        b9       b10
    55555555  55666666  66667777  77777788  88888888
    b11       b12
    abpf0000  CCCCCCCC
    
    Servo Chs 1 to 8 encoded as 10 bits      
    a is digital chA bit
    b is digital ChB bit 
    p is PWM mode bit for ch3
    f is failsafe flag
    C is the CRC
  */
  
  //------------- encode the data -------------
  
  uint16_t ch1to8[8];
  for (int i = 0; i < 8; i += 1)
    ch1to8[i] = combineBytes(receivedData[2 + i*2], receivedData[3 + i*2]);
  
  uint8_t dataToTransmit[13]; 
  
  dataToTransmit[0] = transmitterID;
  
  dataToTransmit[1] = (ch1to8[0] >> 2) & 0xFF;
  dataToTransmit[2] = (ch1to8[0] << 6 | ch1to8[1] >> 4) & 0xFF;
  dataToTransmit[3] = (ch1to8[1] << 4 | ch1to8[2] >> 6) & 0xFF;
  dataToTransmit[4] = (ch1to8[2] << 2 | ch1to8[3] >> 8) & 0xFF;
  dataToTransmit[5] = ch1to8[3] & 0xFF;
  dataToTransmit[6] = (ch1to8[4] >> 2) & 0xFF;
  dataToTransmit[7] = (ch1to8[4] << 6 | ch1to8[5] >> 4) & 0xFF;
  dataToTransmit[8] = (ch1to8[5] << 4 | ch1to8[6] >> 6) & 0xFF;
  dataToTransmit[9] = (ch1to8[6] << 2 | ch1to8[7] >> 8) & 0xFF;
  dataToTransmit[10] = ch1to8[7] & 0xFF;
  
  //dig ChA, dig ChB, pwm mode for ch3, failsafe flag
  dataToTransmit[11] =  0x00;
  dataToTransmit[11] |= (receivedData[1] & 0x0F) << 4;
  
  //crc
  dataToTransmit[12] =  crc8Maxim(dataToTransmit, 12);
  
  //------------ transmit -----------------
  
  if (LoRa.beginPacket()) //Returns 1 if radio is ready to transmit, 0 if busy or on failure.
  {
    LoRa.write(dataToTransmit, 13);
    LoRa.endPacket(); //pass true for async, else block until done transmitting
    radioTotalPackets++;
    // hop to next frequency
    hop();
  }
}

//--------------------------------------------------------------------------------------------------

uint16_t combineBytes(uint8_t _byte1, uint8_t _byte2)
{
  uint16_t qq;
  qq = (uint16_t) _byte2;
  qq <<= 7;
  qq |= (uint16_t) _byte1;
  return qq;
}
