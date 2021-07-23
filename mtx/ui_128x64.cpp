
#include "Arduino.h"
#include "GFX.h"
#include <EEPROM.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "common.h"
#include "io.h"
#include "bitmaps.h"
#include "eestore.h"
#include "ui_128x64.h"

#if defined (DISPLAY_KS0108)
  #include "LCDKS0108.h"
  LCDKS0108 display = LCDKS0108(PIN_KS_RS, PIN_KS_EN, PIN_KS_CS1, PIN_KS_CS2, PIN_LATCH);
#elif defined (DISPLAY_CGM12864G)
  #include "LCDCGM12864G_595.h"
  LCDCGM12864G_595 display = LCDCGM12864G_595(PIN_CGM_RSEL, PIN_CGM_RD, PIN_CGM_RST, PIN_LATCH);
#endif

enum {
  WRAP = true, 
  NOWRAP = false
};
enum {
  PRESSED_ONLY = 0, 
  PRESSED_OR_HELD = 1, 
  SLOW_CHANGE = 2, 
  FAST_CHANGE
}; 

//--- helpers ----

void toggleEditModeOnSelectClicked();
void changeToScreen(int8_t _theScrn);
void resetTimer1();
void drawHeader(const char* str);
void printVolts(uint16_t _milliVolts);
void printHHMMSS(uint32_t _milliSecs);
void changeFocusOnUPDOWN(uint8_t _maxItemNo);
void drawCursor(uint8_t _xpos, uint8_t _ypos);
void makeToast(const __FlashStringHelper* text, uint16_t _duration, uint16_t _delay);
void drawToast();
void drawPopupMenu(const char *const list[], int8_t _numItems);
void drawCheckbox(uint8_t _xcord, uint8_t _ycord, bool _val);
bool modelIsfree(uint8_t _mdlNo);
void printModelName(char* _buff, uint8_t _lenBuff, uint8_t _mdlNo);
int8_t adjustTrim(int8_t _lowerLimit, int8_t _upperLimit, int8_t _val);
void drawTelemVolts(uint8_t xpos, uint8_t ypos);
void drawLoadingAnimation(uint8_t xpos, uint8_t ypos, uint8_t _size);
int incDecOnUpDown(int _val, int _lowerLimit, int _upperLimit, bool _enableWrap, uint8_t _state);
void drawFullScreenMsg(const char* str);


//-- Startup menu strings. Max 15 characters per string
#define NUM_ITEMS_STARTUP_MENU 3
char const startupStr0[] PROGMEM = "Calibrte sticks"; 
char const startupStr1[] PROGMEM = "Format EEPROM"; 
char const startupStr2[] PROGMEM = "Cancel";
const char* const startupMenu[] PROGMEM = { //table to refer to the strings
  startupStr0, startupStr1, startupStr2
};

//-- Timer popup menu strings. Max 15 characters per string
char const tmrStr0[] PROGMEM = "Start timer 2"; //shown if timer2 is paused
char const tmrStr1[] PROGMEM = "Stop timer 2";  //shown if timer2 is running
char const tmrStr2[] PROGMEM = "Reset timer 2";
char const tmrStr3[] PROGMEM = "Reset timer 1";
char const tmrStr4[] PROGMEM = "Setup timer 1";
#define NUM_ITEMS_TIMER_POPUP 4
const char* const timerMenuA[] PROGMEM = {
  tmrStr0, tmrStr2, tmrStr3, tmrStr4
};
const char* const timerMenuB[] PROGMEM = { 
  tmrStr1, tmrStr2, tmrStr3, tmrStr4
};

//-- Mixer popup menu strings. Max 15 characters per string
#define NUM_ITEMS_MIXER_POPUP 5
char const mxrStr0[] PROGMEM = "View mixes"; 
char const mxrStr1[] PROGMEM = "Reset mix"; 
char const mxrStr2[] PROGMEM = "Move mix to";
char const mxrStr3[] PROGMEM = "Copy mix to";
char const mxrStr4[] PROGMEM = "Reset all mixes";
const char* const mixerMenu[] PROGMEM = { //table to refer to the strings
  mxrStr0, mxrStr1, mxrStr2, mxrStr3, mxrStr4
};

//-- mixer sources name strings. 5 characters max
char const srcName0[]  PROGMEM = "roll"; 
char const srcName1[]  PROGMEM = "ptch";
char const srcName2[]  PROGMEM = "thrt";
char const srcName3[]  PROGMEM = "yaw";
char const srcName4[]  PROGMEM = "knob";
char const srcName5[]  PROGMEM = "max";
char const srcName6[]  PROGMEM = "SwA"; 
char const srcName7[]  PROGMEM = "SwB"; 
char const srcName8[]  PROGMEM = "SwC"; 
char const srcName9[]  PROGMEM = "SwD"; 
char const srcName10[] PROGMEM = "SwE"; 
char const srcName11[] PROGMEM = "SwF"; 
char const srcName12[] PROGMEM = "Slow";
char const srcName13[] PROGMEM = "Ail";
char const srcName14[] PROGMEM = "Ele";
char const srcName15[] PROGMEM = "Thrt";
char const srcName16[] PROGMEM = "Rud";
char const srcName17[] PROGMEM = "None";
char const srcName18[] PROGMEM = "Ch1";
char const srcName19[] PROGMEM = "Ch2";
char const srcName20[] PROGMEM = "Ch3";
char const srcName21[] PROGMEM = "Ch4";
char const srcName22[] PROGMEM = "Ch5";
char const srcName23[] PROGMEM = "Ch6";
char const srcName24[] PROGMEM = "Ch7";
char const srcName25[] PROGMEM = "Ch8";
char const srcName26[] PROGMEM = "Ch9";
char const srcName27[] PROGMEM = "Virt1";
char const srcName28[] PROGMEM = "Virt2";

const char* const srcNames[] PROGMEM = {
  srcName0, srcName1, srcName2, srcName3, srcName4, srcName5, srcName6, srcName7, 
  srcName8, srcName9, srcName10,srcName11, srcName12, srcName13, srcName14,
  srcName15, srcName16, srcName17, srcName18, srcName19, srcName20, srcName21, 
  srcName22, srcName23, srcName24, srcName25, srcName26, srcName27, srcName28
};

//Mix control switch strings
char const mxSwStr0[]  PROGMEM = "--";
char const mxSwStr1[]  PROGMEM = "SwA\x18";
char const mxSwStr2[]  PROGMEM = "SwA\x19";
char const mxSwStr3[]  PROGMEM = "SwB\x18";
char const mxSwStr4[]  PROGMEM = "SwB\x19";
char const mxSwStr5[]  PROGMEM = "SwC\x18";
char const mxSwStr6[]  PROGMEM = "SwC-";
char const mxSwStr7[]  PROGMEM = "SwC\x19";
char const mxSwStr8[]  PROGMEM = "!SwC\x18";
char const mxSwStr9[]  PROGMEM = "!SwC-";
char const mxSwStr10[]  PROGMEM = "!SwC\x19";
char const mxSwStr11[] PROGMEM = "SwD\x18";
char const mxSwStr12[] PROGMEM = "SwD\x19";
char const mxSwStr13[] PROGMEM = "SwE\x18";
char const mxSwStr14[] PROGMEM = "SwE\x19";
char const mxSwStr15[] PROGMEM = "SwF\x18";
char const mxSwStr16[] PROGMEM = "SwF\x19";
const char* const mixSwitchStr[] PROGMEM = { 
  mxSwStr0, mxSwStr1, mxSwStr2, mxSwStr3, mxSwStr4, mxSwStr5, mxSwStr6, mxSwStr7, mxSwStr8, 
  mxSwStr9, mxSwStr10, mxSwStr11, mxSwStr12, mxSwStr13, mxSwStr14, mxSwStr15, mxSwStr16
};

//sound mode strings. Max 5 characters
char const soundModeStr0[] PROGMEM = "Off"; 
char const soundModeStr1[] PROGMEM = "Alarm"; 
char const soundModeStr2[] PROGMEM = "NoKey"; 
char const soundModeStr3[] PROGMEM = "All";
const char* const soundModeStr[] PROGMEM = { 
  soundModeStr0, soundModeStr1, soundModeStr2, soundModeStr3
};
  
//Model popup menus. Max 15 characters
char const modelLoadStr[] PROGMEM    = "Load model";
char const modelCopyStr[] PROGMEM    = "Copy from";
char const modelRenameStr[] PROGMEM  = "Rename model";
char const modelResetStr[] PROGMEM   = "Reset model";
char const modelDeleteStr[] PROGMEM  = "Delete model";
char const modelCreateStr[] PROGMEM  = "Create model";

#define NUM_ITEMS_ACTIVE_MODEL_MENU  3
const char* const activeModelMenu[] PROGMEM = { 
  modelRenameStr, modelCopyStr, modelResetStr
};
#define NUM_ITEMS_INACTIVE_MODEL_MENU  2
const char* const inactiveModelMenu[] PROGMEM = { 
  modelLoadStr, modelDeleteStr
};
#define NUM_ITEMS_FREE_MODEL_MENU  1
const char* const freeModelMenu[] PROGMEM = {
  modelCreateStr
};

//Backlight mode strings
char const backlightModeStr0[] PROGMEM = "Off"; 
char const backlightModeStr1[] PROGMEM = "5s"; 
char const backlightModeStr2[] PROGMEM = "15s"; 
char const backlightModeStr3[] PROGMEM = "60s";
char const backlightModeStr4[] PROGMEM = "On";
const char* const backlightModeStr[] PROGMEM = { 
  backlightModeStr0, backlightModeStr1, backlightModeStr2, backlightModeStr3, backlightModeStr4
};

//RF power level strings
char const rfPowerStr0[] PROGMEM = "2mW";
char const rfPowerStr1[] PROGMEM = "5mW";
char const rfPowerStr2[] PROGMEM = "10mW";
char const rfPowerStr3[] PROGMEM = "25mW";
char const rfPowerStr4[] PROGMEM = "50mW";
const char* const rfPowerStr[] PROGMEM = {  
  rfPowerStr0, rfPowerStr1, rfPowerStr2, rfPowerStr3, rfPowerStr4
};

//Timer operator strings
char const tmrOperatorStr0[] PROGMEM = ">";
char const tmrOperatorStr1[] PROGMEM = "<";
char const tmrOperatorStr2[] PROGMEM = "abs>";
char const tmrOperatorStr3[] PROGMEM = "abs<";
const char* const tmrOperatorStr[] PROGMEM = {
  tmrOperatorStr0, tmrOperatorStr1, tmrOperatorStr2, tmrOperatorStr3
};

//-- Main menu strings. Max 16 characters per string
#define NUM_ITEMS_MAIN_MENU 9
char const main0[] PROGMEM = "Main menu"; //heading
char const main1[] PROGMEM = "Model";
char const main2[] PROGMEM = "Inputs";
char const main3[] PROGMEM = "Mixer";
char const main4[] PROGMEM = "Outputs";
char const main5[] PROGMEM = "System";
char const main6[] PROGMEM = "Telemetry";
char const main7[] PROGMEM = "Receiver";
char const main8[] PROGMEM = "About";
const char* const mainMenu[] PROGMEM = { //table to refer to the strings
  main0, main1, main2, main3, main4, main5, main6, main7, main8
};

//Assign indices for ui states
enum
{
  //Same order as in the main menu 
  MAIN_MENU = 0,
  MODE_MODEL,
  MODE_INPUTS,
  MODE_MIXER,
  MODE_OUTPUTS,
  MODE_SYSTEM,
  MODE_TELEMETRY,
  MODE_RECEIVER,
  MODE_ABOUT,
  
  //others
  
  HOME_SCREEN,
  
  MODE_CALIB,
  
  MODE_CHANNEL_MONITOR,
  
  POPUP_TIMER_MENU,
  MODE_TIMER_SETUP,
  
  MODE_MIXER_OUTPUT,
  POPUP_MIXER_MENU,
  POPUP_MOVE_MIX,
  POPUP_COPY_MIX,
  CONFIRMATION_MIXES_RESET,
  
