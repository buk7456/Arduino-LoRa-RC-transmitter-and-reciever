
void HandleMainUI(); 
void HandleStartupMenu(); 
void FullScreenMsg(const char* str);
//---helpers

void toggleEditModeOnSelectClicked();
void drawAndNavMenu(const char *const list[], int8_t _numMenuItems);
void changeToScreen(int8_t _theScrn);
void resetTimer1();
void drawHeader();
void printVolts(uint16_t _milliVolts, uint8_t _prec);
void printHHMMSS(uint32_t _milliSecs, int _cursorX, int _cursorY);
void changeFocusOnUPDOWN(uint8_t _maxItemNo);
void drawCursor(int16_t _xpos, int16_t _ypos);
void makeToast(const __FlashStringHelper* text, uint16_t _duration, uint16_t _delay);
void drawToast();
void drawPopupMenu(const char *const list[], int8_t _numItems);
void drawCheckbox(int16_t _xcord, int16_t _ycord, bool _val);
bool isDefaultModelName(char* _nameBuff, uint8_t _lenBuff);
int8_t adjustTrim(int8_t _lowerLimit, int8_t _upperLimit, int8_t _val);

enum {WRAP = true, NOWRAP = false};
enum {PRESSED_ONLY = 0, PRESSED_OR_HELD = 1, SLOW_CHANGE = 2, FAST_CHANGE}; 
int incDecOnUpDown(int _val, int _lowerLimit, int _upperLimit, bool _enableWrap, uint8_t _state);

bool hasEnoughSlots(uint8_t _startIdx, uint8_t _numRequired);
void loadMix(uint8_t _mixNo, 
             uint8_t _in1, int8_t _weight1, int8_t _diff1, int8_t _offset1,
             uint8_t _in2, int8_t _weight2, int8_t _diff2, int8_t _offset2, 
             uint8_t _operator, uint8_t _sw, uint8_t _out);

//==================================================================================================

//-- Startup menu strings. Max 15 characters per string
#define NUM_ITEMS_STARTUP_MENU 3
char const startupStr0[] PROGMEM = "Calibrte sticks"; 
char const startupStr1[] PROGMEM = "Format EEPROM"; 
char const startupStr2[] PROGMEM = "Cancel";
const char* const startupMenu[] PROGMEM = { //table to refer to the strings
  startupStr0, startupStr1, startupStr2
};

