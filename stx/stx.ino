/***************************************************************************************************
* Code for slave transmiter microcontroller 
* BUK 2020
* buk7456@gmail.com

* Tested to compile on Arduino IDE 1.8.9 or later
* Sketch should compile without warnings even with -WALL
***************************************************************************************************/

#include <SPI.h>

#include "LoRa.h"
  // NOTE: This library exposes the LoRa radio directly, and allows you to send data to 
  //       any radios in range with same radio parameters. 
  //       All data is broadcasted and there is no addressing. 
  //       Any LoRa radio that are configured with the same radio parameters and in range 
  //       can see the packets you send.

#include "crc16.h"

bool radioInitialised = false;

uint8_t dataToTransmit[12]; //Holds data to send to rc receiver

const int msgLength = 32;  //bytes in master's message including start and stop
uint8_t receivedData[msgLength];  //Holds the valid data from master mcu

unsigned long lastValidMsgRcvTime;
unsigned long validMsgCount = 0;

#define RF_ENABLED 206   //As received from master mcu
#define RF_DISABLED 207  //As received from master mcu


//--------Tone stuff----------
#include "NonBlockingRtttl.h"

#define AUDIO_NONE          0xD2  
#define AUDIO_TIMERELAPSED  0xD3  
#define AUDIO_THROTTLEWARN  0xD4  
#define AUDIO_BATTERYWARN   0xD5  
#define AUDIO_KEYTONE       0xD6
#define AUDIO_SWITCHMOVED   0xD7


uint8_t audioToPlay = AUDIO_NONE;
uint8_t lastAudioToPlay = AUDIO_NONE;

//Sounds in rtttl format
const char* battLowSound = "battlow2:d=4,o=5,b=290:4c6,32p,4a#,32p,4g.";
const char* warnSound = "warn:d=4,o=4,b=160:4b5";
const char* shortBeepSound = "shortBeep:d=4,o=4,b=250:16c#7";
const char* timerElapsedSound = "timerElapsed:d=4,o=5,b=210:16b6,16p,8b6";

//-----------------------------------------
uint8_t radioPacketsPerSecond;
unsigned long packetsPerSecPrevCalcTime = 0;
unsigned long radioTotalPackets = 0;
unsigned long prevradioTotalPackets = 0;


//-------------------Pins---------------------
const int buzzerPin = 9;
const int lcdBacklightPin = 6;
const int SwCUpperPosPin = A4; //3pos switch
const int SwCLowerPosPin = A5; //3pos switch

//-----------------3pos switch states-----------
enum {SWUPPERPOS = 2, SWMIDPOS = 0, SWLOWERPOS = 1};
uint8_t SwCState = SWUPPERPOS; 


//==================================================================================================
void setup()
{
  memset(receivedData, 0, sizeof(receivedData)); //Clear this array to prevent unexpected results

  pinMode(lcdBacklightPin,  OUTPUT);
  pinMode(buzzerPin,  OUTPUT);
  
  pinMode(SwCUpperPosPin, INPUT_PULLUP);
  pinMode(SwCLowerPosPin, INPUT_PULLUP);
  

  Serial.begin(115200);
  delay(500);



  /// Override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(10, 8, 2); //pin 2 actually not used unless we have a callback 

  if (LoRa.begin(433E6))
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

  //other initialisations
  lastValidMsgRcvTime = millis();
}