  POPUP_ACTIVE_MODEL_MENU,
  POPUP_INACTIVE_MODEL_MENU,
  POPUP_FREE_MODEL_MENU,
  
  POPUP_RENAME_MODEL,
  POPUP_COPYFROM_MODEL,
  
  CONFIRMATION_MODEL_COPY,
  CONFIRMATION_MODEL_DELETE,
  CONFIRMATION_MODEL_RESET
};


char txtBuff[22]; //generic buffer for working with strings

uint8_t theScreen = HOME_SCREEN;
uint8_t focusedItem = 1; //The item that currently has focus in MODE Screens or popups
bool isEditMode = false;

//Model
uint8_t thisMdl;

//mixer
uint8_t thisMixNum = 0; //these are references
uint8_t destMixNum = 0; 

//generic stopwatch
unsigned long stopwatchElapsedTime = 0;
unsigned long stopwatchLastElapsedTime = 0;
unsigned long stopwatchLastPaused = 0;
bool stopwatchIsPaused = true;

//battery warning
bool battWarnDismissed = false;

//toast
const __FlashStringHelper* toastText;
unsigned long toastExpireTime;
unsigned long toastStartTime;


//==================================================================================================

void initialiseDisplay()
{
  display.begin();
  display.setTextWrap(false);
}

//==================================================================================================

void startStickCalibration()
{
  changeToScreen(MODE_CALIB); 
}

//================================== Generic messages ==============================================

void showFormattingMsg()
{
  display.clearDisplay();
  drawFullScreenMsg(PSTR("Formatting.."));
  display.display();
}

void showAnimation()
{
  display.clearDisplay();
  drawLoadingAnimation(60, 28, 2);
  display.display();
}

void showThrottleWarning()
{
  display.clearDisplay();
  drawFullScreenMsg(PSTR("Check throttle"));
  display.display();
}

void showEEWarning()
{
  display.clearDisplay();
  drawFullScreenMsg(PSTR("Bad EEPROM data\n\nPress any key"));
  display.display();
}

void showEEFormatConfirmation()
{
  display.clearDisplay();
  drawFullScreenMsg(PSTR("Format EEPROM?\n\nYes [Up]  \nNo  [Down]"));
  display.display();
}

///=================================================================================================
///                                Startup menu
///=================================================================================================

void handleStartupMenu()
{
  display.clearDisplay();
  drawPopupMenu(startupMenu, NUM_ITEMS_STARTUP_MENU);
  display.display();
  while (buttonCode != 0)  //wait for button release to prevent false trigger
  {
    readSwitchesAndButtons();
  }

  while(1)
  {
    delay(30);
 
    readSwitchesAndButtons();
    determineButtonEvent();
    
    display.clearDisplay();
    
    changeFocusOnUPDOWN(NUM_ITEMS_STARTUP_MENU);
    drawPopupMenu(startupMenu, NUM_ITEMS_STARTUP_MENU);
    
    display.display();
    
    uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
    
    if(_selection == 1) 
    {
      skipThrottleCheck = true;
      changeToScreen(MODE_CALIB);
      return; //exit
    }
    else if(_selection == 2)
    {
      EEPROM.write(EE_INITFLAG_ADDR, ~EEPROM.read(EE_INITFLAG_ADDR)); //clear flag
      return; //exit
    }
    else if(_selection == 3 || heldButton == SELECT_KEY) 
    {
      return; //exit
    }
  }
}

///=================================================================================================
///                                 Main user interface 
///=================================================================================================