//-- Main menu strings. Max 16 characters per string
#define NUM_ITEMS_MAIN_MENU 8
char const main0[] PROGMEM = "Main menu"; //heading
char const main1[] PROGMEM = "Model";
char const main2[] PROGMEM = "Inputs";
char const main3[] PROGMEM = "Mixer";
char const main4[] PROGMEM = "Outputs";
char const main5[] PROGMEM = "System";
char const main6[] PROGMEM = "Receiver";
char const main7[] PROGMEM = "About";
const char* const mainMenu[] PROGMEM = { //table to refer to the strings
  main0, main1, main2, main3, main4, main5, main6, main7
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
  MODE_RECEIVER,
  MODE_ABOUT,
  
  //others
  HOME_SCREEN,
  POPUP_TIMER_MENU,
  MODE_TIMER_SETUP,
  POPUP_RENAME_MODEL,
  MODE_MIXER_OUTPUT,
  POPUP_MIXER_MENU,
  POPUP_MOVE_MIX,
  POPUP_COPY_MIX,
  POPUP_TEMPLATES_MENU,
  MODE_CALIB,
  MODE_CHANNEL_MONITOR,
  
  CONFIRMATION_MODEL_COPY,
  CONFIRMATION_MODEL_RESET,
  CONFIRMATION_MIXES_RESET
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
#define NUM_ITEMS_MIXER_POPUP 6
char const mxrStr0[] PROGMEM = "View mixes"; 
char const mxrStr1[] PROGMEM = "Reset mix"; 
char const mxrStr2[] PROGMEM = "Move mix to";
char const mxrStr3[] PROGMEM = "Copy mix to";
char const mxrStr4[] PROGMEM = "Reset all mixes";
char const mxrStr5[] PROGMEM = "Templates";
const char* const mixerMenu[] PROGMEM = { //table to refer to the strings
  mxrStr0, mxrStr1, mxrStr2, mxrStr3, mxrStr4, mxrStr5
};

#define NUM_ITEMS_TEMPLATES_POPUP 3
char const tmpltStr0[] PROGMEM = "Elevon"; 
char const tmpltStr1[] PROGMEM = "Vtail"; 
char const tmpltStr2[] PROGMEM = "Diffr thrust";
const char* const templatesMenu[] PROGMEM = { //table to refer to the strings
  tmpltStr0, tmpltStr1, tmpltStr2
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
char const srcName10[] PROGMEM = "Slow";
char const srcName11[] PROGMEM = "Cv1"; 
char const srcName12[] PROGMEM = "Ail";
char const srcName13[] PROGMEM = "Ele";
char const srcName14[] PROGMEM = "Thrt";
char const srcName15[] PROGMEM = "Rud";
char const srcName16[] PROGMEM = "None";
char const srcName17[] PROGMEM = "Ch1";
char const srcName18[] PROGMEM = "Ch2";
char const srcName19[] PROGMEM = "Ch3";
char const srcName20[] PROGMEM = "Ch4";
char const srcName21[] PROGMEM = "Ch5";
char const srcName22[] PROGMEM = "Ch6";
char const srcName23[] PROGMEM = "Ch7";
char const srcName24[] PROGMEM = "Ch8";
char const srcName25[] PROGMEM = "Virt1";
char const srcName26[] PROGMEM = "Virt2";

const char* const srcNames[] PROGMEM = {
  srcName0, srcName1, srcName2, srcName3, srcName4, srcName5, srcName6, srcName7, 
  srcName8, srcName9, srcName10,srcName11, srcName12, srcName13, srcName14,
  srcName15, srcName16, srcName17, srcName18, srcName19, srcName20, srcName21, 
  srcName22, srcName23, srcName24, srcName25, srcName26
};

//-- MixSwitch string. Last character ^ means upper postn, ~ means mid postn, _ means lower postn 
char const mxSwStr0[]  PROGMEM = "-- ";
char const mxSwStr1[]  PROGMEM = "SwA^";
char const mxSwStr2[]  PROGMEM = "SwA_";
char const mxSwStr3[]  PROGMEM = "SwB^";
char const mxSwStr4[]  PROGMEM = "SwB_";
char const mxSwStr5[]  PROGMEM = "SwC^";
char const mxSwStr6[]  PROGMEM = "SwC~";
char const mxSwStr7[]  PROGMEM = "SwC_";
char const mxSwStr8[]  PROGMEM = "!SwC^";
char const mxSwStr9[]  PROGMEM = "!SwC~";
char const mxSwStr10[]  PROGMEM = "!SwC_";
char const mxSwStr11[] PROGMEM = "SwD^";
char const mxSwStr12[] PROGMEM = "SwD_";
const char* const mixSwitchStr[] PROGMEM = { 
  mxSwStr0, mxSwStr1, mxSwStr2, mxSwStr3, mxSwStr4, mxSwStr5, 
  mxSwStr6, mxSwStr7, mxSwStr8, mxSwStr9, mxSwStr10, mxSwStr11, mxSwStr12
};

//sound mode strings. Max 5 characters
char const soundModeStr0[] PROGMEM = "Off"; 
char const soundModeStr1[] PROGMEM = "Alarm"; 
char const soundModeStr2[] PROGMEM = "NoKey"; 
char const soundModeStr3[] PROGMEM = "All";
const char* const soundModeStr[] PROGMEM = { 
  soundModeStr0, soundModeStr1, soundModeStr2, soundModeStr3
};
  
//Model action strings. Max 9 characters
char const modelActionStr0[] PROGMEM = "Load";
char const modelActionStr1[] PROGMEM = "Copy from";
char const modelActionStr2[] PROGMEM = "Rename";
char const modelActionStr3[] PROGMEM = "Reset";
const char* const modelActionStr[] PROGMEM = { 
  modelActionStr0, modelActionStr1, modelActionStr2, modelActionStr3
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

// ---------------- Globals ------------------

char txtBuff[22]; //generic buffer for working with strings

uint8_t theScreen = HOME_SCREEN;
uint8_t focusedItem = 1; //The item that currently has focus in MODE Screens or popups
bool isEditMode = false;

//Dont use unsigned types for these!!!
int8_t topItem = 1;         //in main menu
int8_t highlightedItem = 1; //in main menu

//Model
enum {LOADMODEL = 0, COPYFROMMODEL, RENAMEMODEL, RESETMODEL};
uint8_t _action_ = LOADMODEL;
uint8_t _thisMdl_;

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
unsigned long battWarnMillisQQ = 0;

//toast
const __FlashStringHelper* toastText;
unsigned long toastExpireTime;
unsigned long toastStartTime;


//================================== Generic messages ==============================================

void FullScreenMsg(const char* str)
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

  int y_offset = (display.height() - numTextLines * 9) / 2; //9 is line pitch
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

///=================================================================================================
///                                Startup menu
///=================================================================================================

void HandleStartupMenu()
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

void HandleMainUI()
{
  ///--------------- INACTIVITY ALARM ---------------------
  if(Sys.inactivityMinutes > 0 && ((millis() - inputsLastMoved) > (Sys.inactivityMinutes * 60000UL)))
  {
    if(thisLoopNum % (15000 / fixedLoopTime) == 1) //repeat every 15 secs
      audioToPlay = AUDIO_INACTIVITY;
  }

  ///--------------- GENERIC STOPWATCH --------------------
  if(stopwatchIsPaused == false) //run
    stopwatchElapsedTime = stopwatchLastElapsedTime + millis() - stopwatchLastPaused;
  else //pause
  {
    stopwatchLastElapsedTime = stopwatchElapsedTime;
    stopwatchLastPaused = millis();
  }
  
  ///---------------- TIMER 1 -----------------------------
  //play sound when timer expires (if count down timer)
  uint32_t _initMillis = Model.Timer1InitMins * 60000UL;
  if (Model.Timer1InitMins > 0 && timer1ElapsedTime > _initMillis)
  {
    if((timer1ElapsedTime - _initMillis) < 500) //only play sound within this timeframe
      audioToPlay = AUDIO_TIMERELAPSED;
  }
  
  /// --------------- TELEMETRY WARN ----------------------
  
  static uint8_t _tCounter = 0;
  static uint32_t _tWarnEntryLoopNum = 0;
  static bool _tWarnStarted = false;
  
  if(Sys.telemAlarmEnabled && receiverPacketRate > 0)
  {
    //check and increment or decrement counter
    if(telem_volts < Sys.Telem_VoltsThresh && telem_volts != 0x0FFF)
    {
      if(_tWarnStarted == false) 
        ++_tCounter;
    }
    else
    {
      if(_tCounter > 0) --_tCounter;
      if(_tCounter == 0) _tWarnStarted = false;
    }
    
    //if more than 4 seconds, trigger alarm
    if(_tCounter > (4000 / fixedLoopTime) && _tWarnStarted == false) 
    {
      _tWarnStarted = true;
      _tWarnEntryLoopNum = thisLoopNum;
    }
    
    if(_tWarnStarted && ((thisLoopNum - _tWarnEntryLoopNum) % (5000 / fixedLoopTime) == 1))
      audioToPlay = AUDIO_TELEMWARN;
  }
  
  if(Sys.rfOutputEnabled == false)
  {
    _tCounter = 0;
    _tWarnStarted = false;
  }
  

  /// --------------- TX LOW BATTERY WARN -----------------
  if(battState == BATTLOW)
  {
    if(battWarnDismissed == false)
    {
      //show warning
      display.clearDisplay();
      FullScreenMsg(PSTR("Battery low"));
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
    if(battWarnDismissed == true && (millis() - battWarnMillisQQ > 600000UL)) 
    {
      battWarnDismissed = false;
      battWarnMillisQQ = millis();
    }
  }
  else
    battWarnMillisQQ = millis();

  /// ---------------- BIND STATUS-----------------------------------
  if(bindStatus == 1)
    makeToast(F("Bind success"), 3000, 0);
  else if(bindStatus == 2)
    makeToast(F("Bind fail"), 3000, 0);
  
  
  ///----------------- MAIN STATE MACHINE ---------------------------
  switch (theScreen)
  {
    case HOME_SCREEN:
      {
        enum {NORMALMODE = 0, DIGCHMODE, TRIMMODE };
        static uint8_t homeScreenMode = NORMALMODE;
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
            display.fillRect(2 + i*3, 2, 2, 3, BLACK);
          lastNumOfBars = numOfBars;
        }

        //---------show dualrate icon --------
        if (SwBEngaged && Model.DualRate > 0)
          display.drawBitmap(66, 1, dualrate_icon, 13, 6, 1);
        
        //--------show rf icon and tx power level ----
        if (Sys.rfOutputEnabled)
        {
          display.drawBitmap(85, 0, rf_icon, 7, 7, 1);
          for(int i = 0; i < Sys.rfPower + 2; i++)
            display.drawVLine(91 + i, 6 - i, i + 1, BLACK);
        }
        
        //--------show mute icon------------
        if (Sys.soundMode == SOUND_OFF)
          display.drawBitmap(41, 0, mute_icon, 7, 7, 1);

        //------show model name-----------
        display.setCursor(20, 16);
        if(isDefaultModelName(Model.modelName, sizeof(Model.modelName)/sizeof(Model.modelName[0])))
        {
          display.print(F("MODEL"));
          display.print(Sys.activeModel);
        }
        else
          display.print(Model.modelName);
        
        // draw separator
        display.drawHLine(20,27,84,BLACK);

        //----show timer 1 ---------
        if(Model.Timer1InitMins == 0) //a countup timer
          printHHMMSS(timer1ElapsedTime, 20, 32);
        else //a count down timer
        {
          uint32_t _initMillis = Model.Timer1InitMins * 60000UL;
          if(timer1ElapsedTime < _initMillis)
          {
            uint32_t ttqq = _initMillis - timer1ElapsedTime;
            printHHMMSS(ttqq + 999, 20, 32); //add 999ms so the displayed time doesnt 
            //change immediately upon running the timer
          }
          else
          {
            uint32_t ttqq = timer1ElapsedTime - _initMillis;
            if(ttqq >= 1000) //prevents displaying -00:00
            { 
              display.setCursor(20, 32);
              display.print(F("-"));
              printHHMMSS(ttqq, 26, 32);
            }
            else
              printHHMMSS(ttqq, 20, 32);
          }
        }

        //------- Show generic timer ------------
        printHHMMSS(stopwatchElapsedTime, 20, 45);

        //---------------------------------------
        
        if(homeScreenMode == DIGCHMODE)
        {
          display.drawBitmap(53, 0, lock_icon, 6, 7, 1); 
          //handle keys
          DigChA = (buttonCode == UP_KEY)? 1 : 0; 
          if(pressedButton == SELECT_KEY) 
            DigChB = ~DigChB & 0x01; //toggle
          //draw
          if(DigChA) display.drawBitmap(113, 1, chA_icon1, 9, 9, 1);
          else display.drawBitmap(113, 1, chA_icon0, 9, 9, 1);
          if(DigChB) display.drawBitmap(113, 29, chB_icon1, 9, 9, 1);
          else display.drawBitmap(113, 29, chB_icon0, 9, 9, 1);
        }
        else if(homeScreenMode == TRIMMODE)
        {
          display.drawBitmap(53, 0, lock_icon, 6, 7, 1); 
          
          if(pressedButton == SELECT_KEY) //prevent possible double beeps
            audioToPlay = AUDIO_NONE;
          //handle keys
          if(clickedButton == SELECT_KEY) 
          {
            selectedTrim++;
            if(selectedTrim > 3)
              selectedTrim = 0;
            audioToPlay = AUDIO_TRIM_AIL + selectedTrim;
          }
          //adjust
          int8_t oldTrimVal = Model.Trim[selectedTrim];
          Model.Trim[selectedTrim] = adjustTrim(-20, 20, Model.Trim[selectedTrim]);
          if(Model.Trim[selectedTrim] != oldTrimVal)
            trimIsPendingSave = true;
          
          //show current trim
          display.setCursor(81, 34);
          strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_AIL + selectedTrim])), sizeof(txtBuff));
          display.print(txtBuff);
          display.setCursor(81, 42);
          display.print(F("trim"));
          display.drawRect(79, 32, 27, 20, BLACK);
          
          //draw sliders
          display.drawHLine(73, 62, 41, BLACK);
          display.drawVLine(126, 17, 41, BLACK);
          display.drawVLine(1, 17, 41, BLACK);
          display.drawHLine(14, 62, 41, BLACK);
          //draw thumbs
          display.drawRect(Model.Trim[0] + 92, 61, 3, 3, BLACK);
          display.drawRect(125, 36 - Model.Trim[1], 3, 3, BLACK);
          display.drawRect(0, 36 - Model.Trim[2], 3, 3, BLACK);
          display.drawRect(Model.Trim[3] + 33, 61, 3, 3, BLACK); 
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
        
        if (homeScreenMode == NORMALMODE && clickedButton == SELECT_KEY)
          changeToScreen(MAIN_MENU);
        else if (homeScreenMode == NORMALMODE && clickedButton == UP_KEY)
          changeToScreen(MODE_CHANNEL_MONITOR);
        else if (homeScreenMode != TRIMMODE && clickedButton == DOWN_KEY)
          changeToScreen(POPUP_TIMER_MENU);
        else if(homeScreenMode != TRIMMODE && heldButton == DOWN_KEY)
        {
          audioToPlay = AUDIO_KEYTONE;
          if(homeScreenMode == NORMALMODE) homeScreenMode = DIGCHMODE;
          else if(homeScreenMode == DIGCHMODE) homeScreenMode = NORMALMODE;
          heldButton = 0;
        }
        else if(homeScreenMode != DIGCHMODE && heldButton == SELECT_KEY)
        {
          audioToPlay = AUDIO_SWITCHMOVED;
          if(homeScreenMode == NORMALMODE) homeScreenMode = TRIMMODE;
          else if(homeScreenMode == TRIMMODE) homeScreenMode = NORMALMODE;
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
        strlcpy_P(txtBuff, PSTR("Timer 1"), sizeof(txtBuff));
        drawHeader();
        
        uint8_t _maxFocusableItems = 1;
        
        display.setCursor(0, 10);
        display.print(F("Control:   "));
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[Model.Timer1ControlSrc])), sizeof(txtBuff));
        display.print(txtBuff);
        
        if(Model.Timer1ControlSrc != IDX_NONE)
        {
          _maxFocusableItems = 4;
          
          display.setCursor(0, 19);
          display.print(F("Operator:  "));
          strlcpy_P(txtBuff, (char *)pgm_read_word(&(tmrOperatorStr[Model.Timer1Operator])), sizeof(txtBuff));
          display.print(txtBuff);
          
          display.setCursor(0, 28);
          display.print(F("Value:     "));
          display.print(Model.Timer1Value);
          
          display.setCursor(0, 37);
          display.print(F("Initial:   "));
          display.print(Model.Timer1InitMins);
          display.print(F(" min"));
        }
        
        changeFocusOnUPDOWN(_maxFocusableItems);
        toggleEditModeOnSelectClicked();
        drawCursor(58, (focusedItem * 9) + 1);
        
        if (focusedItem == 1)
        {
          Model.Timer1ControlSrc = incDecOnUpDown(Model.Timer1ControlSrc, 0, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
          //Validate sources. Allowed sources are raw sticks, knob, switches, channels, virtuals
          while(Model.Timer1ControlSrc == IDX_100PERC 
                || (Model.Timer1ControlSrc >= IDX_SLOW1 && Model.Timer1ControlSrc <= IDX_RUD))
          {
            if(buttonCode == UP_KEY) 
              ++Model.Timer1ControlSrc;
            else if(buttonCode == DOWN_KEY) 
              --Model.Timer1ControlSrc;
          }
        }
        else if(focusedItem == 2)
          Model.Timer1Operator = incDecOnUpDown(Model.Timer1Operator, 0, NUM_TIMER_OPERATORS - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 3)
          Model.Timer1Value = incDecOnUpDown(Model.Timer1Value, -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 4)
          Model.Timer1InitMins = incDecOnUpDown(Model.Timer1InitMins, 0, 240, NOWRAP, PRESSED_OR_HELD);
          
      
        if (heldButton == SELECT_KEY)
        {
          eeSaveModelData(Sys.activeModel);
          changeToScreen(HOME_SCREEN);
        }
      }
      break;
      
    case MODE_CHANNEL_MONITOR:
      {
        strlcpy_P(txtBuff, PSTR("Outputs"), sizeof(txtBuff));
        drawHeader();
        
        for(int i = 0; i < NUM_PRP_CHANNLES && i < 8; i++)
        {
          if(i < 4)
            display.setCursor(11, 16 + i * 10);
          else
            display.setCursor(71, 16 + (i - 4) * 10);
          
          display.print(F("Ch"));          
          display.print(1 + i);  
          display.print(F(":"));          
          display.print(ChOut[i] / 5);
        }
        
        if(clickedButton == UP_KEY || heldButton == SELECT_KEY)
          changeToScreen(HOME_SCREEN);
      }
      break;

    case MAIN_MENU:
      {
        drawAndNavMenu(mainMenu, NUM_ITEMS_MAIN_MENU);
        if (clickedButton == SELECT_KEY)
          changeToScreen(highlightedItem);
        else if (heldButton == SELECT_KEY)
        {
          // reset menu
          // highlightedItem = 1;
          // topItem = 1;
          
          changeToScreen(HOME_SCREEN);
        }
      }
      break;
      
    case MODE_MODEL:
      {
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_MODEL])), sizeof(txtBuff));
        drawHeader();

        //-- show action
        display.setCursor(49, 12);
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(modelActionStr[_action_])), sizeof(txtBuff));
        display.print(txtBuff);

        //-- show model name
        
        if(_action_ > COPYFROMMODEL)
          _thisMdl_ = Sys.activeModel; //reinit
        
        display.setCursor(49, 22);
        
        eeCopyModelName(_thisMdl_, txtBuff); //copy model name into temporary buffer
        if(isDefaultModelName(txtBuff, sizeof(txtBuff)))
        { 
          display.print(F("MODEL"));
          display.print(_thisMdl_);
        }
        else
          display.print(txtBuff);
        
        //-- show confirmation
        display.setCursor(49, 32);
        display.print(F("Confirm"));


        changeFocusOnUPDOWN(3);
        toggleEditModeOnSelectClicked();
        drawCursor(41, (focusedItem * 10) + 2);
        
        if (focusedItem == 1) 
          _action_ = incDecOnUpDown(_action_, 0, 3, WRAP, SLOW_CHANGE);
        
        else if (focusedItem == 2 && (_action_ == LOADMODEL || _action_ == COPYFROMMODEL))
          _thisMdl_ = incDecOnUpDown(_thisMdl_, 1, numOfModels, WRAP, SLOW_CHANGE);
        
        else if (focusedItem == 3 && isEditMode) //confirm action
        {
          if(_action_ == RENAMEMODEL)
            changeToScreen(POPUP_RENAME_MODEL);
          
          else if(_action_ == LOADMODEL)
          {
            if(_thisMdl_ == Sys.activeModel)
            {
              makeToast(F("Already active"), 2000, 0);
              changeToScreen(MODE_MODEL);
            }
            else
            {
              //Save the active model before changing to another model
              eeSaveModelData(Sys.activeModel);
              //load into ram
              eeReadModelData(_thisMdl_);
              //set as active model
              Sys.activeModel = _thisMdl_; 
              
              //reset other stuff
              resetTimer1();
              Sys.rfOutputEnabled = false;
              eeSaveSysConfig();
              
              makeToast(F("Loaded"), 2000, 0);
              changeToScreen(HOME_SCREEN);
            }
            
            _action_ = LOADMODEL; //reinit
            _thisMdl_ = Sys.activeModel; //reinit
          }
          
          else if(_action_ == COPYFROMMODEL)
          {
            if(_thisMdl_ == Sys.activeModel)
            {
              makeToast(F("Nothing to copy"), 2000, 0);
              changeToScreen(MODE_MODEL);
            }
            else
              changeToScreen(CONFIRMATION_MODEL_COPY);
          }
          
          else if (_action_ == RESETMODEL)
            changeToScreen(CONFIRMATION_MODEL_RESET);
        }

        if (heldButton == SELECT_KEY)
        {
          //reinit
          _action_ = LOADMODEL;
          _thisMdl_ = Sys.activeModel;
          
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case CONFIRMATION_MODEL_COPY:
      {
        FullScreenMsg(PSTR("Model data will\nbe overwritten.\nProceed?\n\nYes [Up]  \nNo  [Down]"));
        if(clickedButton == UP_KEY)
        {
          //temporarily store model name as we shall maintain it 
          strlcpy(txtBuff, Model.modelName, sizeof(txtBuff));
          //load source model into ram
          eeReadModelData(_thisMdl_);
          //restore model name
          strlcpy(Model.modelName, txtBuff, sizeof(Model.modelName));
          //save
          eeSaveModelData(Sys.activeModel);
          
          //reset other stuff
          resetTimer1();
          Sys.rfOutputEnabled = false;
          eeSaveSysConfig();
          
          _action_ = LOADMODEL; //reinit
          _thisMdl_ = Sys.activeModel; //reinit
          
          makeToast(F("Copied"), 2000, 0);
          changeToScreen(HOME_SCREEN);
        }
        else if(clickedButton == DOWN_KEY || heldButton == SELECT_KEY)
        {
          changeToScreen(MODE_MODEL);
        }
      }
      break;
      
    case CONFIRMATION_MODEL_RESET:
      {
        FullScreenMsg(PSTR("Model data will\nbe cleared.\nProceed?\n\nYes [Up]  \nNo  [Down]"));
        if(clickedButton == UP_KEY)
        {
          //set defaults
          setDefaultModelBasicParams();
          setDefaultModelMixerParams();
          setDefaultModelName();
          //save
          eeSaveModelData(Sys.activeModel);
          
          //reset other stuff
          resetTimer1();
          Sys.rfOutputEnabled = false;
          eeSaveSysConfig();
          
          _action_ = LOADMODEL; //reinit
          _thisMdl_ = Sys.activeModel; //reinit
          
          makeToast(F("Data cleared"), 2000, 0);
          changeToScreen(HOME_SCREEN);
        }
        else if(clickedButton == DOWN_KEY || heldButton == SELECT_KEY)
        {
          changeToScreen(MODE_MODEL);
        }
      }
      break;
      
    case POPUP_RENAME_MODEL:
      {
        display.drawRect(15,11,97,40,BLACK); //draw bounding box
        
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
        
        if(thisChar >= 65 && thisChar <= 90) thisChar = 90 - thisChar ;
        else if(thisChar == 32) thisChar = 26;
        else if(thisChar >= 97 && thisChar <= 122) thisChar -= 70;
        else if(thisChar >= 45 && thisChar <= 57) thisChar += 8;
        
        //adjust 
        thisChar = incDecOnUpDown(thisChar, 65, 0, NOWRAP, SLOW_CHANGE);

        //map back
        if(thisChar <= 25) thisChar = 90 - thisChar;
        else if(thisChar == 26) thisChar = 32; 
        else if(thisChar >= 27 && thisChar <= 52) thisChar += 70;
        else if(thisChar >= 53 && thisChar <= 65) thisChar -= 8;

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
          heldButton = 0; //override. prevents false triggers
        }
          
        if(charPos == (sizeof(Model.modelName)/sizeof(Model.modelName[0])) - 1) //done renaming. Exit
        {
          charPos = 0;
          eeSaveModelData(Sys.activeModel);
          changeToScreen(HOME_SCREEN); 
          makeToast(F("Done"), 2000, 0);
        }
      }
      break;

    case MODE_INPUTS:
      {
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_INPUTS])), sizeof(txtBuff));
        drawHeader();

        enum{AIL_CURVE = 0, ELE_CURVE = 1, RUD_CURVE = 2, THR_CURVE, CURVE1, SLOW1, RAW_INPUTS};
        static uint8_t _page = AIL_CURVE;
        
        if (focusedItem == 1)
          _page = incDecOnUpDown(_page, 0, 6, WRAP, SLOW_CHANGE);
          
        ///////////////// RATES AND EXPO ////////////////////////////////////////
        if(_page == AIL_CURVE || _page == ELE_CURVE || _page == RUD_CURVE)  
        {  
          changeFocusOnUPDOWN(4);
          toggleEditModeOnSelectClicked();
          
          int8_t *_rate = &Model.RateNormal[_page];
          int8_t *_expo = &Model.ExpoNormal[_page];
          
          if(SwBEngaged == true && ((Model.DualRate >> _page) & 0x01)) //sport
          {
            _rate = &Model.RateSport[_page];
            _expo = &Model.ExpoSport[_page];
            
            display.drawRect(0, 49, 33, 11, BLACK);
            display.setCursor(2, 51);
            display.print(F("Sport"));
          }

          //Adjust values
          if (focusedItem == 2)
            *_rate = incDecOnUpDown(*_rate, 0, 100, NOWRAP, PRESSED_OR_HELD); 
          else if (focusedItem == 3)
            *_expo = incDecOnUpDown(*_expo, -100, 100, NOWRAP, PRESSED_OR_HELD);
          else if (focusedItem == 4)
          {
            uint8_t _drVal = (Model.DualRate >> _page) & 0x01;
            _drVal = incDecOnUpDown(_drVal, 0, 1, WRAP, PRESSED_ONLY);
            Model.DualRate &= ~(1 << _page); //clear
            Model.DualRate |= (_drVal << _page); //set
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
          uint8_t _drVal = (Model.DualRate >> _page) & 0x01;
          drawCheckbox(42, 40, _drVal);
        
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
    
        //////////////// 5-POINT CURVES ////////////////////////////////////////
        if(_page == THR_CURVE || _page == CURVE1)
        {
          uint8_t _maxFocusableItems = 3;
          uint8_t _curveNameIdx = IDX_THRTL_CURV;
          int8_t *ptr_CurvePts = Model.ThrottlePts;
          int _inptSrc = throttleIn;
          if(_page == CURVE1)
          {
            _maxFocusableItems = 4;
            _curveNameIdx = IDX_CRV1;
            ptr_CurvePts = Model.Curve1Pts;
            _inptSrc = curve1SrcVal;
          }
          
          changeFocusOnUPDOWN(_maxFocusableItems);
          toggleEditModeOnSelectClicked();
          
          static uint8_t _thisPt = 0;
          
          //adjust 
          if(focusedItem == 2)
            _thisPt = incDecOnUpDown(_thisPt, 0, 4, WRAP, SLOW_CHANGE);
          else if(focusedItem == 3)
            *(ptr_CurvePts + _thisPt) = incDecOnUpDown(*(ptr_CurvePts + _thisPt), -100, 100, NOWRAP, PRESSED_OR_HELD);
          else if(focusedItem == 4)
            Model.Curve1Src = incDecOnUpDown(Model.Curve1Src, 0, IDX_KNOB, NOWRAP, SLOW_CHANGE);
          
          //-----draw text
          display.setCursor(8, 11);
          strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_curveNameIdx])), sizeof(txtBuff));
          display.print(txtBuff);
          display.drawHLine(8, 19, strlen(txtBuff) * 6, BLACK);
          
          display.setCursor(0, 22);
          display.print(F("Pt :   "));
          display.write(97 + _thisPt); //a,b,c,d,e
        
          display.setCursor(0, 31);
          display.print(F("Val:   "));
          display.print(*(ptr_CurvePts + _thisPt));
          
          if(_page == CURVE1)
          {
            display.setCursor(0, 40);
            display.print(F("Src:   "));
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[Model.Curve1Src])), sizeof(txtBuff));
            display.print(txtBuff);
          }
          
          //-----draw graph
          //axes
          display.drawVLine(100, 11, 51, BLACK);
          display.drawHLine(74, 36, 52, BLACK);
          
          //Interpolate and draw points. We use x cordinate to estimate corresponding y cordinate
          //Actual plot area is 50x50.
          int xpts[5] = {-500, -250, 0, 250, 500};
          int ypts[5];
          for(int i = 0; i < 5; i++)
            ypts[i] = *(ptr_CurvePts + i) * 5;
          
          for (int xval = -25; xval <= 25; xval++) //50x50 grid so first point is -25
          {
            int yval = linearInterpolate(xpts, ypts, 5, xval * 20) / 20;
            display.drawPixel(100 + xval, 36 - yval, BLACK); //plot points
          }
          
          //trace source
          int yy = linearInterpolate(xpts, ypts, 5, _inptSrc) / 20;
          display.fillRect(99 + (_inptSrc / 20), 35 - yy, 3, 3, BLACK);
         
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
          display.print(Model.Slow1Up / 10);
          display.print(F("."));
          display.print(Model.Slow1Up % 10);
          display.print(F("s"));
          
          display.setCursor(0, 31);
          display.print(F("Down:  "));
          display.print(Model.Slow1Down / 10);
          display.print(F("."));
          display.print(Model.Slow1Down % 10);
          display.print(F("s"));
          
          display.setCursor(0, 40);
          display.print(F("Src:   "));
          strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[Model.Slow1Src])), sizeof(txtBuff));
          display.print(txtBuff);
          
          if(focusedItem == 2)
            Model.Slow1Up = incDecOnUpDown(Model.Slow1Up, 0, 50, NOWRAP, PRESSED_OR_HELD);
          else if(focusedItem == 3)
            Model.Slow1Down = incDecOnUpDown(Model.Slow1Down, 0, 50, NOWRAP, PRESSED_OR_HELD);
          else if(focusedItem == 4)
            Model.Slow1Src = incDecOnUpDown(Model.Slow1Src, IDX_SWA, IDX_SWD, NOWRAP, PRESSED_OR_HELD);
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
          for(int i = 0; i < 5; i++)
          {
            display.setCursor(14, 21 + i*9);
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_ROLL + i])), sizeof(txtBuff));
            display.print(txtBuff);
            display.setCursor(42, 21 + i*9);
            display.print(_stickVal[i]/5);
          }
          //show switches
          uint8_t _swState[4] = {SwAEngaged, SwBEngaged, SwCState, SwDEngaged}; //order as in source names
          for(int i = 0; i < 4; i++)
          {
            int16_t _ycord = 21 + i*9;
            display.setCursor(83, _ycord);
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[IDX_SWA + i])), sizeof(txtBuff));
            display.print(txtBuff);
            if(_swState[i] == 0) 
              display.drawBitmap(105, _ycord, switchUp_icon, 5, 7, BLACK);
            else if(_swState[i] == 1) 
              display.drawBitmap(105, _ycord, switchDown_icon, 5, 7, BLACK);
            else 
              display.drawBitmap(105, _ycord, switchMid_icon, 5, 7, BLACK);
          }
        }
        
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
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_MIXER])), sizeof(txtBuff));
        drawHeader();
        
        display.setCursor(0, 8);
        display.print(F("Mix no:  #"));
        display.print(thisMixNum + 1);
        
        display.setCursor(0, 16);
        display.print(F("Output:  "));
        uint8_t _outNameIndex = Model.MixOut[thisMixNum];
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_outNameIndex])), sizeof(txtBuff));
        display.print(txtBuff);
        
        display.setCursor(0, 24);
        display.print(F("Input:   "));
        uint8_t _inName[2] = {Model.MixIn1[thisMixNum], Model.MixIn2[thisMixNum]};
        for(uint8_t i = 0; i < 2; i++)
        {
          display.setCursor(54 + i*43, 24);
          if(_inName[i] == IDX_SLOW1) 
          {
            strlcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[Model.Slow1Src])), sizeof(txtBuff));
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
        display.print(Model.MixIn1Weight[thisMixNum]);
        display.print(F("%"));
        display.setCursor(97, 32);
        display.print(Model.MixIn2Weight[thisMixNum]);
        display.print(F("%"));
        
        display.setCursor(0, 40);
        display.print(F("Dfrntl:  "));
        display.print(Model.MixIn1Diff[thisMixNum]);
        display.print(F("%"));
        display.setCursor(97, 40);
        display.print(Model.MixIn2Diff[thisMixNum]);
        display.print(F("%"));
        
        display.setCursor(0, 48);
        display.print(F("Offset:  "));
        display.print(Model.MixIn1Offset[thisMixNum]);
        display.setCursor(97, 48);
        display.print(Model.MixIn2Offset[thisMixNum]);
        
        display.setCursor(0, 56);
        display.print(F("Opertr:  "));
        if(Model.MixOperator[thisMixNum] == MIX_ADD) 
          display.print(F("Add"));
        else if(Model.MixOperator[thisMixNum] == MIX_MULTIPLY) 
          display.print(F("Mltply"));
        else if(Model.MixOperator[thisMixNum] == MIX_REPLACE) 
          display.print(F("RplcW"));
        
        //show mixer switch
        display.setCursor(97, 56);
        uint8_t _swNameIndex = Model.MixSwitch[thisMixNum];
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mixSwitchStr[_swNameIndex])), sizeof(txtBuff));
        char _lastChar = txtBuff[strlen(txtBuff) - 1];
        txtBuff[strlen(txtBuff) - 1] = '\0'; //delete the last char
        display.print(txtBuff);
        int16_t _xcord = 97 + strlen(txtBuff)*6;
        if(_lastChar == '^')
          display.drawBitmap(_xcord, 56, switchUp_icon, 5, 7, BLACK);
        else if(_lastChar == '~')
          display.drawBitmap(_xcord, 56, switchMid_icon, 5, 7, BLACK);
        else if(_lastChar == '_')
          display.drawBitmap(_xcord, 56, switchDown_icon, 5, 7, BLACK);


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
          Model.MixOut[thisMixNum] = incDecOnUpDown(Model.MixOut[thisMixNum], IDX_NONE, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 3) //change input 1
          Model.MixIn1[thisMixNum] = incDecOnUpDown(Model.MixIn1[thisMixNum], 0, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 4) //adjust weight 1
          Model.MixIn1Weight[thisMixNum] = incDecOnUpDown(Model.MixIn1Weight[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 5) //adjust differential 1
          Model.MixIn1Diff[thisMixNum] = incDecOnUpDown(Model.MixIn1Diff[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 6) //adjust offset 1
          Model.MixIn1Offset[thisMixNum] = incDecOnUpDown(Model.MixIn1Offset[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 7) //change operator
          Model.MixOperator[thisMixNum] = incDecOnUpDown(Model.MixOperator[thisMixNum], 0, NUM_MIXOPERATORS - 1, WRAP, PRESSED_ONLY);
        else if(focusedItem == 8) //change input 2
          Model.MixIn2[thisMixNum] = incDecOnUpDown(Model.MixIn2[thisMixNum], 0, NUM_MIXSOURCES - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 9) //adjust weight 2
          Model.MixIn2Weight[thisMixNum] = incDecOnUpDown(Model.MixIn2Weight[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 10) //adjust differential 2
          Model.MixIn2Diff[thisMixNum] = incDecOnUpDown(Model.MixIn2Diff[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 11) //adjust offset 2
          Model.MixIn2Offset[thisMixNum] = incDecOnUpDown(Model.MixIn2Offset[thisMixNum], -100, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 12) //change switch
          Model.MixSwitch[thisMixNum] = incDecOnUpDown(Model.MixSwitch[thisMixNum], 0, NUM_MIXSWITCHES - 1, NOWRAP, SLOW_CHANGE);
        
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
        else if(_selection == 6) //mixer templates
        {
          changeToScreen(POPUP_TEMPLATES_MENU);
        }
        else if(heldButton == SELECT_KEY) //exit
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case CONFIRMATION_MIXES_RESET:
      {
        FullScreenMsg(PSTR("All mixes will\nbe reset.\nProceed?\n\nYes [Up]  \nNo  [Down]"));
        if(clickedButton == UP_KEY)
        {
          setDefaultModelMixerParams();
          thisMixNum = 0;
          destMixNum = 0;

          makeToast(F("All mixes reset"), 2000, 0);
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
        strlcpy_P(txtBuff, PSTR("Mixer output"), sizeof(txtBuff));
        drawHeader();
        
        display.setCursor(0,56);
        display.print(F("Ch"));
        
        // Graph mixer outputs
        for (int i = 0; i < NUM_PRP_CHANNLES && i < 8; i++)
        {
          int _outVal = mixerChOutGraphVals[i] / 5;
          int _xOffset = i*13;
          if (_outVal > 0)
            display.fillRect(17 + _xOffset, 33 - _outVal, 3, _outVal , BLACK);
          else if (_outVal < 0)
          {
            _outVal = -_outVal;
            display.fillRect(17 + _xOffset, 34, 3, _outVal, BLACK);
          }
          //draw dotted lines
          for (int j = 1; j <= 39; j += 1)
            display.drawPixel(18 + _xOffset, 13 + j, j % 2);
          //draw midpoint
          display.drawHLine(8, 33, 112, BLACK);
          //Show channel numbers
          display.setCursor(16 + _xOffset,56);
          display.print(i+1);
        }

        if(heldButton == SELECT_KEY || clickedButton == SELECT_KEY)
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case POPUP_MOVE_MIX:
      {
        display.drawRect(15,11,97,40,BLACK); //draw bounding box
        
        display.setCursor(19,14);
        display.print(F("Move mix#"));
        display.print(thisMixNum + 1); 
        display.setCursor(37,23);
        display.print(F("to:  #"));
        display.print(destMixNum + 1);
        
        toggleEditModeOnSelectClicked();
        drawCursor(59, 23);
        
        destMixNum = incDecOnUpDown(destMixNum, 0, NUM_MIXSLOTS - 1, NOWRAP, SLOW_CHANGE);
        
        if(isEditMode == false)
        {
          uint8_t oldPostn = thisMixNum;
          uint8_t newPostn = destMixNum;
          
          //store temporarily the old position's data
          uint8_t _mix1in     =   Model.MixIn1[oldPostn];
          int8_t _mix1weight  =   Model.MixIn1Weight[oldPostn];
          int8_t _mix1offset1 =   Model.MixIn1Offset[oldPostn];
          int8_t _mix1diff    =   Model.MixIn1Diff[oldPostn];
          
          uint8_t _mix2in     =   Model.MixIn2[oldPostn];
          int8_t _mix2weight  =   Model.MixIn2Weight[oldPostn];
          int8_t _mix2offset1 =   Model.MixIn2Offset[oldPostn];
          int8_t _mix2diff    =   Model.MixIn2Diff[oldPostn];
          
          uint8_t _mixOper    =   Model.MixOperator[oldPostn];
          uint8_t _mixSw      =   Model.MixSwitch[oldPostn];
          uint8_t _mixout     =   Model.MixOut[oldPostn];
          
          //shift elements of the arrays
          uint8_t thisPostn = oldPostn;
          if(newPostn < oldPostn)
          {
            while(thisPostn > newPostn)
            {
              Model.MixOut[thisPostn]       = Model.MixOut[thisPostn-1];
              Model.MixIn1[thisPostn]       = Model.MixIn1[thisPostn-1];
              Model.MixIn1Weight[thisPostn] = Model.MixIn1Weight[thisPostn-1];
              Model.MixIn1Offset[thisPostn] = Model.MixIn1Offset[thisPostn-1];
              Model.MixIn1Diff[thisPostn]   = Model.MixIn1Diff[thisPostn-1];
              Model.MixOperator[thisPostn]  = Model.MixOperator[thisPostn-1];
              Model.MixSwitch[thisPostn]    = Model.MixSwitch[thisPostn-1];
              Model.MixIn2[thisPostn]       = Model.MixIn2[thisPostn-1];
              Model.MixIn2Weight[thisPostn] = Model.MixIn2Weight[thisPostn-1];
              Model.MixIn2Offset[thisPostn] = Model.MixIn2Offset[thisPostn-1];
              Model.MixIn2Diff[thisPostn]   = Model.MixIn2Diff[thisPostn-1];
              
              thisPostn--;
            }
          }
          else if(newPostn > oldPostn) 
          {
            while(thisPostn < newPostn)
            {
              Model.MixOut[thisPostn]       = Model.MixOut[thisPostn+1];
              Model.MixIn1[thisPostn]       = Model.MixIn1[thisPostn+1];
              Model.MixIn1Weight[thisPostn] = Model.MixIn1Weight[thisPostn+1];
              Model.MixIn1Offset[thisPostn] = Model.MixIn1Offset[thisPostn+1];
              Model.MixIn1Diff[thisPostn]   = Model.MixIn1Diff[thisPostn+1];
              Model.MixOperator[thisPostn]  = Model.MixOperator[thisPostn+1];
              Model.MixSwitch[thisPostn]    = Model.MixSwitch[thisPostn+1];
              Model.MixIn2[thisPostn]       = Model.MixIn2[thisPostn+1];
              Model.MixIn2Weight[thisPostn] = Model.MixIn2Weight[thisPostn+1];
              Model.MixIn2Offset[thisPostn] = Model.MixIn2Offset[thisPostn+1];
              Model.MixIn2Diff[thisPostn]   = Model.MixIn2Diff[thisPostn+1];
              
              thisPostn++;
            }
          }
          
          //copy from temporary into new position
          Model.MixOut[newPostn]       = _mixout;     
          Model.MixIn1[newPostn]       = _mix1in;    
          Model.MixIn1Weight[newPostn] = _mix1weight;
          Model.MixIn1Offset[newPostn] = _mix1offset1;
          Model.MixIn1Diff[newPostn]   = _mix1diff;  
          Model.MixOperator[newPostn]  = _mixOper;   
          Model.MixSwitch[newPostn]    = _mixSw;
          Model.MixIn2[newPostn]       = _mix2in;
          Model.MixIn2Weight[newPostn] = _mix2weight; 
          Model.MixIn2Offset[newPostn] = _mix2offset1;
          Model.MixIn2Diff[newPostn]   = _mix2diff;  

          thisMixNum = destMixNum; //display the destination mix slot when we go back to mixer screen
          destMixNum = thisMixNum;
          changeToScreen(MODE_MIXER);
        }

        if(heldButton == SELECT_KEY) 
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case POPUP_COPY_MIX:
      {
        display.drawRect(15,11,97,40,BLACK); //draw bounding box
        
        display.setCursor(19,14);
        display.print(F("Copy mix#"));
        display.print(thisMixNum + 1); 
        display.setCursor(37,23);
        display.print(F("to:  #"));
        display.print(destMixNum + 1);
        
        toggleEditModeOnSelectClicked();
        drawCursor(59, 23);
        
        destMixNum = incDecOnUpDown(destMixNum, 0, NUM_MIXSLOTS - 1, NOWRAP, SLOW_CHANGE);
        
        if(isEditMode == false)
        {
          Model.MixOut[destMixNum]       = Model.MixOut[thisMixNum];
          Model.MixIn1[destMixNum]       = Model.MixIn1[thisMixNum];
          Model.MixIn1Weight[destMixNum] = Model.MixIn1Weight[thisMixNum];
          Model.MixIn1Offset[destMixNum] = Model.MixIn1Offset[thisMixNum];
          Model.MixIn1Diff[destMixNum]   = Model.MixIn1Diff[thisMixNum];
          Model.MixOperator[destMixNum]  = Model.MixOperator[thisMixNum];
          Model.MixSwitch[destMixNum]    = Model.MixSwitch[thisMixNum];
          Model.MixIn2[destMixNum]       = Model.MixIn2[thisMixNum];
          Model.MixIn2Weight[destMixNum] = Model.MixIn2Weight[thisMixNum];
          Model.MixIn2Offset[destMixNum] = Model.MixIn2Offset[thisMixNum];
          Model.MixIn2Diff[destMixNum]   = Model.MixIn2Diff[thisMixNum];
           
          thisMixNum = destMixNum; //display the destination when we go back to mixer screen
          destMixNum = thisMixNum;
          changeToScreen(MODE_MIXER); 
        }

        if(heldButton == SELECT_KEY)
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case POPUP_TEMPLATES_MENU:
      {
        changeFocusOnUPDOWN(NUM_ITEMS_TEMPLATES_POPUP);
        drawPopupMenu(templatesMenu, NUM_ITEMS_TEMPLATES_POPUP);
        uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
        
        if(_selection == 1)
        {
          //elevon 
          // Ch1 = -50%Ail + -50%Ele, 
          // Ch2 = 50%Ail + -50%Ele
          if(hasEnoughSlots(thisMixNum, 2))
          {
            loadMix(thisMixNum,     IDX_AIL, -50, 0, 0, IDX_ELE, -50, 0, 0, MIX_ADD, SW_NONE, IDX_CH1);
            loadMix(thisMixNum + 1, IDX_AIL,  50, 0, 0, IDX_ELE, -50, 0, 0, MIX_ADD, SW_NONE, IDX_CH2);
          }
          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 2)
        {
          //vtail  
          // Ch2 = 50%Rud + -50%Ele, 
          // Ch4 = -50%Rud + -50%Ele
          if(hasEnoughSlots(thisMixNum, 2))
          {
            loadMix(thisMixNum,     IDX_RUD,  50, 0, 0, IDX_ELE, -50, 0, 0, MIX_ADD, SW_NONE, IDX_CH2);
            loadMix(thisMixNum + 1, IDX_RUD, -50, 0, 0, IDX_ELE, -50, 0, 0, MIX_ADD, SW_NONE, IDX_CH4);
          }
          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 3) 
        {
          //differential thrust
          // Ch3 = 100%Thrt +  40%yaw when SwD is down
          // Ch7 = 100%Thrt + -40%yaw when SwD is down
          if(hasEnoughSlots(thisMixNum, 2))
          {
            loadMix(thisMixNum,     IDX_THRTL_CURV, 100, 0, 0, IDX_YAW,  40, 0, 0,  MIX_ADD, SWD_DOWN, IDX_CH3);
            loadMix(thisMixNum + 1, IDX_THRTL_CURV, 100, 0, 0, IDX_YAW, -40, 0, 0, MIX_ADD, SWD_DOWN, IDX_CH7);
            makeToast(F("Set failsafe!"), 2500, 750);
          }
          changeToScreen(MODE_MIXER);
        }
        else if(heldButton == SELECT_KEY) //exit
          changeToScreen(MODE_MIXER);
      }
      break;

    case MODE_OUTPUTS:
      {
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_OUTPUTS])), sizeof(txtBuff));
        drawHeader();

        changeFocusOnUPDOWN(6);
        toggleEditModeOnSelectClicked();
        drawCursor(52, focusedItem * 8);
        
        static uint8_t _selectedChannel = 0; //0 is ch1, 1 is ch2, etc.

        if (focusedItem == 1)
          _selectedChannel = incDecOnUpDown(_selectedChannel, 0, NUM_PRP_CHANNLES - 1, WRAP, SLOW_CHANGE); 
        else if (focusedItem == 2)
        {
          uint8_t _reverseVal = (Model.Reverse >> _selectedChannel) & 0x01;
          _reverseVal = incDecOnUpDown(_reverseVal, 0, 1, WRAP, PRESSED_ONLY);
          Model.Reverse &= ~((uint16_t) 1 << _selectedChannel); //clear
          Model.Reverse |= ((uint16_t)_reverseVal << _selectedChannel); //set
        }
        else if (focusedItem == 3)
          Model.Subtrim[_selectedChannel] = incDecOnUpDown(Model.Subtrim[_selectedChannel], -20, 20, NOWRAP, SLOW_CHANGE);
        else if (focusedItem == 4)
          Model.Failsafe[_selectedChannel] = incDecOnUpDown(Model.Failsafe[_selectedChannel], -101, 100, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 5)
          Model.EndpointL[_selectedChannel] = incDecOnUpDown(Model.EndpointL[_selectedChannel], -100, 0, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 6)
          Model.EndpointR[_selectedChannel] = incDecOnUpDown(Model.EndpointR[_selectedChannel], 0, 100, NOWRAP, PRESSED_OR_HELD);

        //-------Show on lcd---------------
        display.setCursor(0, 8);
        display.print(F("Channel:  "));
        display.print(_selectedChannel + 1);
        
        display.setCursor(0, 16);
        display.print(F("Reverse:  "));
        uint8_t _reverseVal = (Model.Reverse >> _selectedChannel) & 0x01;
        drawCheckbox(60, 16, _reverseVal);

        display.setCursor(0, 24);
        display.print(F("Subtrim:  "));
        display.print(Model.Subtrim[_selectedChannel]); 
        
        display.setCursor(0, 32);
        display.print(F("Failsaf:  "));
        if(Model.Failsafe[_selectedChannel]== -101)
          display.print(F("Off"));
        else
          display.print(Model.Failsafe[_selectedChannel]);

        display.setCursor(0, 40);
        display.print(F("Endpt L:  "));
        display.print(Model.EndpointL[_selectedChannel]);
        
        display.setCursor(0, 48);
        display.print(F("Endpt R:  "));
        display.print(Model.EndpointR[_selectedChannel]);
        
        //----show the current channel output value (right align)
        int16_t outVal = ChOut[_selectedChannel] / 5;
        int16_t len = 0;
        if(abs(outVal) < 10) len = 6;
        else if(abs(outVal) < 100) len = 12;
        else len = 18;
        if(outVal < 0) len += 6;
        display.drawRect(123 - len, 6, len + 5, 11, BLACK);
        display.setCursor(126 - len, 8);
        display.print(outVal);


        if (heldButton == SELECT_KEY)
        {
          eeSaveModelData(Sys.activeModel);
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case MODE_SYSTEM:
      {
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_SYSTEM])), sizeof(txtBuff));
        drawHeader();

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
          Sys.inactivityMinutes = incDecOnUpDown(Sys.inactivityMinutes, 0, 30, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 6 && isEditMode)
        {
          bindActivated = true;
          makeToast(F("Sending bind.."), 5000, 0);
          eeSaveSysConfig();
          changeToScreen(HOME_SCREEN);
        }
        
        //go back to main menu
        if (heldButton == SELECT_KEY)
        {
          eeSaveSysConfig();
          changeToScreen(MAIN_MENU);
        }
      }
      break;

    case MODE_RECEIVER:
      {
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_RECEIVER])), sizeof(txtBuff));
        drawHeader();
        
        display.setCursor(8, 9);
        strlcpy_P(txtBuff, PSTR("Ext volts"), sizeof(txtBuff));
        display.print(txtBuff);
        display.drawHLine(8, 17, strlen(txtBuff) * 6, BLACK);
        
        display.setCursor(20, 20);
        display.print(F("Alarm:  "));
        drawCheckbox(68, 20, Sys.telemAlarmEnabled);
        
        display.setCursor(20, 29);
        display.print(F("V low:  "));
        printVolts(Sys.Telem_VoltsThresh * 10, 2);
        
        //Show the voltage
        display.setCursor(68, 40);
        bool _showVal = true;
        if(telem_volts == 0x0FFF)
        {
          display.print(F("No data"));
          _showVal = false;
        }
        else if((telem_volts < Sys.Telem_VoltsThresh) && (millis() % 1000 > 600))
          _showVal = false;
        if(_showVal)
        {
          printVolts(telem_volts * 10, 2);
          display.drawRect(66, 38, 39, 11, BLACK);
        }

        changeFocusOnUPDOWN(3);
        toggleEditModeOnSelectClicked();
        if(focusedItem == 1) drawCursor(0, 9);
        else drawCursor(60, 20 + (focusedItem - 2) * 9);
        
        if(focusedItem == 2)
          Sys.telemAlarmEnabled = incDecOnUpDown(Sys.telemAlarmEnabled, 0, 1, WRAP, PRESSED_ONLY);
        else if(focusedItem == 3)
          Sys.Telem_VoltsThresh = incDecOnUpDown(Sys.Telem_VoltsThresh, 0, 4000, NOWRAP, FAST_CHANGE);

        if (heldButton == SELECT_KEY)
        {
          eeSaveSysConfig();
          changeToScreen(MAIN_MENU);
        }
      }
      break;

    case MODE_CALIB:
      {
        strlcpy_P(txtBuff, PSTR("Calibration"), sizeof(txtBuff));
        drawHeader();

        isCalibratingSticks = true;
        
        enum {STICKS_MOVE = 0, STICKS_DEADZONE};
        static uint8_t calibStage = STICKS_MOVE;
        static bool calibInitialised = false;
        
        if(calibInitialised == false)
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
          display.setCursor(14, 18);
          display.print(F("Move sticks fully,"));
          display.setCursor(14, 26);
          display.print(F("then center."));
          display.setCursor(23, 42);
          display.print(F("[OK] when done"));

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
            makeToast(F("Calibrated"), 2000, 0);
          }
        }
      }
      break;
      
    case MODE_ABOUT:
      {
        strlcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_ABOUT])), sizeof(txtBuff));
        drawHeader();

        //Show battery voltage
        display.setCursor(0,10);
        display.print(F("Battery: "));
        printVolts(battVoltsNow, 2);
        
        //Show uptime
        display.setCursor(0,19);
        display.print(F("Uptime:  "));
        printHHMMSS(millis(), display.getCursorX(), display.getCursorY());
        
        //Show packet rate
        display.setCursor(0, 28);
        display.print(F("PktRate: "));
        display.print(transmitterPacketRate);
        display.print(F("-"));
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