//========================================= MAIN ==================================================
void loop()
{
  /// ---------- READ THE 3 POSITION SWITCH ---------------
  SwCState = ((!digitalRead(SwCUpperPosPin) << 1) | !digitalRead(SwCLowerPosPin)) & 0x03;
  //state is sent after reading serial data
 
  
  /// ---------- READ THE SERIAL DATA ---------------------
  getSerialData();

  ///----------- CONTROL LCD BACKLIGHT --------------------
  if (receivedData[27] == 0xCA)
    digitalWrite(lcdBacklightPin, HIGH);
  else if (receivedData[27] == 0xCB)
    digitalWrite(lcdBacklightPin, LOW);


  /// ---------- TRANSMIT VIA RF MODULE --------------------
  uint8_t rfStatus = receivedData[29];
  static unsigned long lastValidMsgCount = 0;
  if (radioInitialised == true && rfStatus == RF_ENABLED && validMsgCount > lastValidMsgCount)
  {
    encodeDataToTransmit();

    if (LoRa.beginPacket()) //Returns 1 if radio is ready to transmit, 0 if busy or on failure.
    {
      LoRa.write(dataToTransmit, 12);
      if (LoRa.endPacket()) //Returns 1 on success, 0 on failure.
      {
        lastValidMsgCount = validMsgCount;
        radioTotalPackets++;
      }
    }
  }
  
  //-------------- Calculate packets per second --------------------
  unsigned long ttElapsed = millis() - packetsPerSecPrevCalcTime;
  if (ttElapsed >= 1000)
  {
    packetsPerSecPrevCalcTime = millis();

    unsigned long _radioPktsPS = (radioTotalPackets - prevradioTotalPackets) * 1000;
    _radioPktsPS /= ttElapsed;
    radioPacketsPerSecond = uint8_t(_radioPktsPS);
    prevradioTotalPackets = radioTotalPackets;
    //packets per sec send to master on receiving data from master
  }
  
  ///-------------------- Play any audio alerts ------------------------
  audioToPlay = receivedData[30];

  if(audioToPlay != lastAudioToPlay) //init playback with the specified audio
  {
    lastAudioToPlay = audioToPlay;
    switch(audioToPlay)
    {
      case AUDIO_THROTTLEWARN:   rtttl::begin(buzzerPin, warnSound); break;
      case AUDIO_BATTERYWARN:    rtttl::begin(buzzerPin, battLowSound); break;
      case AUDIO_TIMERELAPSED:   rtttl::begin(buzzerPin, timerElapsedSound); break;
      case AUDIO_KEYTONE:        rtttl::begin(buzzerPin, shortBeepSound); break;
      case AUDIO_SWITCHMOVED:    rtttl::begin(buzzerPin, shortBeepSound); break;
    }
  }
  else //Playback. Playback will automatically stop once all notes have been played
  {
    rtttl::play();
  }
}

//==================================================================================================
void getSerialData()
{
  //Reads the incoming serial data into receivedData[]
  /* Master to Slave MCU communication format as below

  - Start byte 0xBB (0d187)
  
  - Channel 1 to 8 data (2bytes per channel, total 16 bytes). 0b0LLLLLLL 0b0HHHHHHH
  
  - Channel 9 to 10 data (1 byte per channel, total 2 bytes)
  
  - Channe1 1 to 8 failsafes (1byte each, 8 bytes total)

  - Backlight status (1 byte) 0d202 on, 0d203 off
  - Battery status (1 byte) 0d204 healthy, 0d205 low
  - RF status (1 byte) 0d206 on, 0d207 off
  - Audio tone to play (1 byte)
  
  - Stop byte 0xDD (0d221)
  */

  //Read in new message
  /*Here, we need to first check how many bytes are available in serial buffer. If nothing, we exit.
    If the number of available bytes is less than we are expecting (but not zero), then we have to
    wait a little probably because the incoming byte stream is not yet fully arrived.
    We then read everything from the Serial buffer  */

  int numBytesSer = Serial.available();
  if (numBytesSer == 0)
  {
    return;
  }

  if (numBytesSer > 0 && numBytesSer < msgLength)
    delay(3); /* 115200 baud is about 11520 bytes/s. 32 bytes takes about 3ms */

  uint8_t tmpBuff[64];
  memset(tmpBuff, 0, sizeof(tmpBuff)); //fill tmpBuff with zeros to prevent unpredictable results

  bool tmpBuffIsFull = false;
  unsigned int cntr = 0;

  while (Serial.available() > 0)
  {
    if (tmpBuffIsFull == false) //put data into tmpBuff
    {
      tmpBuff[cntr] = Serial.read();
      cntr += 1;
      if (cntr >= (sizeof(tmpBuff) / sizeof(tmpBuff[0]))) //prevent array out of bounds
        tmpBuffIsFull = true;
    }
    else //Throw away any extra data
      Serial.read();
  }

  //-------- Check for fisrt occurence of start and stop bytes in tmpBuff ---------
  int startByteIndex = 0, stopByteIndex = 0;
  for (unsigned int i = 0; i < (sizeof(tmpBuff) / sizeof(tmpBuff[0])); i += 1)
  {
    if (tmpBuff[i] == 0xBB)
      startByteIndex = i;
    else if (tmpBuff[i] == 0xDD)
    {
      stopByteIndex = i;
      break;
    }
  }

  //--------Copy to receivedData[] if the data received is good. Also send back pkts/sec----
  if ((stopByteIndex - startByteIndex + 1) == msgLength)
  {
    validMsgCount++;
    lastValidMsgRcvTime = millis();
    for (int i = 0; i < msgLength; i += 1)
      receivedData[i] = tmpBuff[i];
    
    //send back packets per sec and 3pos switch state
    uint8_t returnByte = (radioPacketsPerSecond & 0x3F) | ((SwCState << 6) & 0xC0);
    Serial.write(returnByte); 
  }
}

