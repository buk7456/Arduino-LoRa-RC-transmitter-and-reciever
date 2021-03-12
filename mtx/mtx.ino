
/***************************************************************************************************
  Transmitter master mcu code for Atmega328p

  buk7456 at gmail dot com   
  2020
  
  Released under the MIT Licence
  Tested to compile on Arduino IDE 1.8.9 or later
  Sketch should compile without warnings even with -WALL. Only warning is the Low memory.
  
  FREE RAM AFTER COMPILE SHOULD BE ATLEAST 300 BYTES 
  
***************************************************************************************************/

#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SPI.h>

#include "GFX.h"

#include "config.h"
#include "common.h"
#include "eestore.h"
#include "io.h"
#include "bitmaps.h"
#include "ui_128x64.h" 

#include "crc8.h"

// Declarations 
void sendSerialData(); 
void getSerialData();
void checkBattery();
uint16_t joinBytes(uint8_t _highByte, uint8_t _lowByte); //helper

//===================================== setup ======================================================

void setup()
{
  //set up pins
  pinMode(PIN_ROW1, INPUT);
  pinMode(PIN_ROW2, INPUT);
  pinMode(PIN_COL1, OUTPUT);
  pinMode(PIN_COL2, OUTPUT);
  pinMode(PIN_COL3, OUTPUT);
  
  //init spi 
  SPI.begin(); //Still needed even when using SPI transactions

  //init serial port
  Serial.begin(UART_BAUD_RATE);
  delay(200);
  
  //init lcd
  display.begin();
  display.setTextWrap(false);
  
  //initialise model data with safe values. Will be overridden by reading from eeprom
  setDefaultModelName();
  setDefaultModelBasicParams();
  setDefaultModelMixerParams();
  
  //Startup menu
  readSwitchesAndButtons();
  if(buttonCode == SELECT_KEY)
  {
    HandleStartupMenu(); //blocking
  }
  
  //compute eeprom start address for model data and number of possible models
  eeModelDataStartAddress = eeSysDataStartAddress + sizeof(Sys);
  numOfModels = (EEPROM.length() - eeModelDataStartAddress) / sizeof(Model);

  ///------------- EEPROM Check ------------------
  
  uint8_t eeInitFlag = crc8Maxim((uint8_t *) &Sys, sizeof(Sys)) ^ crc8Maxim((uint8_t *) &Model, sizeof(Model));
  
  bool formatEE = false;
 
  /// Check signature. If its not matching, then assume it's a fresh mcu and format the eeprom
  uint16_t fileSignature;
  EEPROM.get(EE_FILE_SIGNATURE_ADDR, fileSignature);
  if(fileSignature != 0xE7D9) //force format
  {
    display.clearDisplay();
    FullScreenMsg(PSTR("Bad EEPROM data\n\nPress any key"));
    display.display();
    
    while(buttonCode == 0)
    {
      readSwitchesAndButtons();
      delay(30);
    }
    
    buttonCode = 0; 
    formatEE = true;
  }
  ///Check flag. Signature may match but not the data structs
  else if(EEPROM.read(EE_INITFLAG_ADDR) != eeInitFlag)
  {
    display.clearDisplay();
    FullScreenMsg(PSTR("Format EEPROM?\n\nYes [Up]  \nNo  [Down]"));
    display.display();
    
    while(buttonCode != UP_KEY && buttonCode != DOWN_KEY)
    {
      readSwitchesAndButtons();
      delay(30);
    }
    
    if(buttonCode == UP_KEY) //format
      formatEE = true;
    else if(buttonCode == DOWN_KEY)
      EEPROM.write(EE_INITFLAG_ADDR, eeInitFlag);
    
    buttonCode = 0;
  }
  
  if(formatEE)
  {
    display.clearDisplay();
    FullScreenMsg(PSTR("Formatting.."));
    display.display();
    delay(500);
    
    //erase eeprom
    eraseEEPROM();
    
    //write system data
    eeSaveSysConfig();
    //create a model in slot 1
    eeCreateModel(1);
    
    //write flag
    EEPROM.write(EE_INITFLAG_ADDR, eeInitFlag);
    //write signature
    fileSignature = 0xE7D9;
    EEPROM.put(EE_FILE_SIGNATURE_ADDR, fileSignature);
    
    changeToScreen(MODE_CALIB); 
    skipThrottleCheck = true;
  }
  
  ///-------------------------------------------
  display.clearDisplay();
  FullScreenMsg(PSTR("Welcome"));
  display.display();
  delay(1000);

  ///--------- Load data from eeprom -----------
  eeReadSysConfig();
  eeReadModelData(Sys.activeModel);

  ///--------- Init battery reading ------------
  battVoltsNow = battVoltsMax; 

  ///--------- Warn if throttle is not low -----
  if(!skipThrottleCheck)
  {
    readSticks();
    bool _rfState = Sys.rfOutputEnabled;
    while (throttleIn > -450)
    {
      display.clearDisplay();
      FullScreenMsg(PSTR("Check throttle"));
      display.display();
      readSticks();
      //play warning sound
      audioToPlay = AUDIO_THROTTLEWARN;
      Sys.rfOutputEnabled = false; //overide
      sendSerialData();
      getSerialData();
      delay(30);
    }
    Sys.rfOutputEnabled = _rfState; //restore
  }

  ///--------- Init timers ---------------------
  timer1LastPaused = millis(); 
  
  ///--------- other initialisations -----------
  //initialise battery volts
  for(uint8_t i = 0; i < 30; i++)
  {
    checkBattery();
    delay(5);
  }

}

//===================================== main =======================================================