void printHHMMSS(unsigned long _milliSecs, int _cursorX, int _cursorY)
{
  //Prints the time as mm:ss or hh:mm:ss at the specified screen cordinates
  
  unsigned long hh, mm, ss;
  ss = _milliSecs / 1000;
  hh = ss / 3600;
  ss = ss - hh * 3600;
  mm = ss / 60;
  ss = ss - mm * 60;
  display.setCursor(_cursorX, _cursorY);
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

void printVolts(uint16_t _milliVolts, uint8_t _prec)
{
  if(_prec > 2) _prec = 2;
  
  uint16_t val = _milliVolts / 10;
  display.print(val / 100);
  if(_prec > 0)
    display.print(F("."));
  if(_prec == 1)
    display.print((val/10)% 10);
  else if(_prec == 2)
  {
    val = val % 100;
    if (val < 10) 
      display.print(F("0"));
    display.print(val);
  }
  display.print(F("V"));
}

//--------------------------------------------------------------------------------------------------

int incDecOnUpDown(int _val, int _lowerLimit, int _upperLimit, bool _enableWrap, uint8_t _state)
{
  //Increments/decrements the passed value between the specified limits inclusive. 
  //If wrap is enabled, wraps around when either limit is reached.
  
  if(isEditMode == false)
  {
    return _val;
  }

  uint8_t _heldBtn = 0;
  if(_state == PRESSED_OR_HELD || _state == FAST_CHANGE) 
    _heldBtn = heldButton;
  else if(_state == SLOW_CHANGE && thisLoopNum % (100 / fixedLoopTime) == 1) 
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
    delta = 20;

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
  if(isEditMode == true)
    return;
  
  uint8_t _heldBtn = 0;
  if(thisLoopNum % (200 / fixedLoopTime) == 1) 
    _heldBtn = heldButton;
  
  if(pressedButton == UP_KEY || _heldBtn == UP_KEY)
  {
    focusedItem--;
    if(focusedItem == 0)
      focusedItem = _maxItemNo;
  }
  else if(pressedButton == DOWN_KEY || _heldBtn == DOWN_KEY)
  {
    focusedItem++;
    if(focusedItem > _maxItemNo)
      focusedItem = 1;
  }
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

void drawCursor(int16_t _xpos, int16_t _ypos)
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

void drawHeader()
{
  int _txtWidthPix = strlen(txtBuff) * 6;
  int headingX_offset = (display.width() - _txtWidthPix) / 2; //middle align heading
  display.setCursor(headingX_offset, 0);
  display.println(txtBuff);
  display.drawHLine(0, 3, headingX_offset - 1, BLACK);
  int _xcord = headingX_offset + _txtWidthPix;
  display.drawHLine(_xcord, 3, 128 - _xcord, BLACK);
}

//--------------------------------------------------------------------------------------------------

void drawAndNavMenu(const char *const list[], int8_t _numMenuItems)
{
  _numMenuItems -= 1; //exclude menu heading in count
  
  uint8_t _heldBtn = 0;
  if(thisLoopNum % (200 / fixedLoopTime) == 1) _heldBtn = heldButton;
  
  //------handle menu navigation (up and down keys)------
  if (pressedButton == DOWN_KEY || _heldBtn == DOWN_KEY)
  {
    highlightedItem += 1; //highlight next item
    if (highlightedItem > _numMenuItems)
      highlightedItem = 1; //wrap to first item
    if ((highlightedItem - topItem) >= 4 || (highlightedItem - topItem) <= 0)
    {
      //Move one step down. If at bottom of list, wrap to top
      topItem += 1;
      if (topItem > (_numMenuItems - 3))
        topItem = 1;
    }
  }
  else if (pressedButton == UP_KEY || _heldBtn == UP_KEY)
  {
    highlightedItem -= 1; //highlight next item
    if (highlightedItem == 0)
      highlightedItem = _numMenuItems; //wrap to last item
    if ((highlightedItem - topItem) < 0 || (highlightedItem - topItem) >= 4)
    {
      //Move one step up .If at very top of list, wrapup to bottom
      topItem -= 1;
      if (topItem == 0)
      {
        topItem = highlightedItem - 3;
        if (_numMenuItems <= 4)
          topItem = 1;
      }
    }
  }

  //------show heading------
  strlcpy_P(txtBuff, (char *)pgm_read_word(&list[0]), sizeof(txtBuff));
  int _txtWidthPix = strlen(txtBuff) * 6;
  int headingX_offset = (display.width() - _txtWidthPix) / 2; //middle align heading
  display.setCursor(headingX_offset, 2);
  display.println(txtBuff);
  display.drawHLine(0, 11, 128, BLACK);

  //------fill menu slots----
  for (int i = 0; i < 4 && i < _numMenuItems; i++) //4 item slots
  {
    strlcpy_P(txtBuff, (char *)pgm_read_word(&list[topItem + i]), sizeof(txtBuff));
    if (highlightedItem == (topItem + i)) //highlight selection
    {
      display.fillRect(6, 13 + i * 13, 116, 11, BLACK);
      display.setTextColor(WHITE);
    }
    display.setCursor(14, 15 + i*13);
    display.println(txtBuff);
    display.setTextColor(BLACK);
  }

  //------draw a simple scroll bar ----
  const int viewPortHeight = 52; //4*13
  int contentHeight = _numMenuItems * 13;
  int contentY = (topItem - 1) * 13;
  int barSize = (viewPortHeight * viewPortHeight) / contentHeight;
  barSize += 1; //Add 1 to compensate for truncation error
  int barYPostn = (viewPortHeight * contentY) / contentHeight;
  display.drawVLine(125 , 12 + barYPostn, barSize, BLACK);
}

//--------------------------------------------------------------------------------------------------

void resetTimer1()
{
  timer1ElapsedTime = 0;
  timer1LastElapsedTime = 0;
  timer1LastPaused = millis();
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
  uint32_t _currTime = millis();
  if (_currTime >= toastStartTime && _currTime < toastExpireTime)
  {
    int _txtWidthPix = 6 * strlen_P((const char*)toastText); //(const char*) casts
    int x_offset = (display.width() - _txtWidthPix) / 2; //middle align
    display.drawRect(x_offset - 3, 50, _txtWidthPix + 5, 13, WHITE);
    display.fillRect(x_offset - 2, 51, _txtWidthPix + 3, 11, BLACK);
    display.setTextColor(WHITE);
    display.setCursor(x_offset, 53);
    display.println(toastText);
    display.setTextColor(BLACK);
  }
}

//--------------------------------------------------------------------------------------------------

void drawPopupMenu(const char *const list[], int8_t _numItems)
{
  //Calculate y offset for text item 0. Items are center aligned
  int _yOffsetStr0 = (display.height() - (_numItems * 9)) / 2;  //9 is pitchsize between text lines
  _yOffsetStr0 += 1;
  
  //draw bounding box
  display.drawRect(15, _yOffsetStr0 - 3, 97, _numItems*9 + 4, BLACK);  
  
  //fill menu
  for (int i = 0; i < _numItems; i++)
  {
    strlcpy_P(txtBuff, (char *)pgm_read_word(&list[i]), sizeof(txtBuff));
    if(i == (focusedItem - 1))
    {
      display.fillRect(17, (_yOffsetStr0 - 1)+ i*9, 93, 9, BLACK);
      display.setTextColor(WHITE);
    }
    display.setCursor(19, _yOffsetStr0 + i*9);
    display.print(txtBuff);
    display.setTextColor(BLACK);
  }
}

//--------------------------------------------------------------------------------------------------

void drawCheckbox(int16_t _xcord, int16_t _ycord, bool _val)
{
  if (_val == true)
    display.drawBitmap(_xcord, _ycord, checkbox_checked, 7, 7, 1);
  else
    display.drawBitmap(_xcord, _ycord, checkbox_unchecked, 7, 7, 1);
}

//--------------------------------------------------------------------------------------------------

bool isDefaultModelName(char* _nameBuff, uint8_t _lenBuff)
{
  uint8_t _nameLen = sizeof(Model.modelName)/sizeof(Model.modelName[0]) - 1;
  
  for(uint8_t i = 0; i < _nameLen && i < _lenBuff - 1; i++)
  {
    if(*(_nameBuff + i) != ' ') //check if it isn't a space character
      return false;
  }
  return true;
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
    audioToPlay = AUDIO_SWITCHMOVED;
  }
  else if((pressedButton == UP_KEY || _heldBtn == UP_KEY) && _val < _upperLimit)
  {
    _val++;
    audioToPlay = AUDIO_SWITCHMOVED;
  }
  return _val;
}

//--------------------------------------------------------------------------------------------------

bool hasEnoughSlots(uint8_t _startIdx, uint8_t _numRequired)
{
  if((_startIdx + _numRequired) > NUM_MIXSLOTS)
  {
    makeToast(F("Can't load here!"), 2000, 0);
    return false;
  }
  else 
    return true;
}

void loadMix(uint8_t _mixNo, 
             uint8_t _in1, int8_t _weight1, int8_t _diff1, int8_t _offset1,
             uint8_t _in2, int8_t _weight2, int8_t _diff2, int8_t _offset2, 
             uint8_t _operator, uint8_t _sw, uint8_t _out)
{
  if(_mixNo >= NUM_MIXSLOTS)
  {
    return;
  }
  
  Model.MixIn1[_mixNo]        = _in1; 
  Model.MixIn1Weight[_mixNo]  = _weight1;
  Model.MixIn1Diff[_mixNo]    = _diff1;
  Model.MixIn1Offset[_mixNo]  = _offset1;
  
  Model.MixIn2[_mixNo]        = _in2;
  Model.MixIn2Weight[_mixNo]  = _weight2;
  Model.MixIn2Diff[_mixNo]    = _diff2;
  Model.MixIn2Offset[_mixNo]  = _offset2;
  
  Model.MixOperator[_mixNo]   = _operator; 
  Model.MixSwitch[_mixNo]     = _sw;
  Model.MixOut[_mixNo]        = _out;
}
