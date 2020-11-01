
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
void serialSendData(); 
void checkBattery();

void serialWrite14bit(uint16_t value); //helper

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

  //init lcd
  display.begin();
  display.setTextWrap(false);
  
  //init serial port
  Serial.begin(UART_BAUD_RATE);
  delay(200);
  
  //initialise with safe values. Will be overridden by reading from eeprom
  setDefaultModelName();
  setDefaultModelBasicParams();
  setDefaultModelMixerParams();
  
  //Boot UI
  readSwitchesAndButtons();
  if(buttonCode == SELECT_KEY)
  {
    HandleBootUI(); //blocking
  }
  
  //compute eeprom start address for model data and number of possible models we can have
  eeModelDataStartAddress = eeSysDataStartAddress + sizeof(Sys);
  numOfModels = ((E2END + 1) - eeModelDataStartAddress) / sizeof(Model); //E2END is last address

  /********* EEPROM init ***************/
  uint8_t EE_INITFLAG = crc8Maxim((uint8_t *) &Sys, sizeof(Sys)) ^ crc8Maxim((uint8_t *) &Model, sizeof(Model));
  if (EEPROM.read(0) != EE_INITFLAG)
  {
    while(buttonCode == 0)
    {
      DisplayFullScreenMsg(F("Bad EEPROM data"));
      readSwitchesAndButtons();
      delay(30);
    }
    DisplayFullScreenMsg(F("Formating.."));
    
    //save
    eeSaveSysConfig();
    for (uint8_t mdlNo = 1; mdlNo <= numOfModels; mdlNo++)
      eeSaveModelData(mdlNo);
 
    EEPROM.write(0, EE_INITFLAG);
    delay(1000);
    changeToScreen(MODE_CALIB); //Set to sticks calib screen
    buttonCode = 0; 
    
    skipThrottleCheck = true;
  }

  ///--------- Load data from eeprom -----------
  DisplayFullScreenMsg(F("Loading.."));
  delay(500);
  eeReadSysConfig();
  eeReadModelData(Sys.activeModel);

  ///--------- Init battery reading --------------
  battVoltsNow = battVoltsMax; 
  
  ///--------- Overrides on startup ----------
  readSwitchesAndButtons();
  if(buttonCode == UP_KEY) //mute
  {
    Sys.soundMode = SOUND_OFF;
    eeSaveSysConfig();
  }
  buttonCode = 0;
  
  ///---------- Warn throttle if throttle position is more than 5% above minimum ---
  if(!skipThrottleCheck)
  {
    readSwitchesAndButtons();
    readSticks();
    bool _rfState = Sys.rfOutputEnabled;
    while (throttleIn > -450)
    {
      DisplayFullScreenMsg(F("Check throttle"));
      readSwitchesAndButtons();
      readSticks();
      //play warning sound
      audioToPlay = AUDIO_THROTTLEWARN;
      Sys.rfOutputEnabled = false; //overide
      serialSendData();
      delay(30);
    }
    Sys.rfOutputEnabled = _rfState; //restore
  }

  //------------------- Init throttle timer -------------------------
  throttleTimerLastPaused = millis(); 
  
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
  serialSendData();
  
  while (Serial.available() > 0)
  {
    returnedByte = Serial.read();
  }

  //limit max rate of loop
  unsigned long loopTime = millis() - loopStartTime;
  if(loopTime < fixedLoopTime) 
    delay(fixedLoopTime - loopTime);
  // display.setCursor(0, 0); //benchmarking
  // display.print(loopTime); //benchmarking
}

//==================================================================================================