//==================================================================================================
uint16_t combineBytes(uint8_t _byte1, uint8_t _byte2)
{
  uint16_t qq;
  qq = (uint16_t) _byte2;
  qq <<= 7;
  qq |= (uint16_t) _byte1;
  return qq;
}


//==================================================================================================
void encodeDataToTransmit()
{
  // We encode the data into a more compact form

  uint16_t ch1to8[8];
  for (int i = 0; i < 8; i += 1)
    ch1to8[i] = combineBytes(receivedData[1 + i*2], receivedData[2 + i*2]);

  /** Air protocol format
     Chs 1 to 8 encoded as 10 bits

     byte  b0       b1      b2        b3       b4       
        11111111 11222222 22223333 33333344 44444444 
        
           b5       b6      b7        b8       b9
        55555555 55666666 66667777 77777788 88888888

           b10      b11
        abfCCCCC   CCCCCCCC
        
     a is digital ch9, b digital Ch10,  f is failsafe flag
     
     CCCCC CCCCCCCC is the 13 bit CRC (calculated as CRC16)   
  */

  //Send Fail safe every 2 secs
  dataToTransmit[10] = 0x00;
  static unsigned long fsLastms = 0;
  if (millis() - fsLastms >= 2000)
  {
    fsLastms = millis();
    dataToTransmit[10] = 0x20; //failsafe bit
    
    //Replace channel values by failsafes
    //if failsafe is off, set to 1023 (1111111111)
    for(int i=0; i<8; i++)
    {
      if(receivedData[19+i] == 0)
        ch1to8[i] = 1023;
      else
      {
        ch1to8[i] = receivedData[19 + i] - 1;
        ch1to8[i] *= 5;
      }
    }
  }

  //Encode
  
  dataToTransmit[0]  = (ch1to8[0] >> 2) & 0xFF;
  dataToTransmit[1]  = (ch1to8[0] << 6 | ch1to8[1] >> 4) & 0xFF;
  dataToTransmit[2]  = (ch1to8[1] << 4 | ch1to8[2] >> 6) & 0xFF;
  dataToTransmit[3]  = (ch1to8[2] << 2 | ch1to8[3] >> 8) & 0xFF;
  dataToTransmit[4]  = ch1to8[3] & 0xFF;
  
  dataToTransmit[5]  = (ch1to8[4] >> 2) & 0xFF;
  dataToTransmit[6]  = (ch1to8[4] << 6 | ch1to8[5] >> 4) & 0xFF;
  dataToTransmit[7]  = (ch1to8[5] << 4 | ch1to8[6] >> 6) & 0xFF;
  dataToTransmit[8]  = (ch1to8[6] << 2 | ch1to8[7] >> 8) & 0xFF;
  dataToTransmit[9]  = ch1to8[7] & 0xFF;
  
  dataToTransmit[10] |= (receivedData[17] & 0x01) << 7; //digitalChA bit
  dataToTransmit[10] |= (receivedData[18] & 0x01) << 6; //digitalChB bit
  
  uint16_t crcQQ = crc16(dataToTransmit, 11);
 
  dataToTransmit[10] |= ((crcQQ >> 8) & 0x1F);
  dataToTransmit[11] = crcQQ & 0xFF;
}
