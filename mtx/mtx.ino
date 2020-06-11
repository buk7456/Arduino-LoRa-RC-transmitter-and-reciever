
/**************************************************************************************************
  Transmitter master mcu code for Atmega328p

  This code reads stick inputs, handles User Interface, outputs via serial to a second mcu chip
  that handles the actual transmission via Rf module to the receiver.
  See the schematics.
  buk7456@gmail.com   2019 - 2020
  
  Released under the MIT Licence
  Tested to compile on Arduino IDE 1.8.9 or later
  Sketch should compile without warnings even with -WALL. Only warning is the Low memory.
  
  FREE RAM AFTER COMPILE SHOULD BE GREATER THAN 340 BYTES !
***************************************************************************************************/

#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SPI.h>

#include "GFX.h"

#include "config.h"
#include "common.h"
#include "io.h"
#include "eestore.h"
#include "bitmaps.h"
#include "ui_128x64.h" 


//### Declarations #####
void sendToSlaveMCU(); //sends radio data to slave mcu
void serialWrite14bit(int _chdat); //helper
void checkBattery();

//===================================== setup ======================================================
void setup()
{
  pinMode(ROW1_MTRX_PIN, INPUT);
  pinMode(ROW2_MTRX_PIN, INPUT);
  pinMode(COL1_MTRX_PIN, OUTPUT);
  pinMode(COL2_MTRX_PIN, OUTPUT);
  pinMode(COL3_MTRX_PIN, OUTPUT);

  Serial.begin(UARTBAUDRATE);
  delay(200);

  display.begin();
  display.setTextWrap(false);
  
  /********* EEPROM init **************
    Check for flag. If flag value is not what we expect, overwrite values in eeprom
    with defaults (initial values of certain global variables), Then write flag.
  */
  uint8_t eeCheckMark = EEPROM.read(_EEPROM_INIT);
  if (eeCheckMark != EE_INITFLAG)
  {
    DrawEEWarning();
    while(buttonCode == 0)
    {
      readSwitchesAndButtons();
      delay(30);
    }
    DrawEEFormatMsg();
    eeUpdateSysConfig();
    for (uint8_t mdlNo = 1; mdlNo <= 6; mdlNo++)
    {
      eeUpdateModelBasicData(mdlNo);
      eeUpdateMixData(mdlNo);
      eeUpdateModelName(mdlNo);
    }
    EEPROM.write(_EEPROM_INIT, EE_INITFLAG);
    delay(1000);
    
    changeToScreen(MODE_CALIB); //## Set to sticks calib screen

    buttonCode = 0;    
  }

  ///--------- Load data from eeprom -----------
  DrawLoadingMsg();
  delay(500);
  eeReadSysConfig();
  eeReadModelBasicData(activeModel);
  eeReadMixData(activeModel); 
  eeReadModelName(activeModel);
  
  ///--------- Override sounds mode if up key held while starting up ----------
  readSwitchesAndButtons();
  if(buttonCode == UP_KEY)
  {
    soundMode = SOUND_OFF;
    eeUpdateSysConfig();
  }
  
  ///----------Show packets per second if down key held while starting up ----
  if(buttonCode == DOWN_KEY)
    showPktsPerSec = true; 
  
  ///----------Warn throttle if throttle position is more than 5% above minimum---
  readSwitchesAndButtons();
  readSticks();
  bool _rfState = rfModuleEnabled; //temporarily store rf state as we are going to override it
  while (throttleIn > -450 && SwAEngaged == false)
  {
    DrawThrottleWarning();
    readSwitchesAndButtons();
    readSticks();
    //play warning sound
    audioToPlay = AUDIO_THROTTLEWARN;
    //override rfModuleEnabled before sending to slave
    rfModuleEnabled = false;
    sendToSlaveMCU();
    delay(30);
  }
  rfModuleEnabled = _rfState; //restore original value before override

  throttleTimerLastPaused = millis(); 
}

//===================================== main =======================================================
void loop()
{
  unsigned long loopStartTime = millis();
  
  thisLoopNum++;
  
  /// -------- CHECk BATTERY --------------------
  checkBattery();

  /// --------- CORE FUNCTIONS ------------------
  readSwitchesAndButtons();
  readSticks();
  computeChannelOutputs();
 
  /// -------- USER INTERFACE -------------------
  HandleMainUI();
  
  /// -------- SEND CHANNEL DATA AND OTHER STATUSES VIA UART TO SLAVE MCU -----
  sendToSlaveMCU();

  ///--------- READ RETURNED BYTE ---------------
  //Slave mcu only sends back this parameter
  while (Serial.available() > 0)
  {
    returnedByte = Serial.read();
  }

  /// ---------- LIMIT MAX RATE OF LOOP ---------
  unsigned long loopTime = millis() - loopStartTime;
  if(loopTime < 28UL)
    delay(28UL - loopTime);
}