void loop()
{
  unsigned long loopStartTime = millis();
  thisLoopNum++;
  
  checkBattery();
  readSwitchesAndButtons();
  determineButtonEvent();
  readSticks();
  computeChannelOutputs();
  HandleMainUI();
  sendSerialData();
  getSerialData();
  
  //limit max rate of loop
  unsigned long loopTime = millis() - loopStartTime;
  if(loopTime < fixedLoopTime) 
    delay(fixedLoopTime - loopTime);
  // display.setCursor(0, 0); //benchmarking
  // display.print(loopTime); //benchmarking
}

//==================================================================================================

void sendSerialData()
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
  
  uint8_t _data[20];
  
  /// ---- status bytes
  
  uint8_t status1 = 0x00;
  uint8_t status2 = 0x00;
  
  static unsigned long lastBtnDownTime = 0;
  if(buttonCode > 0) 
    lastBtnDownTime = millis();
  unsigned long elapsed = millis() - lastBtnDownTime;
  if(Sys.backlightMode == BACKLIGHT_ON 
     || (Sys.backlightMode == BACKLIGHT_5S  && elapsed < 5000UL )
     || (Sys.backlightMode == BACKLIGHT_15S && elapsed < 15000UL)
     || (Sys.backlightMode == BACKLIGHT_60S && elapsed < 60000UL)) 
    status1 |= 0x40;
  
  status1 |= (bindActivated & 0x01) << 5;
  bindActivated = false;
  
  status1 |= (Sys.rfOutputEnabled & 0x01) << 4;
  status1 |= DigChA << 3;
  status1 |= DigChB << 2;

  //alternately send failsafe or request telemetry
  bool sendFailsafe = false;
  static bool telemRequest = false;
  if(thisLoopNum % (200 / fixedLoopTime) == 1) //every 200ms
  {
    telemRequest = !telemRequest;
    if(telemRequest)
      status2 |= (1 << 3);
    else
      sendFailsafe = true;
  }
  
  status1 |= (sendFailsafe & 0x01) << 1;
  status2 |= Sys.rfPower;

  _data[0] = status1;
  _data[1] = status2;

  /// ---- sounds
  if((Sys.soundMode == SOUND_OFF)
      || (Sys.soundMode == SOUND_ALARMS && audioToPlay >= AUDIO_SWITCHMOVED)
      || (Sys.soundMode == SOUND_NOKEY && audioToPlay == AUDIO_KEYTONE))
    audioToPlay = AUDIO_NONE; 
    
  _data[2] = audioToPlay; 
  audioToPlay = AUDIO_NONE; 
  
  /// ---- failsafe and channel data
  if(sendFailsafe) 
  {
    for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
    {
      if(Model.Failsafe[i] == -101) //failsafe not specified, send 1023
      {
        _data[3 + i*2] = 0x03;
        _data[4 + i*2] = 0xFF;
      }
      else //failsafe specified
      {
        int fsf = 5 * Model.Failsafe[i];
        fsf = constrain(fsf, 5 * Model.EndpointL[i], 5 * Model.EndpointR[i]);
        uint16_t val = (fsf + 500) & 0xFFFF;
        _data[3 + i*2] = (val >> 8) & 0xFF;
        _data[4 + i*2] = val & 0xFF;
      }
    }
  }
  else
  {
    for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
    {
      uint16_t val = (ChOut[i] + 500) & 0xFFFF; 
      _data[3 + i*2] = (val >> 8) & 0xFF;
      _data[4 + i*2] = val & 0xFF;
    }
  }
  
  _data[19] = crc8Maxim(_data, 19);
  
  Serial.write(_data, 20);
}

//==================================================================================================

void getSerialData()
{
  /** Slave to Master mcu serial communication
  Byte0   Bits 3 to 2 --> Bind status, Bits 1 to 0 --> 3pos switch state
  Byte1   Transmitter packet rate
  Byte2   Packet rate at receiver side
  Byte3-4 Voltage telemetry
  Byte5   CRC8
  */
  
  const uint8_t msgLength = 6;
  if (Serial.available() < msgLength)
  {
    return;
  }
  
  uint8_t _data[msgLength]; 
  memset(_data, 0, msgLength);

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
  
  //Check if valid and extract
  if(_data[msgLength - 1] == crc8Maxim(_data, msgLength - 1))
  {
    bindStatus = (_data[0] >> 2) & 0x03;
    SwCState = _data[0] & 0x03;
    transmitterPacketRate = _data[1];
    receiverPacketRate = _data[2];
    
    //-- telemetry voltage --
    telem_volts = joinBytes(_data[3], _data[4]);
    //apply offset
    if(telem_volts != 0x0FFF)
    {
      long _volts = telem_volts;
      _volts += (long)Sys.telemVoltsOffset;
      if(_volts < 0)
        _volts = 0;
      telem_volts = _volts & 0xFFFF;
    }
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

void checkBattery()
{
  /* Apply smoothing to measurement. Cant afford to store any data points due to limited RAM,
  so we cant use the usual moving average method. Instead we implement exponential recursive
  smoothing as described here
  https://www.megunolink.com/articles/coding/3-methods-filter-noisy-arduino-measurements/
  It works by subtracting out the mean each time, and adding in a new point. */
  
  /*_NUM_SAMPLES parameter defines number of samples to average over. Higher value results in slower
  response.
  Formula x = x - x/n + a/n  */
  
  const int _NUM_SAMPLES = 20;
  
  long anaRd = ((long)analogRead(PIN_BATTVOLTS) * battVfactor) / 100;
  long battV = ((long)battVoltsNow * (_NUM_SAMPLES - 1) + anaRd) / _NUM_SAMPLES; 
  battVoltsNow = int(battV); 
  
  if (battVoltsNow <= battVoltsMin)
    battState = BATTLOW;
  else if (battVoltsNow > (battVoltsMin + 100)) //100mV hysteris
    battState = BATTHEALTY;
}