void serialSendData()
{
  /* if(Sys.rfOutputEnabled == false)
  {
    return;
  } */
  
  /** Master to Slave MCU communication format as below
  
  byte 0 - Start of message 0xBB
  byte 1 - Status byte 
      bit7 - 0
      bit6 - backlight 1 on, 0 off
      bit5 - bind state 1 bind 0 operate
      bit4 - RF module, 1 on, 0 off 
      bit3 - DigChA, 1 on, 0 off
      bit2 - DigChB, 1 on, 0 off
      bit1 - PWM mode for Channel3, 1 means servo pwm, 0 ordinary pwm
      bit0 - Failsafe, 1 means failsafe data, 0 normal data
      
  byte 2 to 17 - Ch1 thru 8 data (2 bytes per channel, total 16 bytes)
  byte 18 - Sound to play  (1 byte)
  byte 19 - End of message 0xDD
  */
  
  /// ---- message start
  Serial.write(0xBB); 
  
  /// ---- status byte
  uint8_t status = 0x00;
  
  static unsigned long lastBtnDownTime = 0;
  if(buttonCode > 0) 
    lastBtnDownTime = millis();
  unsigned long elapsed = millis() - lastBtnDownTime;
  if(Sys.backlightMode == BACKLIGHT_ON 
     || (Sys.backlightMode == BACKLIGHT_5S  && elapsed < 5000UL )
     || (Sys.backlightMode == BACKLIGHT_15S && elapsed < 15000UL)
     || (Sys.backlightMode == BACKLIGHT_60S && elapsed < 60000UL)) 
    status |= 0x40;
  
  status |= (bindActivated & 0x01) << 5;
  bindActivated = false;
  
  status |= (Sys.rfOutputEnabled & 0x01) << 4;
  status |= DigChA << 3;
  status |= DigChB << 2;
  status |= Sys.PWM_Mode_Ch3 << 1;
  bool isFailsafeData = false; 
  if(thisLoopNum % (2000 / fixedLoopTime) == 1) 
    isFailsafeData = true;
  status |= isFailsafeData & 0x01;
  Serial.write(status); 
  
  /// ---- channel data and failsafes
  if(isFailsafeData == false) 
  {
    for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
    {
      uint16_t val = (ChOut[i] + 500) & 0xFFFF; 
      serialWrite14bit(val);
    }
  }
  else //send failsafe
  {
    for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
    {
      if(Model.Failsafe[i] == -101) //failsafe not specified, send 1023
        serialWrite14bit(1023);
      else //failsafe specified, send it
      {
        int fsf = 5 * Model.Failsafe[i];
        fsf = constrain(fsf, 5 * Model.EndpointL[i], 5 * Model.EndpointR[i]);
        uint16_t val = (fsf + 500) & 0xFFFF;
        serialWrite14bit(val);
      }
    }
  }
  
  /// ---- sounds
  if((Sys.soundMode == SOUND_OFF)
      || (Sys.soundMode == SOUND_ALARMS && audioToPlay >= AUDIO_SWITCHMOVED)
      || (Sys.soundMode == SOUND_NOKEY && audioToPlay == AUDIO_KEYTONE))
    audioToPlay = AUDIO_NONE; 

  Serial.write(audioToPlay); 
  audioToPlay = AUDIO_NONE; //set to none
  
  /// ---- end of message
  Serial.write(0xDD); 
}

void serialWrite14bit(uint16_t value)
{
  // The data is sent as a "14 bit" value (Two bytes) as 0b0LLLLLLL 0b0HHHHHHH
  Serial.write(value & 0x7f);
  Serial.write((value >> 7) & 0x7f);
}

//--------------------------------------------------------------------------------------------------

void checkBattery()
{
  /* Apply smoothing to measurement. Cant afford to store any data points due to limited RAM,
  so we cant use the usual moving average method. Instead we implement exponential recursive
  smoothing as described here
  https://www.megunolink.com/articles/coding/3-methods-filter-noisy-arduino-measurements/
  It works by subtracting out the mean each time, and adding in a new point. */
  
  enum { _NUM_SAMPLES = 30 };
  /*_NUM_SAMPLES parameter defines number of samples to average over. Higher value results in slower
  response.
  Formula x = x - x/n + a/n  */
  
  long anaRd = ((long)analogRead(PIN_BATTVOLTS) * battVfactor) / 100;
  long battV = ((long)battVoltsNow * (_NUM_SAMPLES - 1) + anaRd) / _NUM_SAMPLES; 
  battVoltsNow = int(battV); 
  
  if (battVoltsNow <= battVoltsMin)
    battState = BATTLOW;
  else if (battVoltsNow > (battVoltsMin + 100)) //100mV hysteris
    battState = BATTHEALTY;
}