//==================================================================================================

void sendToSlaveMCU()
{

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
  
  Serial.write(0xBB); //message header

  serialWrite14bit(ChOut[CH1OUT]);
  serialWrite14bit(ChOut[CH2OUT]);
  serialWrite14bit(ChOut[CH3OUT]);
  serialWrite14bit(ChOut[CH4OUT]);
  serialWrite14bit(ChOut[CH5OUT]);
  serialWrite14bit(ChOut[CH6OUT]);
  serialWrite14bit(ChOut[CH7OUT]);
  serialWrite14bit(ChOut[CH8OUT]);

  Serial.write(DigChA);
  Serial.write(DigChB);
  
  // ----- failsafes -----------------
  for(int i=0; i<8; i++)
  {
    if(Failsafe[i] > 0) //failsafe has been specified, have to constrain to endpoints
    {
      uint16_t fsf = (Failsafe[i] - 1) * 5; 
      uint16_t lowerLim = (100 - EndpointL[i]) * 5;
      uint16_t upperLim = (100 + EndpointR[i]) * 5;
      if(fsf < lowerLim) 
        fsf = lowerLim;
      else if(fsf > upperLim) 
        fsf = upperLim;
      fsf = (fsf/5) + 1;
      uint8_t dat = fsf & 0xFF;
      Serial.write(dat);
    }
    else //not specified. Receiver will hold last servo position if rf signal is lost
      Serial.write(0);
  }
  //-----------
  
  if (backlightEnabled == true) Serial.write(202);
  else Serial.write(203);

  if (battState == _BATTHEALTHY_) Serial.write(204);
  else Serial.write(205);

  if (rfModuleEnabled == true) Serial.write(206);
  else Serial.write(207);
  
  //Sounds
  if((soundMode == SOUND_OFF)
      || (soundMode == SOUND_ALERTS && (audioToPlay == AUDIO_KEYTONE || audioToPlay == AUDIO_SWITCHMOVED))
      || (soundMode == SOUND_ALERTS_SWITCHES && audioToPlay == AUDIO_KEYTONE))
    audioToPlay = AUDIO_NONE; 

  Serial.write(audioToPlay); 
  audioToPlay = AUDIO_NONE; //clear flag

  Serial.write(0xDD); //message footer
}


void serialWrite14bit(int _chdat)
{
  /*The channel data is sent as a "14 bit" value (Two bytes) We need to transpose the data so we
    get rid of negative values. Then write as 0b0LLLLLLL 0b0HHHHHHH */
  _chdat = _chdat + 500; //offset to remove negative values
  Serial.write(_chdat & 0x7f);
  Serial.write((_chdat >> 7) & 0x7f);
}


void checkBattery()
{
  /* Apply smoothing to measurement. Cant afford to store any data points due to limited RAM,
  so we cant use the usual moving average method. Instead we implement exponential recursive
  smoothing as described here
  https://www.megunolink.com/articles/coding/3-methods-filter-noisy-arduino-measurements/
  It works by subtracting out the mean each time, and adding in a new point. */
  
  enum { _NUM_SAMPLES = 30 };
  /*_NUM_SAMPLES parameter defines number of samples to average over. Higher value results in slower
  response. For step input, it would take about these cycles for reading to get there.
  Formula x = x - (x/n) + (a/n)  */
  
  long anaRd = ((long)analogRead(BATTVOLTSPIN) * BATTVFACTOR) / 100;
  long battV = ((long)battVoltsNow * (_NUM_SAMPLES - 1) + anaRd) / _NUM_SAMPLES; 
  battVoltsNow = int(battV); 
  
  //add some hysterisis to battState
  if (battState == _BATTHEALTHY_ && battVoltsNow <= BATTV_MIN)
    battState = _BATTLOW_;
  else if (battState == _BATTLOW_ && battVoltsNow > (BATTV_MIN + 100)) //100mV hysteris
    battState = _BATTHEALTHY_;
}