void handleMainUI()
{
  ///--------------- INACTIVITY ALARM ---------------------
  if(Sys.inactivityMinutes > 0 && ((millis() - inputsLastMoved) > (Sys.inactivityMinutes * 60000UL)))
  {
    if(thisLoopNum % (30000 / fixedLoopTime) == 1) //repeat every 30 secs
      audioToPlay = AUDIO_INACTIVITY;
  }

  /// --------------- TELEMETRY WARN ----------------------
  
  static uint8_t _tCounter = 0;
  static uint32_t _tWarnEntryLoopNum = 0;
  static bool _tWarnStarted = false;
  
  if(Sys.telemAlarmEnabled && telem_volts != 0x0FFF)
  {
    //check and increment or decrement counter
    if(telem_volts < Model.telemVoltsThresh)
    {
      if(!_tWarnStarted) 
        ++_tCounter;
    }
    else
    {
      if(_tCounter > 0) --_tCounter;
      if(_tCounter == 0) _tWarnStarted = false;
    }
    
    //if more than 4 seconds, trigger alarm
    if(_tCounter > (4000 / fixedLoopTime) && !_tWarnStarted) 
    {
      _tWarnStarted = true;
      _tWarnEntryLoopNum = thisLoopNum;
    }
    
    if(_tWarnStarted && ((thisLoopNum - _tWarnEntryLoopNum) % (5000 / fixedLoopTime) == 1))
      audioToPlay = AUDIO_TELEMWARN;
  }
  
  if(telem_volts == 0x0FFF)
  {
    _tCounter = 0;
    _tWarnStarted = false;
  }
  
  ///--------------- GENERIC STOPWATCH --------------------
  if(!stopwatchIsPaused) //run
    stopwatchElapsedTime = stopwatchLastElapsedTime + millis() - stopwatchLastPaused;
  else //pause
  {
    stopwatchLastElapsedTime = stopwatchElapsedTime;
    stopwatchLastPaused = millis();
  }
  
  ///---------------- TIMER 1 -----------------------------
  //play sound when timer expires (if count down timer)
  static bool _alarmTriggered = false;
  if (Model.timer1InitMins > 0)
  {
    uint32_t _initMillis = Model.timer1InitMins * 60000UL;
    if((timer1ElapsedTime > _initMillis) && timer1ElapsedTime < (_initMillis + 500) && !_alarmTriggered)
    {
      audioToPlay = AUDIO_TIMERELAPSED;
      _alarmTriggered = true;
    }
    else if(timer1ElapsedTime < _initMillis)
      _alarmTriggered = false;
  }
  
  /// --------------- TX LOW BATTERY WARN -----------------
  static uint32_t battWarnMillisQQ = millis();
  if(battState == BATTLOW)
  {
    if(!battWarnDismissed)
    {
      //show warning
      display.clearDisplay();
      drawFullScreenMsg(PSTR("Battery low"));
      display.display();
      
      audioToPlay = AUDIO_BATTERYWARN; 
      
      //dismiss warning
      if((clickedButton > 0 || millis() - battWarnMillisQQ > 3000))
      {
        battWarnDismissed = true;
        battWarnMillisQQ = millis();
      }
      return; 
    }
    //remind low battery every 10 minutes
    if(battWarnDismissed && (millis() - battWarnMillisQQ > 600000UL)) 
    {
      battWarnDismissed = false;
      battWarnMillisQQ = millis();
    }
  }
  else
    battWarnMillisQQ = millis();

  /// ---------------- BIND STATUS-----------------------------------
  if(bindStatusCode == 1)
  {
    makeToast(F("Bind success"), 3000, 0);
    audioToPlay = AUDIO_BIND_SUCCESS;
  }
  else if(bindStatusCode == 2)
  {
    makeToast(F("Bind failed"), 3000, 0);
  }
  bindStatusCode = 0;
  
  ///----------------- MAIN STATE MACHINE ---------------------------
  switch (theScreen)
  {
    case HOME_SCREEN:
      {
        enum {DEFAULTHOME = 0, TRIMMODE };
        static uint8_t homeScreenMode = DEFAULTHOME;
        static uint8_t selectedTrim = 0; //AIL, ELE, THR, RUD
        static bool trimIsPendingSave = false;
        
        //----------Show a graphical battery gauge---------
        //This crude battery gauge doesn't indicate state of charge; only battery voltage
        display.drawRect(0, 0, 18, 7, BLACK);
        display.drawVLine(18, 2, 3, BLACK);
        static int8_t lastNumOfBars = 19;
        if(battState == BATTHEALTY)
        {
          int8_t numOfBars = (20L *(battVoltsNow - battVoltsMin)) / (battVoltsMax - battVoltsMin);
          if(numOfBars > 19) 
            numOfBars = 19;
          if (numOfBars > lastNumOfBars && numOfBars - lastNumOfBars < 2) //prevent jitter at boundaries
            numOfBars = lastNumOfBars;
          for(int8_t i = 0; i < (numOfBars/4 + 1); i++)
            display.fillRect(2 + i * 3, 2, 2, 3, BLACK);
          lastNumOfBars = numOfBars;
        }

        //---------show dualrate icon --------
        if (swBEngaged && Model.dualRate > 0)
          display.drawBitmap(66, 1, dualrate_icon, 13, 6, 1);
        
        //--------show rf icon and tx power level ----
        if (Sys.rfOutputEnabled)
        {
          display.drawBitmap(85, 0, rf_icon, 7, 7, 1);
          for(uint8_t i = 0; i < Sys.rfPower + 2; i++)
            display.drawVLine(91 + i, 6 - i, i + 1, BLACK);
        }
        
        //--------show mute icon------------
        if (Sys.soundMode == SOUND_OFF)
          display.drawBitmap(41, 0, mute_icon, 7, 7, 1);

        //------show model name-----------
        display.setCursor(14, 16);
        printModelName(Model.modelName, sizeof(Model.modelName), Sys.activeModel);
        
        //------show telemetry voltage ----
        if(Sys.telemVoltsOnHomeScreen)
          drawTelemVolts(78, 16);

        // draw separator
        display.drawHLine(14, 27, 99,BLACK);

        //----show timer 1 ---------
        display.setCursor(14, 32);
        if(Model.timer1InitMins == 0) //a countup timer
          printHHMMSS(timer1ElapsedTime);
        else //a count down timer
        {
          uint32_t _initMillis = Model.timer1InitMins * 60000UL;
          if(timer1ElapsedTime < _initMillis)
          {
            uint32_t ttqq = _initMillis - timer1ElapsedTime;
            printHHMMSS(ttqq + 999); //add 999ms so the displayed time doesnt 
            //change immediately upon running the timer
          }
          else
          {
            uint32_t ttqq = timer1ElapsedTime - _initMillis;
            if(ttqq >= 1000) //prevents displaying -00:00
            { 
              display.print(F("-"));
              printHHMMSS(ttqq);
            }
            else
              printHHMMSS(ttqq);
          }
        }

        //------- Show generic timer ------------
        display.setCursor(14, 45);
        printHHMMSS(stopwatchElapsedTime);

        //---------------------------------------

        if(homeScreenMode == TRIMMODE)
        {
          display.drawBitmap(53, 0, lock_icon, 6, 7, 1); 
          //handle SELECT_KEY
          if(pressedButton == SELECT_KEY)
            audioToPlay = AUDIO_NONE;
          if(clickedButton == SELECT_KEY) 
          {
            selectedTrim++;
            if(selectedTrim > 3)
              selectedTrim = 0;
            audioToPlay = AUDIO_TRIM_AIL + selectedTrim;
          }
          //adjust
          int8_t oldTrimVal = Model.trim[selectedTrim];
          Model.trim[selectedTrim] = adjustTrim(-20, 20, Model.trim[selectedTrim]);
          if(Model.trim[selectedTrim] != oldTrimVal)
            trimIsPendingSave = true;
          
          //show current trim
          display.setCursor(88, 34);
          strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_AIL + selectedTrim])), sizeof(txtBuff));
          display.print(txtBuff);
          display.setCursor(88, 42);
          display.print(F("trim"));
          display.drawRect(86, 32, 27, 20, BLACK);
          
          //draw sliders
          display.drawHLine(73, 62, 41, BLACK);
          display.drawVLine(126, 17, 41, BLACK);
          display.drawVLine(1, 17, 41, BLACK);
          display.drawHLine(14, 62, 41, BLACK);
          //draw thumbs
          display.drawRect(Model.trim[0] + 92, 61, 3, 3, BLACK);
          display.drawRect(125, 36 - Model.trim[1], 3, 3, BLACK);
          display.drawRect(0, 36 - Model.trim[2], 3, 3, BLACK);
          display.drawRect(Model.trim[3] + 33, 61, 3, 3, BLACK); 
          //draw midpoints
          display.drawPixel(93, 62, WHITE);
          display.drawPixel(126, 37, WHITE);
          display.drawPixel(1, 37, WHITE);
          display.drawPixel(34, 62, WHITE);           
        }
        
        //------------------------------
        
        //save periodically
        if(trimIsPendingSave && thisLoopNum % (5000 / fixedLoopTime) == 1)
        {
          eeSaveModelData(Sys.activeModel);
          trimIsPendingSave = false;
        }
        
        if (homeScreenMode == DEFAULTHOME && clickedButton == SELECT_KEY)
          changeToScreen(MAIN_MENU);
        else if (homeScreenMode == DEFAULTHOME && clickedButton == UP_KEY)
          changeToScreen(MODE_CHANNEL_MONITOR);
        else if (homeScreenMode == DEFAULTHOME && clickedButton == DOWN_KEY)
          changeToScreen(POPUP_TIMER_MENU);
        else if(homeScreenMode == DEFAULTHOME && heldButton == UP_KEY)
        {        
          homeScreenMode = TRIMMODE;
          audioToPlay = AUDIO_TRIM_MODE_ENTERED;
          heldButton = 0;
        }
        else if(homeScreenMode == TRIMMODE && heldButton == SELECT_KEY)
        {            
          homeScreenMode = DEFAULTHOME;
          audioToPlay = AUDIO_TRIM_MODE_EXITED;
          heldButton = 0;
        }
      }
      break;
      
    case POPUP_TIMER_MENU:
      {
        changeFocusOnUPDOWN(NUM_ITEMS_TIMER_POPUP);

        if(stopwatchIsPaused)
          drawPopupMenu(timerMenuA, NUM_ITEMS_TIMER_POPUP);
        else 
          drawPopupMenu(timerMenuB, NUM_ITEMS_TIMER_POPUP);
        
        uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
        if(_selection == 1) //play or pause stopwatch (timer 2)
        {
          stopwatchIsPaused = !stopwatchIsPaused; //Pause/Play stopwatch
          changeToScreen(HOME_SCREEN);
        }
        else if(_selection == 2) //reset timer 2
        {
          stopwatchElapsedTime = 0;
          stopwatchLastElapsedTime = 0;
          stopwatchLastPaused = millis();
          changeToScreen(HOME_SCREEN);
        }
        else if(_selection == 3) //reset timer 1
        {
          resetTimer1();
          changeToScreen(HOME_SCREEN);
        }
        else if(_selection == 4) //reset timer 1
          changeToScreen(MODE_TIMER_SETUP);
          
        else if(heldButton == SELECT_KEY) //exit
          changeToScreen(HOME_SCREEN);
      }
      break;
      
    case MODE_TIMER_SETUP:
      {
        drawHeader(PSTR("Timer 1"));
        
        display.setCursor(0, 10);
        display.print(F("Control:   "));
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[Model.timer1ControlSrc])), sizeof(txtBuff));
        display.print(txtBuff);
        
        display.setCursor(0, 19);
        display.print(F("Operator:  "));
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(tmrOperatorStr[Model.timer1Operator])), sizeof(txtBuff));
        display.print(txtBuff);
        
        display.setCursor(0, 28);
        display.print(F("Value:     "));
        display.print(Model.timer1Value);
        
        display.setCursor(0, 37);
        display.print(F("Initial:   "));
        display.print(Model.timer1InitMins);
        display.print(F(" min"));

        changeFocusOnUPDOWN(4);
        toggleEditModeOnSelectClicked();
        drawCursor(58, (focusedItem * 9) + 1);
        
        if (focusedItem == 1)
        {
          Model.timer1ControlSrc = incDecOnUpDown(Model.timer1ControlSrc, 0, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
          //Validate sources. Allowed sources are raw sticks, knob, switches, channels, virtuals
          while(Model.timer1ControlSrc == IDX_100PERC 
                || (Model.timer1ControlSrc >= IDX_SLOW1 && Model.timer1ControlSrc <= IDX_RUD))
          {
            Model.timer1ControlSrc = incDecOnUpDown(Model.timer1ControlSrc, 0, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
          }
        }
        else if(focusedItem == 2)
          Model.timer1Operator = incDecOnUpDown(Model.timer1Operator, 0, NUM_TIMER_OPERATORS - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 3)
          Model.timer1Value = incDecOnUpDown(Model.timer1Value, -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 4)
          Model.timer1InitMins = incDecOnUpDown(Model.timer1InitMins, 0, 240, NOWRAP, PRESSED_OR_HELD);
          
      
        if (heldButton == SELECT_KEY)
        {
          eeSaveModelData(Sys.activeModel);
          changeToScreen(HOME_SCREEN);
        }
      }
      break;
      
    case MODE_CHANNEL_MONITOR:
      {
        drawHeader(PSTR("Outputs"));
        
        for(uint8_t i = 0; i < NUM_PRP_CHANNLES && i < 9; i++)
        {
          if(i < 5)
            display.setCursor(11, 12 + i * 10);
          else
            display.setCursor(71, 12 + (i - 5) * 10);
          
          display.print(F("Ch"));          
          display.print(1 + i);  
          display.print(F(":"));          
          display.print(channelOut[i] / 5);
        }
        
        if(heldButton == SELECT_KEY)
          changeToScreen(HOME_SCREEN);
      }
      break;

    case MAIN_MENU:
      {
        static uint8_t menuTopItem = 1;
        static uint8_t menuHighlightedItem = 1;
        
        int8_t _numMenuItems = NUM_ITEMS_MAIN_MENU - 1; //exclude heading in count
  
        //------ handle navigation
        isEditMode = true;
        menuHighlightedItem = incDecOnUpDown(menuHighlightedItem, _numMenuItems, 1, WRAP, SLOW_CHANGE);
        if (menuHighlightedItem < menuTopItem || menuHighlightedItem >= (menuTopItem + 4))
          menuTopItem = incDecOnUpDown(menuTopItem, _numMenuItems - 3, 1, WRAP, SLOW_CHANGE);
        isEditMode = false;
        
        //------ show heading
        strlcpy_P(txtBuff, (char *)pgm_read_word(&mainMenu[0]), sizeof(txtBuff));
        uint8_t txtWidthPix = strlen(txtBuff) * 6;
        uint8_t headingX_offset = (display.width() - txtWidthPix) / 2; //middle align heading
        display.setCursor(headingX_offset, 2);
        display.println(txtBuff);
        display.drawHLine(0, 11, 128, BLACK);
        
        //------ fill menu slots
        for (uint8_t i = 0; i < 4 && i < _numMenuItems; i++) //4 item slots
        {
          strlcpy_P(txtBuff, (char *)pgm_read_word(&mainMenu[menuTopItem + i]), sizeof(txtBuff));
          if (menuHighlightedItem == (menuTopItem + i)) //highlight selection
          {
            display.fillRect(6, 13 + i * 13, 116, 11, BLACK);
            display.setTextColor(WHITE);
          }
          display.setCursor(14, 15 + i * 13);
          display.println(txtBuff);
          display.setTextColor(BLACK);
        }
        
        //------ draw a simple scroll bar
        if(_numMenuItems > 4)
        {
          const uint8_t viewPortHeight = 52; //4*13
          uint8_t contentHeight = _numMenuItems * 13;
          uint8_t contentY = (menuTopItem - 1) * 13;
          uint8_t barSize = ((int16_t)viewPortHeight * viewPortHeight) / contentHeight;
          barSize += 1; //Add 1 to compensate for truncation error
          uint8_t barYPostn = ((int16_t)viewPortHeight * contentY) / contentHeight;
          display.drawVLine(125 , 12 + barYPostn, barSize, BLACK);
        }

        //----- handle SELECT_KEY
        if(clickedButton == SELECT_KEY)
          changeToScreen(menuHighlightedItem);
        else if(heldButton == SELECT_KEY)
          changeToScreen(HOME_SCREEN);
      }
      break;
      
    case MODE_MODEL:
      {
        drawHeader((char *)pgm_read_word(&mainMenu[MODE_MODEL]));
        
        //------ scrollable list of models ----------
        
        static uint8_t _top;
        static bool _viewInitialised = false;
        if(!_viewInitialised)
        {
          thisMdl = Sys.activeModel;
          if(Sys.activeModel > 6) _top = Sys.activeModel - 5;
          else _top = 1;
          _viewInitialised = true;
        }
        
        // handle navigation
        isEditMode = true;
        thisMdl = incDecOnUpDown(thisMdl, maxNumOfModels, 1, WRAP, SLOW_CHANGE);
        if(thisMdl < _top || thisMdl >= (_top + 6))
          _top = incDecOnUpDown(_top, maxNumOfModels - 5, 1, WRAP, SLOW_CHANGE);
        isEditMode = false;
        
        // fill list
        for(uint8_t i = 1; i <= 6 && i <= maxNumOfModels; i++)
        {
          uint8_t _mdlNo = _top - 1 + i;
          
          if(_mdlNo == Sys.activeModel) //indicate it is active
          {
            display.setCursor(23, 9 * i + 1);
            display.print(F("*"));
          }
          
          if(thisMdl == _mdlNo) //highlight
          {
            if(_mdlNo < 10) display.fillRect(30, 9 * i, 7, 9, BLACK);
            else display.fillRect(30, 9 * i, 14, 9, BLACK);
            display.setTextColor(WHITE);
          }
          display.setCursor(31, 9 * i + 1);
          display.print(_mdlNo);
          display.setTextColor(BLACK);
          
          display.setCursor(46, 9 * i + 1);
          if(!modelIsfree(_mdlNo))
          {
            //print name
            eeCopyModelName(txtBuff, _mdlNo);
            printModelName(txtBuff, sizeof(txtBuff), _mdlNo);
          }
        }

        //----- end of list ----------------------
        
        if(clickedButton == SELECT_KEY)
        {
          if(thisMdl == Sys.activeModel)
            changeToScreen(POPUP_ACTIVE_MODEL_MENU);
          else if(!modelIsfree(thisMdl))
            changeToScreen(POPUP_INACTIVE_MODEL_MENU);
          else if(modelIsfree(thisMdl))
            changeToScreen(POPUP_FREE_MODEL_MENU);
        }

        if (heldButton == SELECT_KEY)
        {
          _viewInitialised = false;//reset view
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case POPUP_ACTIVE_MODEL_MENU:
      {
        changeFocusOnUPDOWN(NUM_ITEMS_ACTIVE_MODEL_MENU);
        drawPopupMenu(activeModelMenu, NUM_ITEMS_ACTIVE_MODEL_MENU);
        
        uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
        
        if(_selection == 1) //rename model
          changeToScreen(POPUP_RENAME_MODEL);
        else if(_selection == 2) //copy from model
          changeToScreen(POPUP_COPYFROM_MODEL);
        else if(_selection == 3) //reset model
          changeToScreen(CONFIRMATION_MODEL_RESET);
        
        if(heldButton == SELECT_KEY) //exit
          changeToScreen(MODE_MODEL);
      }
      break;
      
    case POPUP_INACTIVE_MODEL_MENU:
      {
        changeFocusOnUPDOWN(NUM_ITEMS_INACTIVE_MODEL_MENU);
        drawPopupMenu(inactiveModelMenu, NUM_ITEMS_INACTIVE_MODEL_MENU);
        
        uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
        
        if(_selection == 1) //load model
        {
          //Save the active model before changing to another model
          eeSaveModelData(Sys.activeModel);
          //load into ram
          eeReadModelData(thisMdl);
          //set as active model
          Sys.activeModel = thisMdl; 
          
          //reset other stuff
          resetTimer1();
          Sys.rfOutputEnabled = false;
          //save system
          eeSaveSysConfig();
          
          changeToScreen(MODE_MODEL);
        }
        else if(_selection == 2) //delete model
          changeToScreen(CONFIRMATION_MODEL_DELETE);

        if(heldButton == SELECT_KEY) //exit
          changeToScreen(MODE_MODEL);
      }
      break;
      
    case POPUP_FREE_MODEL_MENU:
      {
        changeFocusOnUPDOWN(NUM_ITEMS_FREE_MODEL_MENU);
        drawPopupMenu(freeModelMenu, NUM_ITEMS_FREE_MODEL_MENU);
        
        uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
        
        if(_selection == 1) //create model
        {
          //Save the current active model first
          eeSaveModelData(Sys.activeModel);
          
          //reset timer1 and disable rf output
          resetTimer1();
          Sys.rfOutputEnabled = false;
          //save system
          eeSaveSysConfig();
          
          //create model and set it active
          eeCreateModel(thisMdl);
          eeReadModelData(thisMdl);
          Sys.activeModel = thisMdl;
          //save system
          eeSaveSysConfig();
          
          changeToScreen(MODE_MODEL);
        }

        if(heldButton == SELECT_KEY) //exit
          changeToScreen(MODE_MODEL);
      }
      break;
      
    case POPUP_RENAME_MODEL:
      {
        display.drawRect(15,11,97,40,BLACK);
        
        display.setCursor(19,14);
        display.print(F("Rename MODEL"));
        display.print(Sys.activeModel); 
        display.setCursor(19,23);
        display.print(F("Name:  "));
        display.print(Model.modelName);
        
        isEditMode = true;

        static uint8_t charPos = 0;
        uint8_t thisChar = Model.modelName[charPos] ;
        
        //----mapping characters---
        //Z to A (ascii 90 to 65) --> 0 to 25
        //space  (ascii 32) --> 26
        //a to z (ascii 97 to 122) --> 27 to 52
        //Symbols and numbers (ascii 45 to 57) --> 53 to 65

        if(thisChar == 32)  thisChar = 26;
        else if(thisChar <= 57) thisChar += 8;
        else if(thisChar <= 90) thisChar = 90 - thisChar ;
        else if(thisChar <= 122) thisChar -= 70;
        
        //adjust 
        thisChar = incDecOnUpDown(thisChar, 65, 0, NOWRAP, SLOW_CHANGE);

        //map back
        if(thisChar <= 25) thisChar = 90 - thisChar;
        else if(thisChar == 26) thisChar = 32; 
        else if(thisChar <= 52) thisChar += 70;
        else if(thisChar <= 65) thisChar -= 8;

        //write
        Model.modelName[charPos] = thisChar;
        
        //draw blinking cursor
        if ((millis() - buttonReleaseTime) % 1000 < 500 || buttonCode > 0)
          display.fillRect(61 + 6*charPos, 31, 5, 2, BLACK);
        
        //change to next character
        if(clickedButton == SELECT_KEY)
          charPos++;
        else if(heldButton == SELECT_KEY && charPos > 0)
        {
          charPos--;
          heldButton = 0; 
        }

        //--- clear model name ----
        if(charPos == 0 && heldButton == SELECT_KEY && millis() - buttonStartTime >= 1500)
        {
          setDefaultModelName();
          heldButton = 0;
        }
        
        //--- done renaming, exit --- 
        if(charPos == (sizeof(Model.modelName) - 1))
        {
          charPos = 0;
          eeSaveModelData(Sys.activeModel);
          changeToScreen(MODE_MODEL); 
        }
      }
      break;

    case POPUP_COPYFROM_MODEL:
      {
        //change source model
        isEditMode = true;
        thisMdl = incDecOnUpDown(thisMdl, 1, maxNumOfModels, WRAP, SLOW_CHANGE);
        //validate
        while(modelIsfree(thisMdl))
          thisMdl = incDecOnUpDown(thisMdl, 1, maxNumOfModels, WRAP, SLOW_CHANGE);
        
        display.drawRect(15,11,97,40,BLACK); //draw bounding box
        
        display.setCursor(19,14);
        display.print(F("Copy data"));
        display.setCursor(19,23);
        display.print(F("from:  "));
        //print name
        eeCopyModelName(txtBuff, thisMdl); 
        printModelName(txtBuff, sizeof(txtBuff), thisMdl);

        drawCursor(53, 23);

        if(clickedButton == SELECT_KEY)
        {
          if(thisMdl == Sys.activeModel)
          {
            makeToast(F("Can't copy"), 2500, 0);
            changeToScreen(MODE_MODEL);
          }
          else
            changeToScreen(CONFIRMATION_MODEL_COPY);
        }
        else if(heldButton == SELECT_KEY) //go back
        {
          thisMdl = Sys.activeModel;
          changeToScreen(MODE_MODEL);
        }
      }
      break;
      
    case CONFIRMATION_MODEL_COPY:
      {
        drawFullScreenMsg(PSTR("All model data and\ntelemetry will be\noverwritten.\nContinue?\n\nYes [Up]  \nNo  [Down]"));
        if(clickedButton == UP_KEY)
        {
          //temporarily store model name as we shall maintain it 
          strlcpy(txtBuff, Model.modelName, sizeof(txtBuff));
          //load source model into ram
          eeReadModelData(thisMdl);
          //restore model name
          strlcpy(Model.modelName, txtBuff, sizeof(Model.modelName));
          //save
          eeSaveModelData(Sys.activeModel);
          
          //reset other stuff
          resetTimer1();
          Sys.rfOutputEnabled = false;
          eeSaveSysConfig();
          
          thisMdl = Sys.activeModel; //reinit
          
          makeToast(F("Copied"), 2500, 0);
          changeToScreen(MODE_MODEL);
        }
        else if(clickedButton == DOWN_KEY || heldButton == SELECT_KEY)
        {
          thisMdl = Sys.activeModel;
          changeToScreen(MODE_MODEL);
        }
      }
      break;
      
    case CONFIRMATION_MODEL_RESET:
      {
        drawFullScreenMsg(PSTR("All model data and\ntelemetry will be\nreset. Continue?\n\nYes [Up]  \nNo  [Down]"));
        if(clickedButton == UP_KEY)
        {
          setDefaultModelBasicParams();
          setDefaultModelMixerParams();
          eeSaveModelData(Sys.activeModel);
          
          //reset other stuff
          resetTimer1();
          Sys.rfOutputEnabled = false;
          eeSaveSysConfig();
          
          makeToast(F("Model reset"), 2500, 0);
          changeToScreen(MODE_MODEL);
        }
        else if(clickedButton == DOWN_KEY || heldButton == SELECT_KEY)
        {
          changeToScreen(MODE_MODEL);
        }
      }
      break;
      
    case CONFIRMATION_MODEL_DELETE:
      {
        drawFullScreenMsg(PSTR("Delete model?\n\nYes [Up]  \nNo  [Down]"));
        if(clickedButton == UP_KEY)
        {
          eeDeleteModel(thisMdl);
          changeToScreen(MODE_MODEL);
        }
        else if(clickedButton == DOWN_KEY || heldButton == SELECT_KEY)
        {
          changeToScreen(MODE_MODEL);
        }
      }
      break;
      
    case MODE_INPUTS:
      {
        drawHeader((char *)pgm_read_word(&mainMenu[MODE_INPUTS]));

        enum{AIL_CURVE = 0, ELE_CURVE = 1, RUD_CURVE = 2, THR_CURVE, SLOW1, RAW_INPUTS};
        static uint8_t _page = AIL_CURVE;
        
        if (focusedItem == 1)
          _page = incDecOnUpDown(_page, 0, 5, WRAP, SLOW_CHANGE);
          
        ///////////////// RATES AND EXPO ////////////////////////////////////////
        if(_page == AIL_CURVE || _page == ELE_CURVE || _page == RUD_CURVE)  
        {  
          changeFocusOnUPDOWN(4);
          toggleEditModeOnSelectClicked();
          
          int8_t *_rate = &Model.rateNormal[_page];
          int8_t *_expo = &Model.expoNormal[_page];
          
          if(swBEngaged && ((Model.dualRate >> _page) & 0x01)) //sport
          {
            _rate = &Model.rateSport[_page];
            _expo = &Model.expoSport[_page];
            
            display.drawRect(0, 49, 33, 11, BLACK);
            display.setCursor(2, 51);
            display.print(F("Sport"));
          }

          //Adjust values
          if (focusedItem == 2)
            *_rate = incDecOnUpDown(*_rate, 0, 100, NOWRAP, PRESSED_OR_HELD); 
          else if (focusedItem == 3)
            *_expo = incDecOnUpDown(*_expo, -100, 100, NOWRAP, PRESSED_OR_HELD);
          else if (focusedItem == 4 && isEditMode)
          {
            if(pressedButton == UP_KEY || pressedButton == DOWN_KEY)
              Model.dualRate ^= 1 << _page; //toggle bit
          }
        
          //Show text
          
          display.setCursor(8,11);
          if(_page == RUD_CURVE) 
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_RUD])), sizeof(txtBuff)); 
          else 
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_AIL + _page])), sizeof(txtBuff));
          display.print(txtBuff);
          display.drawHLine(8, 19, strlen(txtBuff) * 6, BLACK);
          
          display.setCursor(0, 22);
          display.print(F("Rate:  "));
          display.print(*_rate);
          display.print(F("%"));
          
          display.setCursor(0, 31);
          display.print(F("Expo:  "));
          display.print(*_expo);
          display.print(F("%"));
          
          display.setCursor(0, 40);
          display.print(F("D/R:   "));
          drawCheckbox(42, 40, (Model.dualRate >> _page) & 0x01);
        
          //draw graph 
          display.drawVLine(100, 11, 51, BLACK);
          display.drawHLine(74, 36, 52, BLACK);
          for(int i = 0; i <= 25; i++)
          {
            int _output = calcRateExpo(i * 20, *_rate, *_expo) / 20;
            display.drawPixel(100 + i, 36 - _output, BLACK);
            display.drawPixel(100 - i, 36 + _output, BLACK);
          }
          
          //draw stick input marker
          int _stickInpt[3] = {rollIn, pitchIn, yawIn};
          int _output = calcRateExpo(_stickInpt[_page], *_rate, *_expo) / 20;
          display.fillRect(99 + _stickInpt[_page]/20, 35 - _output, 3, 3, BLACK);
        }
    
        //////////////// THROTTLE CURVE ////////////////////////////////////////
        if(_page == THR_CURVE)
        {
          changeFocusOnUPDOWN(3);
          toggleEditModeOnSelectClicked();
          
          static uint8_t _thisPt = 0;
          
          //adjust 
          if(focusedItem == 2)
            _thisPt = incDecOnUpDown(_thisPt, 0, 4, WRAP, SLOW_CHANGE);
          else if(focusedItem == 3)
            Model.throttlePts[_thisPt] = incDecOnUpDown(Model.throttlePts[_thisPt], -100, 100, NOWRAP, PRESSED_OR_HELD);

          //-----draw text
          display.setCursor(8, 11);
          strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_THRTL_CURV])), sizeof(txtBuff));
          display.print(txtBuff);
          display.drawHLine(8, 19, strlen(txtBuff) * 6, BLACK);
          
          display.setCursor(0, 22);
          display.print(F("Pt :   "));
          display.write(97 + _thisPt); //a,b,c,d,e
        
          display.setCursor(0, 31);
          display.print(F("Val:   "));
          display.print(Model.throttlePts[_thisPt]);
          
          //-----draw graph
          //axes
          display.drawVLine(100, 11, 51, BLACK);
          display.drawHLine(74, 36, 52, BLACK);
          
          //Interpolate and draw points. We use x cordinate to estimate corresponding y cordinate
          //Actual plot area is 50x50.
          int xpts[5] = {-500, -250, 0, 250, 500};
          int ypts[5];
          for(uint8_t i = 0; i < 5; i++)
            ypts[i] = Model.throttlePts[i] * 5;
          
          for (int xval = -25; xval <= 25; xval++) //50x50 grid so first point is -25
          {
            int yval = linearInterpolate(xpts, ypts, 5, xval * 20) / 20;
            display.drawPixel(100 + xval, 36 - yval, BLACK); //plot points
          }
          
          //trace source
          int yy = linearInterpolate(xpts, ypts, 5, throttleIn) / 20;
          display.fillRect(99 + (throttleIn / 20), 35 - yy, 3, 3, BLACK);
         
          //show point we are adjusting
          if(focusedItem == 2 || focusedItem == 3)
          {
            int _qq = linearInterpolate(xpts, ypts, 5, (_thisPt * 250) - 500) / 20;
            display.fillRect(99 + xpts[_thisPt]/20, 35 - _qq ,3, 3, WHITE);
            display.drawRect(99 + xpts[_thisPt]/20, 35 - _qq ,3, 3, BLACK);
          }
        }
        
        ////////////////// SLOWED INPUTS //////////////////////////////////////
        if(_page == SLOW1)
        {
          changeFocusOnUPDOWN(4);
          toggleEditModeOnSelectClicked();
          
          display.setCursor(8, 11);
          strlcpy_P(txtBuff, PSTR("(Slow)"), sizeof(txtBuff));
          display.print(txtBuff);
          display.drawHLine(8, 19, strlen(txtBuff) * 6, BLACK);
          
          display.setCursor(0, 22);
          display.print(F("Up:    "));
          display.print(Model.slow1Up / 10);
          display.print(F("."));
          display.print(Model.slow1Up % 10);
          display.print(F("s"));
          
          display.setCursor(0, 31);
          display.print(F("Down:  "));
          display.print(Model.slow1Down / 10);
          display.print(F("."));
          display.print(Model.slow1Down % 10);
          display.print(F("s"));
          
          display.setCursor(0, 40);
          display.print(F("Src:   "));
          strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[Model.slow1Src])), sizeof(txtBuff));
          display.print(txtBuff);
          
          if(focusedItem == 2)
            Model.slow1Up = incDecOnUpDown(Model.slow1Up, 0, 50, NOWRAP, PRESSED_OR_HELD);
          else if(focusedItem == 3)
            Model.slow1Down = incDecOnUpDown(Model.slow1Down, 0, 50, NOWRAP, PRESSED_OR_HELD);
          else if(focusedItem == 4)
            Model.slow1Src = incDecOnUpDown(Model.slow1Src, IDX_SWA, IDX_SWF, NOWRAP, PRESSED_OR_HELD);
        }

        ////////////////// RAW /////////////////////////////////////////////////
        if(_page == RAW_INPUTS)
        {
          toggleEditModeOnSelectClicked();
          
          display.setCursor(8, 11);
          display.print(F("Raw"));
          display.drawHLine(8, 19, 18, BLACK);
          
          //show sticks and knob
          int _stickVal[5] = {rollIn, pitchIn, throttleIn, yawIn, knobIn}; //order as in source names
          for(uint8_t i = 0; i < 5; i++)
          {
            display.setCursor(11, 21 + i * 9);
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_ROLL + i])), sizeof(txtBuff));
            display.print(txtBuff);
            display.setCursor(39, 21 + i * 9);
            display.print(_stickVal[i]/5);
          }
          //show switches
          uint8_t _swState[6] = {swAEngaged, swBEngaged, swCState, swDEngaged, swEEngaged, swFEngaged};
          for(uint8_t i = 0; i < 6; i++)
          {
            uint8_t _ycord = 12 + i * 9;
            display.setCursor(71, _ycord);
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_SWA + i])), sizeof(txtBuff));
            display.print(txtBuff);
            
            if(_swState[i] == 0) display.print(F(" -100"));
            else if(_swState[i] == 1) display.print(F(" 100"));
            else display.print(F(" 0"));
          }
        }
        
        ////// Show cursor
        
        if(focusedItem == 1) drawCursor(0, 11);
        else drawCursor(34, (focusedItem * 9) + 4);

        ////// Exit
        if (heldButton == SELECT_KEY)
        {
          eeSaveModelData(Sys.activeModel);
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case MODE_MIXER:
      {
        drawHeader((char *)pgm_read_word(&mainMenu[MODE_MIXER]));
        
        display.setCursor(0, 8);
        display.print(F("Mix no:  #"));
        display.print(thisMixNum + 1);
        
        display.setCursor(0, 16);
        display.print(F("Output:  "));
        uint8_t _outNameIndex = Model.mixOut[thisMixNum];
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_outNameIndex])), sizeof(txtBuff));
        display.print(txtBuff);
        
        display.setCursor(0, 24);
        display.print(F("Input:   "));
        uint8_t _inName[2] = {Model.mixIn1[thisMixNum], Model.mixIn2[thisMixNum]};
        for(uint8_t i = 0; i < 2; i++)
        {
          display.setCursor(54 + i * 43, 24);
          if(_inName[i] == IDX_SLOW1) 
          {
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[Model.slow1Src])), sizeof(txtBuff));
            display.print(txtBuff);
            display.drawBitmap(display.getCursorX(), display.getCursorY(), asterisk_small, 3, 3, 1);
          }
          else
          {
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_inName[i]])), sizeof(txtBuff));
            display.print(txtBuff);
          }
        }
        
        display.setCursor(0, 32);
        display.print(F("Weight:  "));
        display.print(Model.mixIn1Weight[thisMixNum]);
        display.print(F("%"));
        display.setCursor(97, 32);
        display.print(Model.mixIn2Weight[thisMixNum]);
        display.print(F("%"));
        
        display.setCursor(0, 40);
        display.print(F("Dfrntl:  "));
        display.print(Model.mixIn1Diff[thisMixNum]);
        display.print(F("%"));
        display.setCursor(97, 40);
        display.print(Model.mixIn2Diff[thisMixNum]);
        display.print(F("%"));
        
        display.setCursor(0, 48);
        display.print(F("Offset:  "));
        display.print(Model.mixIn1Offset[thisMixNum]);
        display.setCursor(97, 48);
        display.print(Model.mixIn2Offset[thisMixNum]);
        
        display.setCursor(0, 56);
        display.print(F("Opertr:  "));
        uint8_t _mixOper = Model.mixOper_N_Switch[thisMixNum] >> 6;
        if(_mixOper == MIX_ADD) 
          display.print(F("Add"));
        else if(_mixOper == MIX_MULTIPLY) 
          display.print(F("Mltply"));
        else if(_mixOper == MIX_REPLACE) 
          display.print(F("RplcW"));
        
        //show mixer switch
        display.setCursor(97, 56);
        uint8_t _idx = Model.mixOper_N_Switch[thisMixNum] & 0x3F;
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mixSwitchStr[_idx])), sizeof(txtBuff));
        display.print(txtBuff);

        changeFocusOnUPDOWN(13);
        toggleEditModeOnSelectClicked();
        if(focusedItem < 8)
          drawCursor(46, focusedItem * 8);
        else if(focusedItem < 13)
          drawCursor(89, (focusedItem * 8) - 40);
        
        //show menu icon
        display.fillRect(120, 0, 8, 7, WHITE);
        if(focusedItem == 13)
          display.drawBitmap(120, 0, menu_icon_focused, 8, 7, 1);
        else
          display.drawBitmap(120, 0, menu_icon, 8, 7, 1);

        //edit values
        if (focusedItem == 1)     //Change to another mixer slot
          thisMixNum = incDecOnUpDown(thisMixNum, 0, NUM_MIXSLOTS - 1, WRAP, SLOW_CHANGE);
        else if(focusedItem == 2) //change output
          Model.mixOut[thisMixNum] = incDecOnUpDown(Model.mixOut[thisMixNum], IDX_NONE, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 3) //change input 1
          Model.mixIn1[thisMixNum] = incDecOnUpDown(Model.mixIn1[thisMixNum], 0, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 4) //adjust weight 1
          Model.mixIn1Weight[thisMixNum] = incDecOnUpDown(Model.mixIn1Weight[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 5) //adjust differential 1
          Model.mixIn1Diff[thisMixNum] = incDecOnUpDown(Model.mixIn1Diff[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 6) //adjust offset 1
          Model.mixIn1Offset[thisMixNum] = incDecOnUpDown(Model.mixIn1Offset[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 7) //change operator
        {
          uint8_t _mixOper = Model.mixOper_N_Switch[thisMixNum] >> 6;
          _mixOper = incDecOnUpDown(_mixOper, 0, NUM_MIXOPERATORS - 1, WRAP, PRESSED_ONLY);
          Model.mixOper_N_Switch[thisMixNum] &= ~0xC0; //clear bits 7 and 6
          Model.mixOper_N_Switch[thisMixNum] |= _mixOper << 6;
        }
        else if(focusedItem == 8) //change input 2
          Model.mixIn2[thisMixNum] = incDecOnUpDown(Model.mixIn2[thisMixNum], 0, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 9) //adjust weight 2
          Model.mixIn2Weight[thisMixNum] = incDecOnUpDown(Model.mixIn2Weight[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 10) //adjust differential 2
          Model.mixIn2Diff[thisMixNum] = incDecOnUpDown(Model.mixIn2Diff[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 11) //adjust offset 2
          Model.mixIn2Offset[thisMixNum] = incDecOnUpDown(Model.mixIn2Offset[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 12) //change switch
        {
          uint8_t _mixSw = Model.mixOper_N_Switch[thisMixNum] & 0x3F;
          _mixSw = incDecOnUpDown(_mixSw, 0, NUM_MIXSWITCHES - 1, NOWRAP, SLOW_CHANGE);
          Model.mixOper_N_Switch[thisMixNum] &= ~0x3F; //clear bits 5 to 0
          Model.mixOper_N_Switch[thisMixNum] |= _mixSw;
        }
        
        //open context menu
        if(focusedItem == 13 && clickedButton == SELECT_KEY)
          changeToScreen(POPUP_MIXER_MENU);
        
        //go back to main menu
        if (heldButton == SELECT_KEY)
        {
          eeSaveModelData(Sys.activeModel);
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case POPUP_MIXER_MENU:
      {
        changeFocusOnUPDOWN(NUM_ITEMS_MIXER_POPUP);
        drawPopupMenu(mixerMenu, NUM_ITEMS_MIXER_POPUP);
        uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
        
        if(_selection == 1) //view outputs
        {
          changeToScreen(MODE_MIXER_OUTPUT);
        }
        else if(_selection == 2) //reset this mix
        {
          setDefaultModelMixerParams(thisMixNum);
          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 3) //move mix
        {
          destMixNum = thisMixNum;
          changeToScreen(POPUP_MOVE_MIX);
          isEditMode = true; //start in edit mode
        }  
        else if(_selection == 4) //copy mix
        {
          destMixNum = thisMixNum;
          changeToScreen(POPUP_COPY_MIX);
          isEditMode = true; //start in edit mode
        }
        else if(_selection == 5) //reset all mixes
        {
          changeToScreen(CONFIRMATION_MIXES_RESET);
        }
        else if(heldButton == SELECT_KEY) //exit
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case CONFIRMATION_MIXES_RESET:
      {
        drawFullScreenMsg(PSTR("Reset all mixes?\n\nYes [Up]  \nNo  [Down]"));
        if(clickedButton == UP_KEY)
        {
          setDefaultModelMixerParams();
          thisMixNum = 0;
          destMixNum = 0;

          makeToast(F("All mixes reset"), 2500, 0);
          changeToScreen(MODE_MIXER);
        }
        else if(clickedButton == DOWN_KEY || heldButton == SELECT_KEY)
        {
          changeToScreen(MODE_MIXER);
        }
      }
      break;
      
    case MODE_MIXER_OUTPUT:
      {
        drawHeader(PSTR("Mixer output"));
        
        display.setCursor(0,56);
        display.print(F("Ch"));
        
        // Graph mixer outputs
        for (uint8_t i = 0; i < NUM_PRP_CHANNLES && i < 9; i++)
        {
          int _outVal = mixerChOutGraphVals[i] / 5;
          uint8_t _xOffset = i * 12;
          if (_outVal > 0)
            display.fillRect(17 + _xOffset, 33 - _outVal, 3, _outVal , BLACK);
          else if (_outVal < 0)
          {
            _outVal = -_outVal;
            display.fillRect(17 + _xOffset, 34, 3, _outVal, BLACK);
          }
          //draw dotted lines
          for (uint8_t j = 1; j <= 39; j += 1)
            display.drawPixel(18 + _xOffset, 13 + j, j % 2);
          //draw midpoint
          display.drawHLine(13, 33, 107, BLACK);
          //Show channel numbers
          display.setCursor(16 + _xOffset, 56);
          display.print(i + 1);
        }

        if(heldButton == SELECT_KEY)
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case POPUP_MOVE_MIX:
      {
        display.drawRect(15, 11, 97, 40, BLACK);
        
        display.setCursor(19,14);
        display.print(F("Move mix#"));
        display.print(thisMixNum + 1); 
        display.setCursor(37,23);
        display.print(F("to:  #"));
        display.print(destMixNum + 1);
        
        drawCursor(59, 23);
        
        destMixNum = incDecOnUpDown(destMixNum, 0, NUM_MIXSLOTS - 1, WRAP, SLOW_CHANGE);
        
        if(clickedButton == SELECT_KEY)
        {
          uint8_t oldPostn = thisMixNum;
          uint8_t newPostn = destMixNum;
          
          //store temporarily the old position's data
          uint8_t _mix1in     =   Model.mixIn1[oldPostn];
          int8_t _mix1weight  =   Model.mixIn1Weight[oldPostn];
          int8_t _mix1offset1 =   Model.mixIn1Offset[oldPostn];
          int8_t _mix1diff    =   Model.mixIn1Diff[oldPostn];
          uint8_t _mix2in     =   Model.mixIn2[oldPostn];
          int8_t _mix2weight  =   Model.mixIn2Weight[oldPostn];
          int8_t _mix2offset1 =   Model.mixIn2Offset[oldPostn];
          int8_t _mix2diff    =   Model.mixIn2Diff[oldPostn];
          uint8_t _mixOperNsw =   Model.mixOper_N_Switch[oldPostn];
          uint8_t _mixout     =   Model.mixOut[oldPostn];
          
          //shift elements of the arrays
          uint8_t thisPostn = oldPostn;
          if(newPostn < oldPostn)
          {
            while(thisPostn > newPostn)
            {
              Model.mixOut[thisPostn]       = Model.mixOut[thisPostn-1];
              Model.mixIn1[thisPostn]       = Model.mixIn1[thisPostn-1];
              Model.mixIn1Weight[thisPostn] = Model.mixIn1Weight[thisPostn-1];
              Model.mixIn1Offset[thisPostn] = Model.mixIn1Offset[thisPostn-1];
              Model.mixIn1Diff[thisPostn]   = Model.mixIn1Diff[thisPostn-1];
              Model.mixOper_N_Switch[thisPostn]  = Model.mixOper_N_Switch[thisPostn-1];
              Model.mixIn2[thisPostn]       = Model.mixIn2[thisPostn-1];
              Model.mixIn2Weight[thisPostn] = Model.mixIn2Weight[thisPostn-1];
              Model.mixIn2Offset[thisPostn] = Model.mixIn2Offset[thisPostn-1];
              Model.mixIn2Diff[thisPostn]   = Model.mixIn2Diff[thisPostn-1];
              
              thisPostn--;
            }
          }
          else if(newPostn > oldPostn) 
          {
            while(thisPostn < newPostn)
            {
              Model.mixOut[thisPostn]       = Model.mixOut[thisPostn+1];
              Model.mixIn1[thisPostn]       = Model.mixIn1[thisPostn+1];
              Model.mixIn1Weight[thisPostn] = Model.mixIn1Weight[thisPostn+1];
              Model.mixIn1Offset[thisPostn] = Model.mixIn1Offset[thisPostn+1];
              Model.mixIn1Diff[thisPostn]   = Model.mixIn1Diff[thisPostn+1];
              Model.mixOper_N_Switch[thisPostn]  = Model.mixOper_N_Switch[thisPostn+1];
              Model.mixIn2[thisPostn]       = Model.mixIn2[thisPostn+1];
              Model.mixIn2Weight[thisPostn] = Model.mixIn2Weight[thisPostn+1];
              Model.mixIn2Offset[thisPostn] = Model.mixIn2Offset[thisPostn+1];
              Model.mixIn2Diff[thisPostn]   = Model.mixIn2Diff[thisPostn+1];
              
              thisPostn++;
            }
          }
          
          //copy from temporary into new position
          Model.mixOut[newPostn]       = _mixout;     
          Model.mixIn1[newPostn]       = _mix1in;    
          Model.mixIn1Weight[newPostn] = _mix1weight;
          Model.mixIn1Offset[newPostn] = _mix1offset1;
          Model.mixIn1Diff[newPostn]   = _mix1diff;  
          Model.mixOper_N_Switch[newPostn]  = _mixOperNsw;   
          Model.mixIn2[newPostn]       = _mix2in;
          Model.mixIn2Weight[newPostn] = _mix2weight; 
          Model.mixIn2Offset[newPostn] = _mix2offset1;
          Model.mixIn2Diff[newPostn]   = _mix2diff;  

          thisMixNum = destMixNum;
          changeToScreen(MODE_MIXER);
        }

        if(heldButton == SELECT_KEY) 
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case POPUP_COPY_MIX:
      {
        display.drawRect(15, 11, 97, 40, BLACK);
        
        display.setCursor(19,14);
        display.print(F("Copy mix#"));
        display.print(thisMixNum + 1); 
        display.setCursor(37,23);
        display.print(F("to:  #"));
        display.print(destMixNum + 1);
        
        drawCursor(59, 23);
        
        destMixNum = incDecOnUpDown(destMixNum, 0, NUM_MIXSLOTS - 1, WRAP, SLOW_CHANGE);
        
        if(clickedButton == SELECT_KEY)
        {
          Model.mixOut[destMixNum]       = Model.mixOut[thisMixNum];
          Model.mixIn1[destMixNum]       = Model.mixIn1[thisMixNum];
          Model.mixIn1Weight[destMixNum] = Model.mixIn1Weight[thisMixNum];
          Model.mixIn1Offset[destMixNum] = Model.mixIn1Offset[thisMixNum];
          Model.mixIn1Diff[destMixNum]   = Model.mixIn1Diff[thisMixNum];
          Model.mixOper_N_Switch[destMixNum]  = Model.mixOper_N_Switch[thisMixNum];
          Model.mixIn2[destMixNum]       = Model.mixIn2[thisMixNum];
          Model.mixIn2Weight[destMixNum] = Model.mixIn2Weight[thisMixNum];
          Model.mixIn2Offset[destMixNum] = Model.mixIn2Offset[thisMixNum];
          Model.mixIn2Diff[destMixNum]   = Model.mixIn2Diff[thisMixNum];
           
          thisMixNum = destMixNum; 
          changeToScreen(MODE_MIXER); 
        }

        if(heldButton == SELECT_KEY)
          changeToScreen(MODE_MIXER);
      }
      break;

    case MODE_OUTPUTS:
      {
        drawHeader((char *)pgm_read_word(&mainMenu[MODE_OUTPUTS]));

        changeFocusOnUPDOWN(6);
        toggleEditModeOnSelectClicked();
        drawCursor(52, focusedItem * 8);
        
        static uint8_t _selectedChannel = 0; //0 is ch1, 1 is ch2, etc.

        if (focusedItem == 1)
          _selectedChannel = incDecOnUpDown(_selectedChannel, 0, NUM_PRP_CHANNLES - 1, WRAP, SLOW_CHANGE); 
        else if (focusedItem == 2 && isEditMode)
        {
          if(pressedButton == UP_KEY || pressedButton == DOWN_KEY)
            Model.reverse ^= (uint16_t) 1 << _selectedChannel; //toggle bit
        }
        else if (focusedItem == 3)
          Model.subtrim[_selectedChannel] = incDecOnUpDown(Model.subtrim[_selectedChannel], -20, 20, NOWRAP, SLOW_CHANGE);
        else if (focusedItem == 4)
          Model.failsafe[_selectedChannel] = incDecOnUpDown(Model.failsafe[_selectedChannel], -101, 100, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 5)
          Model.endpointL[_selectedChannel] = incDecOnUpDown(Model.endpointL[_selectedChannel], -100, 0, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 6)
          Model.endpointR[_selectedChannel] = incDecOnUpDown(Model.endpointR[_selectedChannel], 0, 100, NOWRAP, PRESSED_OR_HELD);

        //-------Show on lcd---------------
        display.setCursor(0, 8);
        display.print(F("Channel:  "));
        display.print(_selectedChannel + 1);
        
        display.setCursor(0, 16);
        display.print(F("Reverse:  "));
        drawCheckbox(60, 16, (Model.reverse >> _selectedChannel) & 0x01);

        display.setCursor(0, 24);
        display.print(F("Subtrim:  "));
        display.print(Model.subtrim[_selectedChannel]); 
        
        display.setCursor(0, 32);
        display.print(F("Failsaf:  "));
        if(Model.failsafe[_selectedChannel]== -101)
          display.print(F("Off"));
        else
          display.print(Model.failsafe[_selectedChannel]);

        display.setCursor(0, 40);
        display.print(F("Endpt L:  "));
        display.print(Model.endpointL[_selectedChannel]);
        
        display.setCursor(0, 48);
        display.print(F("Endpt R:  "));
        display.print(Model.endpointR[_selectedChannel]);
        
        //----show the current channel output value (right align)
        int16_t outVal = channelOut[_selectedChannel] / 5;
        uint8_t _txtWidthPix = 0;
        int16_t _val = outVal;
        do 
        {
          _txtWidthPix += 6;
          _val /= 10;
        } while(_val != 0);
        
        if(outVal < 0) 
          _txtWidthPix += 6;
        
        display.drawRect(123 - _txtWidthPix, 6, _txtWidthPix + 5, 11, BLACK);
        display.setCursor(126 - _txtWidthPix, 8);
        display.print(outVal);
        
        //Exit
        if (heldButton == SELECT_KEY)
        {
          eeSaveModelData(Sys.activeModel);
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case MODE_SYSTEM:
      {
        drawHeader((char *)pgm_read_word(&mainMenu[MODE_SYSTEM]));

        display.setCursor(0, 10);
        display.print(F("RF outpt:  "));
        drawCheckbox(66, 10, Sys.rfOutputEnabled);
        
        display.setCursor(0, 19);
        display.print(F("RF power:  "));
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(rfPowerStr[Sys.rfPower])), sizeof(txtBuff));
        display.print(txtBuff);

        display.setCursor(0, 28);
        display.print(F("Backlght:  "));
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(backlightModeStr[Sys.backlightMode])), sizeof(txtBuff));
        display.print(txtBuff);
        
        display.setCursor(0, 37);
        display.print(F("Sounds  :  "));
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(soundModeStr[Sys.soundMode])), sizeof(txtBuff));
        display.print(txtBuff);
        
        display.setCursor(0, 46);
        display.print(F("Inactvty:  "));
        if(Sys.inactivityMinutes == 0)
          display.print(F("Off"));
        else
        {
          display.print(Sys.inactivityMinutes);
          display.print(F("min"));
        }

        display.setCursor(0, 55);
        display.print(F("Receiver:  [Bind]"));
        
        changeFocusOnUPDOWN(6);
        toggleEditModeOnSelectClicked();
        drawCursor(58, 10 + (focusedItem - 1) * 9);
        
        //edit values
        if (focusedItem == 1)
          Sys.rfOutputEnabled = incDecOnUpDown(Sys.rfOutputEnabled, 0, 1, WRAP, PRESSED_ONLY);
        else if (focusedItem == 2)
          Sys.rfPower = incDecOnUpDown(Sys.rfPower, 0, RFPOWER_LAST, NOWRAP, PRESSED_ONLY);
        else if (focusedItem == 3)
          Sys.backlightMode = incDecOnUpDown(Sys.backlightMode, 0, BACKLIGHT_LAST, NOWRAP, PRESSED_ONLY);
        else if (focusedItem == 4)
          Sys.soundMode = incDecOnUpDown(Sys.soundMode, 0, SOUND_LAST, NOWRAP, PRESSED_ONLY);
        else if (focusedItem == 5)
          Sys.inactivityMinutes = incDecOnUpDown(Sys.inactivityMinutes, 0, 20, NOWRAP, SLOW_CHANGE);
        else if (focusedItem == 6 && isEditMode)
        {
          isRequestingBind = true;
          makeToast(F("Sending bind"), 4000, 0);
          eeSaveSysConfig();
          changeToScreen(HOME_SCREEN);
        }
        
        if (heldButton == SELECT_KEY)
        {
          eeSaveSysConfig();
          changeToScreen(MAIN_MENU);
        }
      }
      break;

    case MODE_TELEMETRY:
      {
        drawHeader((char *)pgm_read_word(&mainMenu[MODE_TELEMETRY]));
        
        display.setCursor(8, 9);
        strlcpy_P(txtBuff, PSTR("Ext volts"), sizeof(txtBuff));
        display.print(txtBuff);
        display.drawHLine(8, 17, strlen(txtBuff) * 6, BLACK);
        
        display.setCursor(14, 19);
        display.print(F("Alarm :  "));
        drawCheckbox(68, 19, Sys.telemAlarmEnabled);
        
        display.setCursor(14, 28);
        display.print(F("HmScrn:  "));
        drawCheckbox(68, 28, Sys.telemVoltsOnHomeScreen);
        
        display.setCursor(14, 37);
        display.print(F("V low :  "));
        printVolts(Model.telemVoltsThresh * 10);

        //Show the telemetry voltage
        if(telem_volts != 0x0FFF)
        {
          display.drawRect(89, 9, 39, 11, BLACK);
          drawTelemVolts(91, 11);
        }

        changeFocusOnUPDOWN(4);
        toggleEditModeOnSelectClicked();
        if(focusedItem == 1) 
          drawCursor(0, 9);
        else 
          drawCursor(60, 19 + (focusedItem - 2) * 9);
        
        if(focusedItem == 2)
          Sys.telemAlarmEnabled = incDecOnUpDown(Sys.telemAlarmEnabled, 0, 1, WRAP, PRESSED_ONLY);
        else if(focusedItem == 3)
          Sys.telemVoltsOnHomeScreen = incDecOnUpDown(Sys.telemVoltsOnHomeScreen, 0, 1, WRAP, PRESSED_ONLY);
        else if(focusedItem == 4)
          Model.telemVoltsThresh = incDecOnUpDown(Model.telemVoltsThresh, 0, 2500, NOWRAP, FAST_CHANGE);
        
        if (heldButton == SELECT_KEY)
        {
          eeSaveSysConfig();
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case MODE_RECEIVER:
      {
        enum {_QUERYING_CONFIG, _SENDING_CONFIG, _VIEWING_CONFIG};
        static uint8_t _state = _QUERYING_CONFIG;
        
        static bool _stateInitialised = false;
        static uint32_t _entryTime = 0;
        static bool _actionStarted = false;
        
        if(!_stateInitialised)
        {
          _state = _QUERYING_CONFIG;
          _entryTime = millis();
          gotOutputChConfig = false;
          _stateInitialised = true;
        }
        
        if(_state != _VIEWING_CONFIG)
        {
          drawLoadingAnimation(34, 28, 2);
          display.setCursor(46, 28);
          
          if(_state == _QUERYING_CONFIG)
          {
            display.print(F("Reading"));
            if(!_actionStarted)
            {
              isRequestingOutputChConfig = true;
              _actionStarted = true;
            }
            if(gotOutputChConfig)
            {
              _actionStarted = false;
              _state = _VIEWING_CONFIG;
            }
          }
          else if(_state == _SENDING_CONFIG)
          {
            display.print(F("Saving"));
            if(!_actionStarted)
            {
              sendOutputChConfig = true;
              _actionStarted = true;
            }
            if(receiverConfigStatusCode > 0)
            {
              if(receiverConfigStatusCode == 1)
                makeToast(F("Saved. Reboot rcvr"), 3000, 0);
              else if(receiverConfigStatusCode == 2)
                makeToast(F("Error saving"), 2500, 0);
              
              _stateInitialised = false;
              _actionStarted = false;
              changeToScreen(HOME_SCREEN);
            }
          }
          
          //Time out
          if(millis() - _entryTime > 2000)
          {
            makeToast(F("No response"), 2500, 0);
            _stateInitialised = false;
            _actionStarted = false;
            changeToScreen(HOME_SCREEN);
          }
        }

        else if(_state == _VIEWING_CONFIG)
        {
          drawHeader((char *)pgm_read_word(&mainMenu[MODE_RECEIVER]));
          
          for(uint8_t i = 0; i < NUM_PRP_CHANNLES && i < 9; ++i)
          {
            if(i < 5)
              display.setCursor(0, 9 + i * 9);
            else
              display.setCursor(66, 9 + (i - 5) * 9);
            
            display.print(F("Ch"));
            display.print(i + 1);
            display.print(F(":  "));
            if(outputChConfig[i] == 0) 
              display.print(F("Dgtl"));
            else if(outputChConfig[i] == 1)
              display.print(F("Srvo")); 
            else if(outputChConfig[i] == 2) 
              display.print(F("PWM")); 
          }
          
          display.setCursor(93, 56);
          display.print(F("[Save]"));
          
          //Handle navigation
          
          changeFocusOnUPDOWN(10);
          toggleEditModeOnSelectClicked();
          if(focusedItem <= 5) drawCursor(28, focusedItem * 9);
          else if(focusedItem < 10)  drawCursor(94, (focusedItem - 5) * 9);
          else if(focusedItem == 10) drawCursor(85, 56);
          
          if(focusedItem < 10)
          {
            uint8_t _idx = focusedItem - 1;
            outputChConfig[_idx] = incDecOnUpDown(outputChConfig[_idx], 0, maxOutputChConfig[_idx], WRAP, SLOW_CHANGE);
          }
          else if(focusedItem == 10 && clickedButton == SELECT_KEY)
          {
            _state = _SENDING_CONFIG;
            _entryTime = millis();
            gotOutputChConfig = false;
            _actionStarted = false;
          }
          
          if(heldButton == SELECT_KEY) //exit. No changes will be saved
          {
            _stateInitialised = false;
            _actionStarted = false;
            changeToScreen(MAIN_MENU);
          }
        }

      }
      break;

    case MODE_CALIB:
      {
        drawHeader(PSTR("Calibration"));

        isCalibratingSticks = true;
        
        enum {STICKS_MOVE = 0, STICKS_DEADZONE};
        static uint8_t calibStage = STICKS_MOVE;
        static bool calibInitialised = false;
        
        if(!calibInitialised)
        {
          //set values to lowest
          Sys.rollMin = 1023;  Sys.rollMax = 0;
          Sys.pitchMin = 1023; Sys.pitchMax = 0;
          Sys.yawMin = 1023;   Sys.yawMax = 0;
          Sys.thrtlMin = 1023; Sys.thrtlMax = 0;

          calibInitialised = true;
        }

        if (calibStage == STICKS_MOVE) 
        {
          drawFullScreenMsg(PSTR("Move sticks fully,\nthen center.\n\n[OK] when done"));

          //---- get min, max, center
          //roll
          int _reading = analogRead(PIN_ROLL);
          Sys.rollCenterVal  = _reading;
          if (_reading < Sys.rollMin)
            Sys.rollMin = _reading;
          else if (_reading > Sys.rollMax)
            Sys.rollMax = _reading;
          //yaw
          _reading = analogRead(PIN_YAW);
          Sys.yawCenterVal = _reading;
          if (_reading < Sys.yawMin)
            Sys.yawMin = _reading;
          else if (_reading > Sys.yawMax)
            Sys.yawMax = _reading;
          //pitch
          _reading  = analogRead(PIN_PITCH);
          Sys.pitchCenterVal = _reading;
          if (_reading < Sys.pitchMin)
            Sys.pitchMin = _reading;
          else if (_reading > Sys.pitchMax)
            Sys.pitchMax = _reading;
          //throttle
          _reading = analogRead(PIN_THROTTLE);
          if (_reading < Sys.thrtlMin)
            Sys.thrtlMin = _reading;
          else if (_reading > Sys.thrtlMax)
            Sys.thrtlMax = _reading;
          
          if (clickedButton == SELECT_KEY)
          {
            //Add slight deadband(about 1.5%) at each stick ends to stabilise readings at ends
            //For a range of 0 to 5V, min max are 0.07V and 4.92V
            int ddznQQ = (Sys.rollMax - Sys.rollMin) / 64;
            Sys.rollMax -= ddznQQ;
            Sys.rollMin += ddznQQ;

            ddznQQ = (Sys.pitchMax - Sys.pitchMin) / 64;
            Sys.pitchMax -= ddznQQ;
            Sys.pitchMin += ddznQQ;

            ddznQQ = (Sys.thrtlMax - Sys.thrtlMin) / 64;
            Sys.thrtlMax -= ddznQQ;
            Sys.thrtlMin += ddznQQ;

            ddznQQ = (Sys.yawMax - Sys.yawMin) / 64;
            Sys.yawMax -= ddznQQ;
            Sys.yawMin += ddznQQ;
            
            calibStage = STICKS_DEADZONE;
            isEditMode = true;
          }
        }

        else if (calibStage == STICKS_DEADZONE)
        {
          display.setCursor(21, 21);
          display.print(F("Adjust deadzone"));
          
          drawCursor(51, 34);
          Sys.deadZonePerc = incDecOnUpDown(Sys.deadZonePerc, 0, 15, NOWRAP, PRESSED_OR_HELD);
          
          display.setCursor(59,34);
          display.print(Sys.deadZonePerc);
          display.print(F("%"));

          if (clickedButton == SELECT_KEY) //exit
          {
            isCalibratingSticks = false;
            calibInitialised = false;
            calibStage = STICKS_MOVE;
            eeSaveSysConfig();
            changeToScreen(HOME_SCREEN);
            makeToast(F("Calibrated"), 2500, 0);
          }
        }
      }
      break;
      
    case MODE_ABOUT:
      {
        drawHeader((char *)pgm_read_word(&mainMenu[MODE_ABOUT]));

        //Show battery voltage
        display.setCursor(0,10);
        display.print(F("Battery: "));
        printVolts(battVoltsNow);
        
        //Show uptime
        display.setCursor(0,19);
        display.print(F("Uptime:  "));
        printHHMMSS(millis());
        
        //Show packet rate
        display.setCursor(0, 28);
        display.print(F("PktRate: "));
        display.print(transmitterPacketRate);
        display.print(F(","));
        display.print(receiverPacketRate);
        
        //show version
        display.setCursor(0, 37);
        display.print(F("FW ver:  "));
        display.print(F(_SKETCHVERSION));
        
        //Show author
        display.setCursor(0, 46);
        display.print(F("Devlpr:  buk7456"));

        if (heldButton == SELECT_KEY)
          changeToScreen(MAIN_MENU);
      }
      break;
      
    default:
      changeToScreen(HOME_SCREEN);
  }
  ///-------------end of state machine -------------------------


  ///----------------- TOAST ----------------------------------
  drawToast();
  
  ///----------------- SHOW ON PHYSICAL LCD -------------------
  display.display(); //show on physical lcd
  display.clearDisplay(); //clear graphics buffer
}


///====================================== HELPERS ==================================================

void toggleEditModeOnSelectClicked()
{
  if (clickedButton == SELECT_KEY)
    isEditMode = !isEditMode;
}

//--------------------------------------------------------------------------------------------------

void printHHMMSS(unsigned long _milliSecs)
{
  //Prints the time as mm:ss or hh:mm:ss at the specified screen cordinates
  
  unsigned long hh, mm, ss;
  ss = _milliSecs / 1000;
  hh = ss / 3600;
  ss = ss - hh * 3600;
  mm = ss / 60;
  ss = ss - mm * 60;

  if (hh > 0)
  {
    display.print(hh);
    display.print(F(":"));
  }
  if (mm < 10)
    display.print(F("0"));
  display.print(mm);
  display.print(F(":"));
  if (ss < 10)
    display.print(F("0"));
  display.print(ss);
}

//--------------------------------------------------------------------------------------------------

void printVolts(uint16_t _milliVolts)
{
  uint16_t val = _milliVolts / 10;
  display.print(val / 100);
  display.print(F("."));
  val = val % 100;
  if (val < 10) 
    display.print(F("0"));
  display.print(val);
  display.print(F("V"));
}

//--------------------------------------------------------------------------------------------------

void drawTelemVolts(uint8_t xpos, uint8_t ypos)
{
  if(telem_volts != 0x0FFF) 
  {
    if((telem_volts >= Model.telemVoltsThresh)
       || ((telem_volts < Model.telemVoltsThresh) && (millis() % 1000 < 700)))
    {
      display.setCursor(xpos, ypos);
      printVolts(telem_volts * 10);
    }
  }
}

//--------------------------------------------------------------------------------------------------

int incDecOnUpDown(int _val, int _lowerLimit, int _upperLimit, bool _enableWrap, uint8_t _state)
{
  //Increments/decrements the passed value between the specified limits inclusive. 
  //If wrap is enabled, wraps around when either limit is reached.
  
  if(!isEditMode)
  {
    return _val;
  }

  uint8_t _heldBtn = 0;
  if(_state == PRESSED_OR_HELD || _state == FAST_CHANGE) 
    _heldBtn = heldButton;
  else if(_state == SLOW_CHANGE && thisLoopNum % (125 / fixedLoopTime) == 1) 
    _heldBtn = heldButton;

  //Default -- UP_KEY increments, DOWN_KEY decrements
  uint8_t incrKey = UP_KEY;
  uint8_t decrKey = DOWN_KEY;
  // UP_KEY decrements, DOWN_KEY increments 
  if(_lowerLimit > _upperLimit) 
  {
    //swap lower and upper limits
    int _tmp = _lowerLimit;
    _lowerLimit = _upperLimit;
    _upperLimit = _tmp;
    //swap key actions
    incrKey = DOWN_KEY;
    decrKey = UP_KEY;
  }
   
  int delta = 1;
  if(_heldBtn > 0 && (millis() - buttonStartTime > (LONGPRESSTIME + 1000UL)) && _state != SLOW_CHANGE)
    delta = 2; //speed up increment
  if(_heldBtn > 0 && (millis() - buttonStartTime > (LONGPRESSTIME + 3000UL)) && _state == FAST_CHANGE)
    delta = 10;

  //inc dec
  if (pressedButton == incrKey || _heldBtn == incrKey)
  {
    _val += delta;
    if(_val > _upperLimit)
    {
      if(_enableWrap) _val = _lowerLimit;
      else _val = _upperLimit;
    }
  }
  else if (pressedButton == decrKey || _heldBtn == decrKey)
  {
    _val -= delta;
    if(_val < _lowerLimit)
    {
      if(_enableWrap) _val = _upperLimit;
      else _val = _lowerLimit;
    }
  }

  return _val;
}

//--------------------------------------------------------------------------------------------------

void changeFocusOnUPDOWN(uint8_t _maxItemNo)
{
  if(isEditMode)
    return;
  
  isEditMode = true;
  focusedItem = incDecOnUpDown(focusedItem, _maxItemNo, 1, WRAP, SLOW_CHANGE);
  isEditMode = false;
}

//--------------------------------------------------------------------------------------------------

void changeToScreen(int8_t _theScrn)
{
  theScreen = _theScrn;
  focusedItem = 1;
  heldButton = 0;
  isEditMode = false;
}

//--------------------------------------------------------------------------------------------------

void drawCursor(uint8_t _xpos, uint8_t _ypos)
{
  if(isEditMode) //draw blinking cursor
  {
    if ((millis() - buttonReleaseTime) % 1000 < 500 || buttonCode == UP_KEY || buttonCode == DOWN_KEY)
      display.fillRect(_xpos + 3, _ypos - 1, 2, 9, BLACK);
  }
  else 
    display.drawBitmap(_xpos, _ypos, point, 6, 7, 1); //draw arrow
}

//--------------------------------------------------------------------------------------------------

void drawHeader(const char* str)
{
  strlcpy_P(txtBuff, str, sizeof(txtBuff));
  uint8_t _txtWidthPix = strlen(txtBuff) * 6;
  uint8_t headingX_offset = (display.width() - _txtWidthPix) / 2; //middle align heading
  display.setCursor(headingX_offset, 0);
  display.println(txtBuff);
  display.drawHLine(0, 3, headingX_offset - 1, BLACK);
  uint8_t _xcord = headingX_offset + _txtWidthPix;
  display.drawHLine(_xcord, 3, 128 - _xcord, BLACK);
}

//--------------------------------------------------------------------------------------------------

void resetTimer1()
{
  timer1ElapsedTime = 0;
  timer1LastElapsedTime = 0;
  timer1LastPaused = millis();
}

//--------------------------------------------------------------------------------------------------

void drawFullScreenMsg(const char* str)
{
  uint8_t pos = 0; //position in string
  uint8_t numTextLines = 1;
  //get number of lines
  while (pgm_read_byte(str + pos) != '\0')
  {
    if(pgm_read_byte(str + pos) == '\n')
      numTextLines++;
    pos++;
  }
  pos = 0; //reset

  uint8_t y_offset = (display.height() - numTextLines * 9) / 2; //9 is line pitch
  for(uint8_t line = 1; line <= numTextLines; line++)
  {
    //get number of characters in the line
    uint8_t numChars = 0;
    while(pgm_read_byte(str + pos) != '\n' && pgm_read_byte(str + pos) != '\0')
    {
      numChars++;
      pos++;
    }
    pos -= numChars;
    //center text
    int x_offset = (display.width() - numChars * 6) / 2;
    display.setCursor(x_offset, y_offset);
    //write the characters 
    while(numChars--)
      display.write(pgm_read_byte(str + pos++));
    //advance 
    y_offset += 9; 
    pos++;
  }
}

//--------------------------------------------------------------------------------------------------

void makeToast(const __FlashStringHelper* text, uint16_t _duration, uint16_t _delay)
{
  toastText = text;
  toastStartTime = millis() + _delay;
  toastExpireTime = toastStartTime + _duration;
}

void drawToast()
{
  //animate toast (slide up and down)
  uint32_t _currTime = millis();
  if (_currTime >= toastStartTime && _currTime < toastExpireTime)
  {
    const int transitionDuration = 250; //in milliseconds
    
    uint8_t ypos;
  
    if(_currTime < (toastStartTime + transitionDuration)) //slideup
      ypos = 63 - ((_currTime - toastStartTime) * 13) / transitionDuration;
    else if((_currTime + transitionDuration) > toastExpireTime) //slide down
      ypos = 63 - ((toastExpireTime - _currTime) * 13) / transitionDuration;
    else
      ypos = 50;

    uint8_t _txtWidthPix = 6 * strlen_P((const char*)toastText); //(const char*) casts
    uint8_t x_offset = (display.width() - _txtWidthPix) / 2; //middle align
    display.drawRect(x_offset - 3, ypos, _txtWidthPix + 5, 13, WHITE);
    display.fillRect(x_offset - 2, ypos + 1, _txtWidthPix + 3, 11, BLACK);
    display.setTextColor(WHITE);
    display.setCursor(x_offset, ypos + 3);
    display.println(toastText);
    display.setTextColor(BLACK);
  }
}

//--------------------------------------------------------------------------------------------------

void drawPopupMenu(const char *const list[], int8_t _numItems)
{
  //Calculate y offset for text item 0. Items are center aligned
  uint8_t _yOffsetStr0 = (display.height() - (_numItems * 9)) / 2;  //9 is pitchsize between text lines
  _yOffsetStr0 += 1;
  
  //draw bounding box
  display.drawRect(15, _yOffsetStr0 - 3, 97, _numItems * 9 + 4, BLACK);  
  
  //fill menu
  for (uint8_t i = 0; i < _numItems; i++)
  {
    strlcpy_P(txtBuff, (char *)pgm_read_word(&list[i]), sizeof(txtBuff));
    if(i == (focusedItem - 1))
    {
      display.fillRect(17, (_yOffsetStr0 - 1) + i * 9, 93, 9, BLACK);
      display.setTextColor(WHITE);
    }
    display.setCursor(19, _yOffsetStr0 + i * 9);
    display.print(txtBuff);
    display.setTextColor(BLACK);
  }
}

//--------------------------------------------------------------------------------------------------

void drawCheckbox(uint8_t _xcord, uint8_t _ycord, bool _val)
{
  if (_val)
    display.drawBitmap(_xcord, _ycord, checkbox_checked, 7, 7, 1);
  else
    display.drawBitmap(_xcord, _ycord, checkbox_unchecked, 7, 7, 1);
}

//--------------------------------------------------------------------------------------------------

int8_t adjustTrim(int8_t _lowerLimit, int8_t _upperLimit, int8_t _val)
{
  uint8_t _heldBtn = 0;
  uint8_t _holdDelay = 200;
  if(millis() - buttonStartTime > 1200) //speed up
    _holdDelay = 100;
  if(thisLoopNum % (_holdDelay / fixedLoopTime) == 1) 
    _heldBtn = heldButton;
  
  if((pressedButton == DOWN_KEY || _heldBtn == DOWN_KEY) && _val > _lowerLimit)
  {    
    _val--;
    audioToPlay = AUDIO_TRIM_MOVED;
  }
  else if((pressedButton == UP_KEY || _heldBtn == UP_KEY) && _val < _upperLimit)
  {
    _val++;
    audioToPlay = AUDIO_TRIM_MOVED;
  }
  return _val;
}

//--------------------------------------------------------------------------------------------------

bool modelIsfree(uint8_t _mdlNo)
{
  eeCopyModelName(txtBuff, _mdlNo);
  //if all characters in name are 0xFF, then model slot is free
  for(uint8_t i = 0; i < (sizeof(Model.modelName) - 1); i++)
  {
    if((uint8_t) *(txtBuff + i) != 0xFF)
      return false;
  }
  return true;
}

//--------------------------------------------------------------------------------------------------

void printModelName(char* _buff, uint8_t _lenBuff, uint8_t _mdlNo)
{
  uint8_t _nameLen = sizeof(Model.modelName) - 1;
  bool isDefaultName = true;
  for(uint8_t i = 0; i < _nameLen && i < _lenBuff - 1; i++)
  {
    if(*(_buff + i) != ' ') //check if it isn't a space character
    {
      isDefaultName = false;
      break;
    }
  }
  if(isDefaultName)
  {
    display.print(F("MODEL"));
    display.print(_mdlNo);
  }
  else
    display.print(_buff);
}

//--------------------------------------------------------------------------------------------------

void drawLoadingAnimation(uint8_t xpos, uint8_t ypos, uint8_t _size)
{
  //active cells on a 4x4 grid. Each new line here is a frame
  static const uint8_t activeCells[] PROGMEM = {
    1, 2, 7,
    2, 7, 11, 
    7, 11,14,
    11,14,13,
    14,13,8,
    13,8, 4,
    8, 4, 1,
    4, 1, 2,
  };
  
  const int frameTime = 50;
  uint8_t frameIdx = (millis() % (8 * frameTime)) / frameTime; //total of 8 frames

  for(uint8_t j = 0; j < 3; j++)
  {
    uint8_t _cell = pgm_read_byte(&activeCells[(frameIdx * 3) + j]);
    uint8_t _x = xpos + _size * (_cell % 4);
    uint8_t _y = ypos + _size * (_cell / 4);
    display.fillRect(_x, _y, _size, _size, BLACK);
  }
}
