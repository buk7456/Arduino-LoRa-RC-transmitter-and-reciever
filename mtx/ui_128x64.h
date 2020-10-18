//==================================================================================================
void HandleMainUI(); 
void HandleBootUI(); 
void DisplayFullScreenMsg(const __FlashStringHelper* text);

//helpers
void toggleEditModeOnSelectClicked();
void drawAndNavMenu(const char *const list[], int8_t _numMenuItems);
void changeToScreen(int8_t _theScrn);
void plotRateExpo(uint8_t _rate, uint8_t _expo);
void resetThrottleTimer();
void drawHeader();
void printVolts(int _milliVolts);
void printHHMMSS(unsigned long _milliSecs, int _cursorX, int _cursorY);
void changeFocusOnUPDOWN(uint8_t _maxItemNo);
void drawCursor(int16_t _xpos, int16_t _ypos);
void makeToast(const __FlashStringHelper* text, unsigned long duration);
void drawToast();
void drawPopupMenu(const char *const list[], int8_t _numItems);
void drawCheckbox(int16_t _xcord, int16_t _ycord, bool _val);
bool isDefaultModelName(char* _nameBuff, uint8_t _len);
uint8_t adjustTrim(uint8_t _decrButton, uint8_t _incrButton, uint8_t _val);
void showTrimData(const __FlashStringHelper* text, uint8_t _trimVal);
uint8_t incrDecrU8tOnUPDOWN(uint8_t _val, uint8_t _lowerLimit, uint8_t _upperLimit, bool _enableWrap, uint8_t _state);
enum {WRAP = true, NOWRAP = false};
enum {PRESSED_ONLY = 0, PRESSED_OR_HELD = 1, SLOW_CHANGE = 2}; 

void loadMix( uint8_t _mixNo, uint8_t _in1, uint8_t _weight1, uint8_t _diff1, uint8_t _offset1,
uint8_t _in2, uint8_t _weight2, uint8_t _diff2, uint8_t _offset2, uint8_t _operator, uint8_t _out);

///================================================================================================

//-- Boot popup menu strings. Max 15 characters per string
#define NUM_ITEMS_BOOT_POPUP 4
char const bootStr0[] PROGMEM = "Calibrte sticks"; 
char const bootStr1[] PROGMEM = "Show Pkts/sec"; 
char const bootStr2[] PROGMEM = "Format EEPROM"; 
char const bootStr3[] PROGMEM = "Cancel";
const char *const bootMenu[] PROGMEM = { //table to refer to the strings
  bootStr0, bootStr1, bootStr2, bootStr3
};

//-- Main menu strings. Max 16 characters per string
#define NUM_ITEMS_MAIN_MENU 7
char const main0[] PROGMEM = "Main menu"; //heading
char const main1[] PROGMEM = "Model";
char const main2[] PROGMEM = "Curves";
char const main3[] PROGMEM = "Mixer";
char const main4[] PROGMEM = "Outputs";
char const main5[] PROGMEM = "System";
char const main6[] PROGMEM = "About";
const char *const mainMenu[] PROGMEM = { //table to refer to the strings
  main0, main1, main2, main3, main4, main5, main6 
};

//Assign indices for ui states
enum
{
  //Same order as in the main menu 
  MAIN_MENU = 0,
  MODE_MODEL,
  MODE_CURVES,
  MODE_MIXER,
  MODE_OUTPUTS,
  MODE_SYSTEM,
  MODE_ABOUT,
  
  //others
  HOME_SCREEN = 50,
  
  POPUP_TIMER_MENU,
  MODE_TIMER_SETUP,
  POPUP_RENAME_MODEL,
  MODE_MIXER_OUTPUT,
  POPUP_MIXER_MENU,
  POPUP_MOVE_MIX,
  POPUP_COPY_MIX,
  POPUP_TEMPLATES_MENU,
  MODE_CALIB,
};

//-- Timer popup menu strings. Max 15 characters per string
char const tmrStr0[] PROGMEM = "Start timer 2"; //shown if timer2 is paused
char const tmrStr1[] PROGMEM = "Stop timer 2";  //shown if timer2 is playing
char const tmrStr2[] PROGMEM = "Reset timer 2";
char const tmrStr3[] PROGMEM = "Reset timer 1";
char const tmrStr4[] PROGMEM = "Setup timer 1";
#define NUM_ITEMS_TIMER_POPUP 4
const char *const timerMenuA[] PROGMEM = { //table to refer to the strings
  tmrStr0, tmrStr2, tmrStr3, tmrStr4
};
const char *const timerMenuB[] PROGMEM = { //table to refer to the strings
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
const char *const mixerMenu[] PROGMEM = { //table to refer to the strings
  mxrStr0, mxrStr1, mxrStr2, mxrStr3, mxrStr4, mxrStr5
};

#define NUM_ITEMS_TEMPLATES 5
char const tmpltStr0[] PROGMEM = "Elevon"; 
char const tmpltStr1[] PROGMEM = "Vtail"; 
char const tmpltStr2[] PROGMEM = "Flaperon";
char const tmpltStr3[] PROGMEM = "Crow braking";
char const tmpltStr4[] PROGMEM = "Diffr thrust";
const char *const templatesMenu[] PROGMEM = { //table to refer to the strings
  tmpltStr0, tmpltStr1, tmpltStr2, tmpltStr3, tmpltStr4
};

//-- mixer sources name strings. 5 characters max
char const srcName0[]  PROGMEM = "roll"; 
char const srcName1[]  PROGMEM = "ptch";
char const srcName2[]  PROGMEM = "thrt";
char const srcName3[]  PROGMEM = "yaw";
char const srcName4[]  PROGMEM = "knob";
char const srcName5[]  PROGMEM = "SwA"; 
char const srcName6[]  PROGMEM = "SwB"; 
char const srcName7[]  PROGMEM = "SwC"; 
char const srcName8[]  PROGMEM = "SwD"; 
char const srcName9[]  PROGMEM = "Ail";
char const srcName10[] PROGMEM = "Ele";
char const srcName11[] PROGMEM = "Thrt";
char const srcName12[] PROGMEM = "Rud";
char const srcName13[] PROGMEM = "None";
char const srcName14[] PROGMEM = "Ch1";
char const srcName15[] PROGMEM = "Ch2";
char const srcName16[] PROGMEM = "Ch3";
char const srcName17[] PROGMEM = "Ch4";
char const srcName18[] PROGMEM = "Ch5";
char const srcName19[] PROGMEM = "Ch6";
char const srcName20[] PROGMEM = "Ch7";
char const srcName21[] PROGMEM = "Ch8";
char const srcName22[] PROGMEM = "Virt1";
char const srcName23[] PROGMEM = "Virt2";

const char *const srcNames[] PROGMEM = { //table to refer to the strings
  srcName0, srcName1, srcName2, srcName3, srcName4, srcName5, srcName6, srcName7, 
  srcName8, srcName9, srcName10,srcName11, srcName12, srcName13, srcName14,
  srcName15, srcName16, srcName17, srcName18, srcName19, srcName20, srcName21, 
  srcName22, srcName23
};

// --- Other strings ----

//sound mode strings. Max 5 characters
char const soundModeStr0[] PROGMEM = "Off"; 
char const soundModeStr1[] PROGMEM = "Alarm"; 
char const soundModeStr2[] PROGMEM = "NoKey"; 
char const soundModeStr3[] PROGMEM = "All";
const char *const soundModeStr[] PROGMEM = { //table to refer to the strings
  soundModeStr0, soundModeStr1, soundModeStr2, soundModeStr3
};
  
//Model action strings. Max 9 characters
char const modelActionStr0[] PROGMEM = "Load";
char const modelActionStr1[] PROGMEM = "Copy from";
char const modelActionStr2[] PROGMEM = "Rename";
char const modelActionStr3[] PROGMEM = "Reset";
char const modelActionStr4[] PROGMEM = "Delete";
const char *const modelActionStr[] PROGMEM = { //table to refer to the strings
  modelActionStr0, modelActionStr1, modelActionStr2, modelActionStr3, modelActionStr4
};

char const backlightModeStr0[] PROGMEM = "Off"; 
char const backlightModeStr1[] PROGMEM = "5s"; 
char const backlightModeStr2[] PROGMEM = "15s"; 
char const backlightModeStr3[] PROGMEM = "60s";
char const backlightModeStr4[] PROGMEM = "On";
const char *const backlightModeStr[] PROGMEM = { //table to refer to the strings
  backlightModeStr0, backlightModeStr1, backlightModeStr2, backlightModeStr3, backlightModeStr4
};

// ---------------- Globals ------------------

char txtBuff[22]; //generic buffer for working with strings

int8_t theScreen = HOME_SCREEN;
uint8_t focusedItem = 1; //The item that currently has focus in MODE Screens or popups
bool isEditMode = false;

//Dont use unsigned types for these!!!
int8_t topItem = 1;         //in main menu
int8_t highlightedItem = 1; //in main menu

//mixer
uint8_t thisMixNum = 0; //these are references
uint8_t destMixNum = 0; 

// Throttle timer 
unsigned long throttleTimerElapsedTime = 0, throttleTimerLastElapsedTime = 0;
unsigned long throttleTimerLastPaused = 0;

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


//================================== Generic messages ==============================================

void DisplayFullScreenMsg(const __FlashStringHelper* text)
{
  display.clearDisplay();
  int _txtWidthPix = 6 * strlen_P((const char *)text); //(const char*) casts
  int x_offset = (display.width() - _txtWidthPix) / 2; //middle align
  display.setCursor(x_offset, 28);
  display.print(text);
  display.display();
}

///=================================================================================================
///                                BOOT UI
///=================================================================================================

void HandleBootUI()
{
  display.clearDisplay();
  drawPopupMenu(bootMenu, NUM_ITEMS_BOOT_POPUP);
  display.display();
  while (buttonCode > 0)  //wait for button release to prevent false trigger
  {
    readSwitchesAndButtons();
  }

  while(1)
  {
    delay(30);
 
    readSwitchesAndButtons();
    determineButtonEvent();
    
    display.clearDisplay();
    
    changeFocusOnUPDOWN(NUM_ITEMS_BOOT_POPUP);
    drawPopupMenu(bootMenu, NUM_ITEMS_BOOT_POPUP);
    
    display.display();
    
    uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
    
    if(_selection == 1) 
    {
      changeToScreen(MODE_CALIB);
      return; //exit
    }
    else if(_selection == 2) 
    {
      showPktsPerSec = true;
      return; //exit
    }
    else if(_selection == 3)
    {
      DisplayFullScreenMsg(F("Press any key"));
      readSwitchesAndButtons();
      while(buttonCode == 0) //wait for a key press before proceeding
      {
        readSwitchesAndButtons();
        delay(30);
      }
      delay(200);
      EEPROM.write(0, 0xFF); //Clear EEPROM init flag
      return; //exit 
    }
    else if(_selection == 4 || heldButton == SELECT_KEY) 
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
  /* This function handles the main user interface, allowing us to view, navigate and adjust values, etc.
    Three buttons are used for interaction; select, up, and down. Longpressing select acts as back.
  */

  ///------------ THROTTLE TIMER -----------------
  //controlled by throttle stick value. If throttle is above threshold, run, else pause.
  
  int thStpwtch = -500 + (10 * int(Model.throttleTimerMinThrottle));
  unsigned long timerCountDownInitVal = Model.throttleTimerCntDnInitMinutes * 60000UL;
  if(throttleIn <= thStpwtch || cutIsActivated()) //pause
  {
    throttleTimerLastElapsedTime = throttleTimerElapsedTime;
    throttleTimerLastPaused = millis();
  }
  else //run
  {
    throttleTimerElapsedTime = throttleTimerLastElapsedTime + millis() - throttleTimerLastPaused;
  }
 
  //play audio
  if(Model.throttleTimerType == TIMERCOUNTDOWN && throttleTimerElapsedTime > timerCountDownInitVal)
  {
    if((throttleTimerElapsedTime - timerCountDownInitVal) < 500) //only play sound within this timeframe
      audioToPlay = AUDIO_TIMERELAPSED;
  }
  
  ///--------------- GENERIC STOPWATCH --------------------
  if(stopwatchIsPaused == false) //run
    stopwatchElapsedTime = stopwatchLastElapsedTime + millis() - stopwatchLastPaused;
  else //pause
  {
    stopwatchLastElapsedTime = stopwatchElapsedTime;
    stopwatchLastPaused = millis();
  }

  /// -------------- LOW BATTERY WARN ----------------------
  if(battState == BATTLOW)
  {
    if(battWarnDismissed == false)
    {
      //show warning
      DisplayFullScreenMsg(F("Battery Low"));
      audioToPlay = AUDIO_BATTERYWARN; 
      //dismiss warning
      if((clickedButton > 0 || millis() - battWarnMillisQQ > 3000))
      {
        battWarnDismissed = true;
        battWarnMillisQQ = millis();
      }
      return; 
    }
    //remind low battery
    if(battWarnDismissed == true && (millis() - battWarnMillisQQ > 600000UL)) 
    {
      battWarnDismissed = false;
      battWarnMillisQQ = millis();
    }
  }
  else
    battWarnMillisQQ = millis();

  
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
        
        //---------show cut icon -----------
        if (cutIsActivated())
          display.drawBitmap(63, 1, cut_icon, 13, 6, 1);

        //---------show dualrate icon --------
        if (SwBEngaged && (Model.DualRateEnabled[AILRTE] || Model.DualRateEnabled[ELERTE] || Model.DualRateEnabled[RUDRTE]))
          display.drawBitmap(79, 1, dualrate_icon, 13, 6, 1);
        
        //--------show rf icon------------
        if (Sys.rfOutputEnabled == true)
          display.drawBitmap(97, 0, rf_icon, 7, 7, 1);
        
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

        //----show throttle timer---------
        display.drawBitmap(13, 33, pow_icon, 4, 5, 1);
        if(Model.throttleTimerType == TIMERCOUNTUP)
          printHHMMSS(throttleTimerElapsedTime, 20, 32);
        else if(Model.throttleTimerType == TIMERCOUNTDOWN)
        {
          unsigned long timerCountDownInitVal = Model.throttleTimerCntDnInitMinutes * 60000UL;
          if(throttleTimerElapsedTime < timerCountDownInitVal)
          {
            unsigned long ttqq = timerCountDownInitVal - throttleTimerElapsedTime;
            printHHMMSS(ttqq + 999, 20, 32); //add 999ms so the displayed time doesnt 
            //change immediately upon running the timer
          }
          else
          {
            unsigned long ttqq = throttleTimerElapsedTime - timerCountDownInitVal;
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
          if(DigChA) display.drawBitmap(110, 1, chA_icon1, 9, 9, 1);
          else display.drawBitmap(110, 1, chA_icon0, 9, 9, 1);
          if(DigChB) display.drawBitmap(110, 29, chB_icon1, 9, 9, 1);
          else display.drawBitmap(110, 29, chB_icon0, 9, 9, 1);
        }
        else if(homeScreenMode == TRIMMODE)
        {
          display.drawBitmap(53, 0, lock_icon, 6, 7, 1); 
          
          if(pressedButton == SELECT_KEY) //prevent possible double beeps
            audioToPlay = AUDIO_NONE;
          //handle keys
          if(clickedButton == SELECT_KEY) 
          {
            audioToPlay = AUDIO_TRIMSELECTED;
            selectedTrim++;
            if(selectedTrim > 3)
              selectedTrim = 0;
          }
          //adjust
          uint8_t oldTrimVal = Model.Trim[selectedTrim];
          Model.Trim[selectedTrim] = adjustTrim(DOWN_KEY, UP_KEY, Model.Trim[selectedTrim]);
          if(Model.Trim[selectedTrim] != oldTrimVal)
            trimIsPendingSave = true;
          
          //show values
          if(selectedTrim == 0)      showTrimData(F("Ail"), Model.Trim[0]);
          else if(selectedTrim == 1) showTrimData(F("Ele"), Model.Trim[1]);
          else if(selectedTrim == 2) showTrimData(F("Thr"), Model.Trim[2]);
          else if(selectedTrim == 3) showTrimData(F("Rud"), Model.Trim[3]);
          
          display.drawHLine(68, 62, 51, BLACK);
          display.drawVLine(126, 12, 51, BLACK);
          display.drawVLine(1, 12, 51, BLACK);
          display.drawHLine(9, 62, 51, BLACK);
          //draw slider icons
          display.drawRect(Model.Trim[0] - 8, 61, 3, 3, BLACK);
          display.drawRect(125, 136 - Model.Trim[1], 3, 3, BLACK);
          display.drawRect(0, 136 - Model.Trim[2], 3, 3, BLACK);
          display.drawRect(Model.Trim[3] - 67, 61, 3, 3, BLACK); 
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
          resetThrottleTimer();
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
        strcpy_P(txtBuff, PSTR("Timer 1"));
        drawHeader();
      
        display.setCursor(1, 10);
        display.print(F("Throttle >=  "));
        display.print(Model.throttleTimerMinThrottle);
        display.print(F("%"));
        
        display.setCursor(1, 19);
        display.print(F("Timer type:  "));
        if(Model.throttleTimerType == TIMERCOUNTDOWN)
          display.print(F("CntDn"));
        else if(Model.throttleTimerType == TIMERCOUNTUP)
          display.print(F("CntUp"));
        
        uint8_t _maxFocusableItems = 2;
        
        if(Model.throttleTimerType == TIMERCOUNTDOWN)
        {
          _maxFocusableItems = 3;
          display.setCursor(31, 28);
          display.print(F("Start:  "));
          display.print(Model.throttleTimerCntDnInitMinutes);
          display.print(F(" min"));
        }
      
        changeFocusOnUPDOWN(_maxFocusableItems);
        toggleEditModeOnSelectClicked();
        drawCursor(71, (focusedItem * 9) + 1);
        
        if (focusedItem == 1)
          Model.throttleTimerMinThrottle = incrDecrU8tOnUPDOWN(Model.throttleTimerMinThrottle, 0, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 2)
          Model.throttleTimerType = incrDecrU8tOnUPDOWN(Model.throttleTimerType, TIMERCOUNTUP, TIMERCOUNTDOWN, WRAP, PRESSED_ONLY);
        else if(focusedItem == 3)
          Model.throttleTimerCntDnInitMinutes = incrDecrU8tOnUPDOWN(Model.throttleTimerCntDnInitMinutes, 1, 240, NOWRAP, PRESSED_OR_HELD);
      
        if (heldButton == SELECT_KEY)
        {
          eeSaveModelData(Sys.activeModel);
          changeToScreen(HOME_SCREEN);
        }
      }
      break;

    case MAIN_MENU:
      {
        drawAndNavMenu(mainMenu, NUM_ITEMS_MAIN_MENU);
        if (clickedButton == SELECT_KEY)
          changeToScreen(highlightedItem);
        else if (heldButton == SELECT_KEY)
        {
          //reset menu
          highlightedItem = 1;
          topItem = 1;
          
          changeToScreen(HOME_SCREEN);
        }
      }
      break;
      
    case MODE_MODEL:
      {
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_MODEL])));
        drawHeader();

        enum {LOADMODEL = 0, COPYFROMMODEL, RENAMEMODEL, RESETMODEL, DELETEMODEL};
        static uint8_t _action_ = LOADMODEL;
        static uint8_t _thisMdl_ = Sys.activeModel;
        
        char _tmpBuff[sizeof(Model.modelName)/sizeof(Model.modelName[0])]; //for model names
        
        //-- show action
        display.setCursor(49, 12);
        strcpy_P(txtBuff, (char *)pgm_read_word(&(modelActionStr[_action_])));
        display.print(txtBuff);

        //-- show model name
        
        if(_action_ > COPYFROMMODEL)
          _thisMdl_ = Sys.activeModel; //reinit
        
        display.setCursor(49, 22);
        
        eeCopyModelName(_thisMdl_, _tmpBuff); //copy model name into temporary buffer
        if(isDefaultModelName(_tmpBuff, sizeof(_tmpBuff)/sizeof(_tmpBuff[0])))
        { 
          display.print(F("MODEL"));
          display.print(_thisMdl_);
        }
        else
          display.print(_tmpBuff);
        
        //-- show confirmation
        display.setCursor(49, 32);
        display.print(F("Confirm"));


        changeFocusOnUPDOWN(3);
        toggleEditModeOnSelectClicked();
        drawCursor(41, (focusedItem * 10) + 2);
        
        if (focusedItem == 1) 
          _action_ = incrDecrU8tOnUPDOWN(_action_, 0, 4, WRAP, SLOW_CHANGE);
        
        else if (focusedItem == 2 && (_action_ == LOADMODEL || _action_ == COPYFROMMODEL))
          _thisMdl_ = incrDecrU8tOnUPDOWN(_thisMdl_, 1, numOfModels, WRAP, SLOW_CHANGE);
        
        else if (focusedItem == 3 && isEditMode) //confirm action
        {
          if(_action_ == RENAMEMODEL)
          {
            _action_ = LOADMODEL; //reinit
            _thisMdl_ = Sys.activeModel; //reinit
            changeToScreen(POPUP_RENAME_MODEL);
          }
          else
          {
            if(_action_ == LOADMODEL)
            {
              //Save the active model before changing to another model
              eeSaveModelData(Sys.activeModel);
              //load into ram
              eeReadModelData(_thisMdl_);
              //set as active model
              Sys.activeModel = _thisMdl_; 

              makeToast(F("Loaded"), 1500);
            }
            else if(_action_ == COPYFROMMODEL)
            {
              //temporarily store model name as we shall maintain it 
              strcpy(_tmpBuff, Model.modelName);
              //load source model into ram
              eeReadModelData(_thisMdl_);
              //restore model name
              strcpy(Model.modelName, _tmpBuff);
              //save
              eeSaveModelData(Sys.activeModel);
              
              makeToast(F("Copied"), 1500);
            }
            else if (_action_ == RESETMODEL)
            {
              //set defaults, except the name
              setDefaultModelBasicParams();
              setDefaultModelMixerParams();
              //save
              eeSaveModelData(Sys.activeModel);

              makeToast(F("Reset"), 1500);
            }
            else if (_action_ == DELETEMODEL)
            {
              //set defaults, as well as name
              setDefaultModelBasicParams();
              setDefaultModelMixerParams();
              setDefaultModelName();
              //save
              eeSaveModelData(Sys.activeModel);
              
              makeToast(F("Deleted"), 1500);
            }

            //reset other stuff
            resetThrottleTimer();
            Sys.rfOutputEnabled = false;
            
            //save system
            eeSaveSysConfig();
            
            //reinit
            _action_  = LOADMODEL;
            _thisMdl_ = Sys.activeModel;
            
            changeToScreen(HOME_SCREEN);
          }
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
      
    case POPUP_RENAME_MODEL:
      {
        display.drawRect(15,11,97,40,BLACK); //draw bounding box
        
        display.setCursor(19,14);
        display.print(F("Rename Model"));
        display.print(Sys.activeModel); 
        display.setCursor(19,23);
        display.print(F("Name:  "));
        display.print(Model.modelName);
        
        isEditMode = true;

        static uint8_t charPos = 0;
        uint8_t thisChar = Model.modelName[charPos] ;
        
        if(thisChar == 32) thisChar = 0; //map ascii 32 (space) to 0
        else if(thisChar >= 65 && thisChar <= 90) thisChar -= 64; //map ascii 65..90 to 1..26
        else if(thisChar >= 45 && thisChar <= 57) thisChar -= 18; //map ascii 45..57 to 27..39

        thisChar = incrDecrU8tOnUPDOWN(thisChar, 0, 39, NOWRAP, SLOW_CHANGE);

        //map back
        if(thisChar == 0) thisChar = 32; //map 0 to ascii 32 (space)
        else if(thisChar >= 1 && thisChar <= 26) thisChar += 64; //map 1..26 to ascii 65..90
        else if(thisChar >= 27 && thisChar <= 39) thisChar += 18; //map 27..39 to ascii 45..57

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
          makeToast(F("Done"), 1500);
        }
      }
      break;

    case MODE_CURVES:
      {
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_CURVES])));
        drawHeader();

        enum{AIL_CURVE = 0, ELE_CURVE, RUD_CURVE, THR_CURVE};
        static uint8_t displayedCurve = AIL_CURVE;
        
        if (focusedItem == 1) //switch to another curve
          displayedCurve = incrDecrU8tOnUPDOWN(displayedCurve, 0, 3, WRAP, SLOW_CHANGE);
          
        uint8_t _maxFocusableItems = 1;
        
        /////////////////RATES AND EXPO////////////////////////////////////////
        if(displayedCurve < THR_CURVE)  
        {  
          _maxFocusableItems = 4;
          
          uint8_t _rate[2] = {0, 0};
          uint8_t _expo[2] = {0, 0};
          //-----Pass parameters to the arrays for rates and expo-----
          if (displayedCurve == AIL_CURVE)
          {
            _rate[0] = Model.RateNormal[AILRTE];
            _rate[1] = Model.RateSport[AILRTE];
            _expo[0] = Model.ExpoNormal[AILRTE];
            _expo[1] = Model.ExpoSport[AILRTE];
          }
          else if (displayedCurve == ELE_CURVE)
          {
            _rate[0] = Model.RateNormal[ELERTE];
            _rate[1] = Model.RateSport[ELERTE];
            _expo[0] = Model.ExpoNormal[ELERTE];
            _expo[1] = Model.ExpoSport[ELERTE];
          }
          else if (displayedCurve == RUD_CURVE)
          {
            _rate[0] = Model.RateNormal[RUDRTE];
            _rate[1] = Model.RateSport[RUDRTE];
            _expo[0] = Model.ExpoNormal[RUDRTE];
            _expo[1] = Model.ExpoSport[RUDRTE];
          }
          
          //-----Adjudt values on key presses----
          if (focusedItem == 2) //adjust rate
          {
            if(SwBEngaged == false || Model.DualRateEnabled[displayedCurve] == false)
              _rate[0] = incrDecrU8tOnUPDOWN(_rate[0], 0, 100, NOWRAP, PRESSED_OR_HELD); 
            else
              _rate[1] = incrDecrU8tOnUPDOWN(_rate[1], 0, 100, NOWRAP, PRESSED_OR_HELD); 
          }
          else if (focusedItem == 3) //adjust expo
          {
            if(SwBEngaged == false || Model.DualRateEnabled[displayedCurve] == false)
              _expo[0] = incrDecrU8tOnUPDOWN(_expo[0], 0, 200, NOWRAP, PRESSED_OR_HELD);
            else 
              _expo[1] = incrDecrU8tOnUPDOWN(_expo[1], 0, 200, NOWRAP, PRESSED_OR_HELD);
          }
          else if (focusedItem == 4) //toggle dualrate
            Model.DualRateEnabled[displayedCurve] = incrDecrU8tOnUPDOWN(Model.DualRateEnabled[displayedCurve],0,1,WRAP,PRESSED_ONLY);
        
          
          //------ Write the values ------
          if (displayedCurve == AIL_CURVE)
          {
            Model.RateNormal[AILRTE] = _rate[0];
            Model.RateSport[AILRTE]  = _rate[1];
            Model.ExpoNormal[AILRTE] = _expo[0];
            Model.ExpoSport[AILRTE]  = _expo[1];
          }
          else if (displayedCurve == ELE_CURVE)
          {
            Model.RateNormal[ELERTE] = _rate[0];
            Model.RateSport[ELERTE]  = _rate[1];
            Model.ExpoNormal[ELERTE] = _expo[0];
            Model.ExpoSport[ELERTE]  = _expo[1];
          }
          else if (displayedCurve == RUD_CURVE)
          {
            Model.RateNormal[RUDRTE] = _rate[0];
            Model.RateSport[RUDRTE]  = _rate[1];
            Model.ExpoNormal[RUDRTE] = _expo[0];
            Model.ExpoSport[RUDRTE]  = _expo[1];
          }
          
          //----Show on screen----
          
          int _stickInpt = 0; //for showing input line marker on dual rate expo graph
          display.setCursor(0,11);
          display.print(F("Curv:  "));
          if (displayedCurve == AIL_CURVE)
          {
            display.print(F("Ail"));
            _stickInpt = rollIn;
          }
          else if (displayedCurve == ELE_CURVE)
          {
            display.print(F("Ele"));
            _stickInpt = pitchIn;
          }
          else if (displayedCurve == RUD_CURVE) 
          {
            display.print(F("Rud"));
            _stickInpt = yawIn;
          }
          
          uint8_t _datIDX = 1; 
          if(SwBEngaged == false || Model.DualRateEnabled[displayedCurve] == false)
            _datIDX = 0; 
          
          display.setCursor(0, 20);
          display.print(F("Rate:  "));
          display.print(_rate[_datIDX]);
          display.print(F("%"));
          display.setCursor(0, 29);
          display.print(F("Expo:  "));
          display.print(_expo[_datIDX] - 100);
          display.print(F("%"));
          
          display.setCursor(0, 38);
          display.print(F("D/R :  "));
          drawCheckbox(42, 38, Model.DualRateEnabled[displayedCurve]);

          if( SwBEngaged == true 
              && ((displayedCurve == AIL_CURVE && Model.DualRateEnabled[AILRTE] == true)
                   || (displayedCurve == ELE_CURVE && Model.DualRateEnabled[ELERTE] == true)
                   || (displayedCurve == RUD_CURVE && Model.DualRateEnabled[RUDRTE] == true)))
          { 
            display.drawRect(0,47,33,11,BLACK);
            display.setCursor(2,49);
            display.print(F("Sport"));
          }

          //draw graph 
          plotRateExpo(_rate[_datIDX], _expo[_datIDX]);
          //draw stick input marker
          int qq = calcRateExpo(_stickInpt, _rate[_datIDX], _expo[_datIDX]) / 20;
          display.fillRect(99 + _stickInpt/20, 35 - qq, 3, 3, BLACK);
        }
    
        ////////////////THROTTLE CURVE////////////////////////////////////////
        if(displayedCurve == THR_CURVE)
        {
          _maxFocusableItems = 6;
          
          display.setCursor(0,11);
          display.print(F("Curv:  "));
          display.print(F("Thrt"));
          //adjust values
          if (focusedItem >= 2)
          {
            int _idx = 6 - focusedItem;
            Model.ThrottlePts[_idx] = incrDecrU8tOnUPDOWN(Model.ThrottlePts[_idx], 0, 200, NOWRAP, PRESSED_OR_HELD);
          }
   
          display.setCursor(0,20);
          display.print(F("Pt"));
          for (int i = 0; i < 5; i++)
          {
            display.setCursor(18, 20 + i * 9);
            display.write(101 - i); //e,d,c,b,a
            display.print(F(":  "));
            display.print(Model.ThrottlePts[4 - i] - 100);
          }
          
          //----Show graph
          
          //draw the axes
          display.drawVLine(100, 11, 51, BLACK);
          display.drawHLine(74, 36, 52, BLACK);
          
          //Interpolate and draw points. We use x cordinate to estimate corresponding y cordinate
          //Actual plot area is 50x50.
          int xpts[5] = {0, 250, 500, 750, 1000};
          int ypts[5];
          for(int i = 0; i < 5; i++)
            ypts[i] = (int)Model.ThrottlePts[i] * 5;
                            
          for (int xval = 0; xval <= 50; xval++) //plot
          {
            int yval = linearInterpolate(xpts, ypts, 5, xval * 20) / 20;
            display.drawPixel(75 + xval, 61 - yval, BLACK); //plot points
          }
          
          //draw throttle stick input indication and point selected
          int _thrOut = linearInterpolate(xpts, ypts, 5, throttleIn + 500) / 20;
          display.fillRect(74 + ((throttleIn + 500) / 20), 60 - _thrOut ,3, 3, BLACK);
          
          //show point we are adjusting
          if(focusedItem >= 2)
          {
            int _pt = 6 - focusedItem;
            int _qq = linearInterpolate(xpts, ypts, 5, _pt * 250) / 20;
            display.fillRect(74 + xpts[_pt]/20, 60 - _qq ,3, 3, WHITE);
            display.drawRect(74 + xpts[_pt]/20, 60 - _qq ,3, 3, BLACK);
          }
        }
        
        ////// Move cursor
        changeFocusOnUPDOWN(_maxFocusableItems);
        toggleEditModeOnSelectClicked();
        drawCursor(34, (focusedItem * 9) + 2);

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
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_MIXER])));
        drawHeader();
        
        display.setCursor(24, 8);
        display.print(F("Mix:  #"));
        display.print(thisMixNum + 1);
        
        display.setCursor(6, 16);
        display.print(F("Output:  "));
        int _outNameIndex = Model.MixOut[thisMixNum];
        strcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_outNameIndex])));
        display.print(txtBuff);
        
        display.setCursor(12, 24);
        display.print(F("Input:  "));
        int _In1NameIndex = Model.MixIn1[thisMixNum];
        strcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_In1NameIndex])));
        display.print(txtBuff);
        
        display.setCursor(97, 24);
        int _In2NameIndex = Model.MixIn2[thisMixNum];
        strcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_In2NameIndex])));
        display.print(txtBuff);
        
        display.setCursor(6, 32);
        display.print(F("Weight:  "));
        display.print(Model.MixIn1Weight[thisMixNum] - 100);
        display.print(F("%"));
        display.setCursor(97, 32);
        display.print(Model.MixIn2Weight[thisMixNum] - 100);
        display.print(F("%"));
        
        display.setCursor(18, 40);
        display.print(F("Diff:  "));
        display.print(Model.MixIn1Diff[thisMixNum] - 100);
        display.print(F("%"));
        display.setCursor(97, 40);
        display.print(Model.MixIn2Diff[thisMixNum] - 100);
        display.print(F("%"));
        
        display.setCursor(6, 48);
        display.print(F("Offset:  "));
        display.print(Model.MixIn1Offset[thisMixNum] - 100);
        display.setCursor(97, 48);
        display.print(Model.MixIn2Offset[thisMixNum] - 100);
        
        display.setCursor(24, 56);
        display.print(F("Mux:  "));
        if(Model.MixOperator[thisMixNum] == 0)
          display.print(F("Add"));
        else 
          display.print(F("Multiply"));

        changeFocusOnUPDOWN(12);
        toggleEditModeOnSelectClicked();
        if(focusedItem < 8)
          drawCursor(52, focusedItem * 8);
        else if(focusedItem < 12)
          drawCursor(89, (focusedItem * 8) - 40);
        
        //show menu icon
        display.fillRect(120, 0, 8, 7, WHITE);
        if(focusedItem == 12)
          display.drawBitmap(120, 0, menu_icon_focused, 8, 7, 1);
        else
            display.drawBitmap(120, 0, menu_icon, 8, 7, 1);

        //edit values
        if (focusedItem == 1)     //Change to another mixer slot
          thisMixNum = incrDecrU8tOnUPDOWN(thisMixNum, 0, NUM_MIXSLOTS - 1, WRAP, SLOW_CHANGE);
        else if(focusedItem == 2) //change output
          Model.MixOut[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixOut[thisMixNum], IDX_NONE, IDX_VRT2, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 3) //change input 1
          Model.MixIn1[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixIn1[thisMixNum], 0, IDX_VRT2, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 4) //adjust weight 1
          Model.MixIn1Weight[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixIn1Weight[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 5) //adjust differential 1
          Model.MixIn1Diff[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixIn1Diff[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 6) //adjust offset 1
          Model.MixIn1Offset[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixIn1Offset[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 7) //change operator
          Model.MixOperator[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixOperator[thisMixNum], 0, 1, WRAP, PRESSED_ONLY);
        else if(focusedItem == 8) //change input 2
          Model.MixIn2[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixIn2[thisMixNum], 0, IDX_VRT2, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 9) //adjust weight 2
          Model.MixIn2Weight[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixIn2Weight[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 10) //adjust differential 2
          Model.MixIn2Diff[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixIn2Diff[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 11) //adjust offset 2
          Model.MixIn2Offset[thisMixNum] = incrDecrU8tOnUPDOWN(Model.MixIn2Offset[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        
        //open context menu
        if(focusedItem == 12 && clickedButton == SELECT_KEY)
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
          setDefaultModelMixerParams();
          thisMixNum = 0;
          destMixNum = 0;
          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 6) 
        {
          changeToScreen(POPUP_TEMPLATES_MENU);
        }
        else if(heldButton == SELECT_KEY) //exit
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case MODE_MIXER_OUTPUT:
      {
        strcpy_P(txtBuff, PSTR("Mixer output"));
        drawHeader();
        
        display.setCursor(0,56);
        display.print(F("Ch"));
        
        // Graph mixer outputs
        for (int i = 0; i < NUM_PRP_CHANNLES; i++)
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
        
        destMixNum = incrDecrU8tOnUPDOWN(destMixNum, 0, NUM_MIXSLOTS - 1, NOWRAP, SLOW_CHANGE);
        
        if(isEditMode == false)
        {
          uint8_t oldPostn = thisMixNum;
          uint8_t newPostn = destMixNum;
          
          //store temporarily the old position's data
          uint8_t _mixout      =   Model.MixOut[oldPostn];
          uint8_t _mix1in      =   Model.MixIn1[oldPostn];
          uint8_t _mix1weight  =   Model.MixIn1Weight[oldPostn];
          uint8_t _mix1offset1 =   Model.MixIn1Offset[oldPostn];
          uint8_t _mix1diff    =   Model.MixIn1Diff[oldPostn];
          uint8_t _mixOper     =   Model.MixOperator[oldPostn];
          uint8_t _mix2in      =   Model.MixIn2[oldPostn];
          uint8_t _mix2weight  =   Model.MixIn2Weight[oldPostn];
          uint8_t _mix2offset1 =   Model.MixIn2Offset[oldPostn];
          uint8_t _mix2diff    =   Model.MixIn2Diff[oldPostn];
          
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
        
        destMixNum = incrDecrU8tOnUPDOWN(destMixNum, 0, NUM_MIXSLOTS - 1, NOWRAP, SLOW_CHANGE);
        
        if(isEditMode == false)
        {
          Model.MixOut[destMixNum]       = Model.MixOut[thisMixNum];
          Model.MixIn1[destMixNum]       = Model.MixIn1[thisMixNum];
          Model.MixIn1Weight[destMixNum] = Model.MixIn1Weight[thisMixNum];
          Model.MixIn1Offset[destMixNum] = Model.MixIn1Offset[thisMixNum];
          Model.MixIn1Diff[destMixNum]   = Model.MixIn1Diff[thisMixNum];
          Model.MixOperator[destMixNum]  = Model.MixOperator[thisMixNum];
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
        changeFocusOnUPDOWN(NUM_ITEMS_TEMPLATES);
        drawPopupMenu(templatesMenu, NUM_ITEMS_TEMPLATES);
        uint8_t _selection = clickedButton == SELECT_KEY ? focusedItem : 0;
        
        if(_selection == 1)
        {
          //elevon 
          // Ch1 = -50%Ail + -50%Ele, 
          // Ch2 = 50%Ail + -50%Ele
          loadMix(thisMixNum,     IDX_AIL, 50,  100, 100, IDX_ELE, 50, 100, 100, 0, IDX_CH1);
          loadMix(thisMixNum + 1, IDX_AIL, 150, 100, 100, IDX_ELE, 50, 100, 100, 0, IDX_CH2);

          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 2)
        {
          //vtail  
          // Ch2 = 50%Rud + -50%Ele, 
          // Ch4 = -50%Rud + -50%Ele
          loadMix(thisMixNum,     IDX_RUD, 150, 100, 100, IDX_ELE, 50, 100, 100, 0, IDX_CH2);
          loadMix(thisMixNum + 1, IDX_RUD, 50,  100, 100, IDX_ELE, 50, 100, 100, 0, IDX_CH4);

          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 3)
        {
          //flaperon
          // Ch1 = -100%Ail{-25%Diff} + -50%SwC{-50offset}
          // Ch8 = 100%Ail{25%Diff} + -50%SwC{-50offset}
          loadMix(thisMixNum,     IDX_AIL, 0,   75, 100, IDX_SWC, 50, 100, 50, 0, IDX_CH1);
          loadMix(thisMixNum + 1, IDX_AIL, 200, 125, 100, IDX_SWC, 50, 100, 50, 0, IDX_CH8);

          changeToScreen(MODE_MIXER);
        }  
        else if(_selection == 4)
        {
          //crow 
          // Ch1 = -100%Ail{-25%Diff} + 50%SwC{100%Diff}
          // Ch8 = 100%Ail{ 25%Diff} + 50%SwC{100%Diff}
          // Ch5 = -50%SwC{-50offset}
          // Ch6 = 100%Ch5
          loadMix(thisMixNum,     IDX_AIL, 0,   75,  100, IDX_SWC,  150, 200, 100, 0, IDX_CH1);
          loadMix(thisMixNum + 1, IDX_AIL, 200, 125, 100, IDX_SWC,  150, 200, 100, 0, IDX_CH8);
          loadMix(thisMixNum + 2, IDX_SWC, 50,  100,  50, IDX_NONE, 100, 100, 100, 0, IDX_CH5);
          loadMix(thisMixNum + 3, IDX_CH5, 200, 100, 100, IDX_NONE, 100, 100, 100, 0, IDX_CH6);

          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 5) //twin motor
        {
          //twin
          // Virt1 = 40%Rud * 100%SwD{100%Diff}
          // Ch3 = 100%Thrt + 100%Virt1
          // Ch7 = 100%Thrt + -100%Virt1
          loadMix(thisMixNum,     IDX_RUD,        140, 100, 100, IDX_SWD,  200, 200, 100, 1, IDX_VRT1);
          loadMix(thisMixNum + 1, IDX_THRTL_CURV, 200, 100, 100, IDX_VRT1, 200, 100, 100, 0, IDX_CH3);
          loadMix(thisMixNum + 2, IDX_THRTL_CURV, 200, 100, 100, IDX_VRT1, 0,   100, 100, 0, IDX_CH7);

          changeToScreen(MODE_MIXER);
        }
        else if(heldButton == SELECT_KEY) //exit
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case MODE_OUTPUTS:
      {
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_OUTPUTS])));
        drawHeader();

        changeFocusOnUPDOWN(7);
        toggleEditModeOnSelectClicked();
        drawCursor(71, focusedItem * 8);
        
        static uint8_t _selectedChannel = 0; //0 is ch1, 1 is ch2, etc.
    
        if (focusedItem == 1)
          _selectedChannel = incrDecrU8tOnUPDOWN(_selectedChannel, 0, NUM_PRP_CHANNLES - 1, WRAP, SLOW_CHANGE); 
        else if (focusedItem == 2)
          Model.Reverse[_selectedChannel] = incrDecrU8tOnUPDOWN(Model.Reverse[_selectedChannel], 0, 1, WRAP, PRESSED_ONLY);
        else if (focusedItem == 3)
          Model.Subtrim[_selectedChannel] = incrDecrU8tOnUPDOWN(Model.Subtrim[_selectedChannel], 25, 75, NOWRAP, SLOW_CHANGE);
        else if (focusedItem == 4)
          Model.CutValue[_selectedChannel] = incrDecrU8tOnUPDOWN(Model.CutValue[_selectedChannel], 0, 201, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 5)
          Model.Failsafe[_selectedChannel] = incrDecrU8tOnUPDOWN(Model.Failsafe[_selectedChannel], 0, 201, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 6)
          Model.EndpointL[_selectedChannel] = incrDecrU8tOnUPDOWN(Model.EndpointL[_selectedChannel], 0, 100, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 7)
          Model.EndpointR[_selectedChannel] = incrDecrU8tOnUPDOWN(Model.EndpointR[_selectedChannel], 0, 100, NOWRAP, PRESSED_OR_HELD);

        //-------Show on lcd---------------
        display.setCursor(49, 8);
        display.print(F("Ch:  "));
        display.print(_selectedChannel + 1);
        
        display.setCursor(19, 16);
        display.print(F("Reverse:  "));
        drawCheckbox(79, 16, Model.Reverse[_selectedChannel]);

        display.setCursor(19, 24);
        display.print(F("Subtrim:  "));
        int _trmQQ = int(Model.Subtrim[_selectedChannel]) - 50;
        display.print(_trmQQ); //show as centered about 50
        
        display.setCursor(43, 32);
        display.print(F("Cut:  "));
        if(Model.CutValue[_selectedChannel]== 0)
          display.print(F("Off"));
        else
          display.print(Model.CutValue[_selectedChannel] - 101);
        
        display.setCursor(19, 40);
        display.print(F("Failsaf:  "));
        if(Model.Failsafe[_selectedChannel]== 0)
          display.print(F("Off"));
        else
          display.print(Model.Failsafe[_selectedChannel] - 101);

        display.setCursor(25, 48);
        display.print(F("Travel:  "));
        display.print(0 - Model.EndpointL[_selectedChannel]);
        
        display.setCursor(49, 56);
        display.print(F("to:  "));
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
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_SYSTEM])));
        drawHeader();

        display.setCursor(13, 10);
        display.print(F("RFoutput:"));
        drawCheckbox(79, 10, Sys.rfOutputEnabled);
        
        display.setCursor(13, 19);
        display.print(F("Sounds  :  "));
        strcpy_P(txtBuff, (char *)pgm_read_word(&(soundModeStr[Sys.soundMode])));
        display.print(txtBuff);
        
        display.setCursor(13, 28);
        display.print(F("Backlght:  "));
        strcpy_P(txtBuff, (char *)pgm_read_word(&(backlightModeStr[Sys.backlightMode])));
        display.print(txtBuff);
        
        display.setCursor(13, 37);
        display.print(F("Ch3 Mode:  "));
        if(Sys.PWM_Mode_Ch3 == 1) 
          display.print(F("ServoPWM"));
        else  
          display.print(F("PWM"));
        
        display.setCursor(13, 46);
        display.print(F("Receiver:  [Bind]"));
        
        changeFocusOnUPDOWN(5);
        toggleEditModeOnSelectClicked();
        drawCursor(71, 10 + (focusedItem - 1) * 9);
        
        //edit values
        if (focusedItem == 1)
          Sys.rfOutputEnabled = incrDecrU8tOnUPDOWN(Sys.rfOutputEnabled, 0, 1, WRAP, PRESSED_ONLY);
        else if (focusedItem == 2)
          Sys.soundMode = incrDecrU8tOnUPDOWN(Sys.soundMode, 0, 3, WRAP, PRESSED_ONLY);
        else if (focusedItem == 3)
          Sys.backlightMode = incrDecrU8tOnUPDOWN(Sys.backlightMode, 0, 4, WRAP, PRESSED_ONLY);     
        else if (focusedItem == 4)
        {
          Sys.PWM_Mode_Ch3 = incrDecrU8tOnUPDOWN(Sys.PWM_Mode_Ch3, 0, 1, WRAP, PRESSED_ONLY);
          if(isEditMode && (pressedButton == UP_KEY || pressedButton == DOWN_KEY))
            makeToast(F("Restart receiver"), 2000);
        }
        else if (focusedItem == 5 && isEditMode)
        {
          bindActivated = true;
          Sys.transmitterID = random(128) & 0x7F; //generate a random txID
          eeSaveSysConfig();
          makeToast(F("Sending bind.."), 2000);
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

    case MODE_CALIB:
      {
        strcpy_P(txtBuff, PSTR("Calibration"));
        drawHeader();

        isCalibratingSticks = true;
        
        enum {STICKS_MOVE = 0, STICKS_CENTER, STICKS_DEADZONE};
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

        display.setCursor(24, 16);
        display.print(F("[OK] when done"));

        if (calibStage == STICKS_MOVE) 
        {
          display.setCursor(13, 8);
          display.print(F("Move sticks fully"));

          //get min and max
          int _reading = analogRead(PIN_ROLL);
          if (_reading < Sys.rollMin)       Sys.rollMin = _reading;
          else if (_reading > Sys.rollMax)  Sys.rollMax = _reading;
          _reading = analogRead(PIN_YAW);
          if (_reading < Sys.yawMin)        Sys.yawMin = _reading;
          else if (_reading > Sys.yawMax)   Sys.yawMax = _reading;
          _reading = analogRead(PIN_PITCH);
          if (_reading < Sys.pitchMin)      Sys.pitchMin = _reading;
          else if (_reading > Sys.pitchMax) Sys.pitchMax = _reading;
          _reading = analogRead(PIN_THROTTLE);
          if (_reading < Sys.thrtlMin)      Sys.thrtlMin = _reading;
          else if (_reading > Sys.thrtlMax) Sys.thrtlMax = _reading;

          if (clickedButton == SELECT_KEY)
            calibStage = STICKS_CENTER;
        }
        
        else if (calibStage == STICKS_CENTER)
        {
          display.setCursor(25, 8);
          display.print(F("Center sticks"));

          //get stick centers
          Sys.rollCenterVal  = analogRead(PIN_ROLL);
          Sys.yawCenterVal   = analogRead(PIN_YAW);
          Sys.pitchCenterVal = analogRead(PIN_PITCH);

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
          }
        }
        
        else if (calibStage == STICKS_DEADZONE)
        {
          display.setCursor(20, 8);
          display.print(F("Adjust deadzone"));
          
          isEditMode = true;
          Sys.deadZonePerc = incrDecrU8tOnUPDOWN(Sys.deadZonePerc, 0, 15, NOWRAP, PRESSED_OR_HELD);
          drawCursor(52, 34);
          isEditMode = false;
          
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
            makeToast(F("Calibrated"), 1500);
            //reset main menu view
            highlightedItem = 1;
            topItem = 1;
          }
        }
      }
      break;
      
    case MODE_ABOUT:
      {
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_ABOUT])));
        drawHeader();

        //Show battery voltage
        display.setCursor(19,10);
        display.print(F("Battery: "));
        printVolts(battVoltsNow);
        
        //Show uptime
        display.setCursor(19,19);
        display.print(F("Uptime:  "));
        printHHMMSS(millis(), 73, 19);
        
        //show version
        display.setCursor(19, 28);
        display.print(F("FW ver:  "));
        display.print(F(_SKETCHVERSION));
        
        //Show author
        display.setCursor(19, 37);
        display.print(F("Devlpr:  buk7456"));

        if (heldButton == SELECT_KEY)
          changeToScreen(MAIN_MENU);
      }
      break;
  }
  ///-------------end of state machine -------------------------


  ///----------------- TOAST ----------------------------------
  drawToast();
  
  ///----------------- Pkts per second ------------------------
  if(showPktsPerSec == true)
  {
    display.fillRect(116,57,12,7,WHITE);
    display.setCursor(117,57);
    uint8_t pktRate = returnedByte & 0x3F;
    if(pktRate < 10)
      display.print(F(" "));
    display.print(pktRate);
  }

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

void printVolts(int _milliVolts)
{
  int val = _milliVolts / 10;
  display.print(val / 100);
  display.print(F("."));
  val = val % 100;
  if (val < 10) 
    display.print(F("0"));
  display.print(val);
  display.print(F("V"));
}

//--------------------------------------------------------------------------------------------------

uint8_t incrDecrU8tOnUPDOWN(uint8_t _val, uint8_t _lowerLimit, uint8_t _upperLimit, bool _enableWrap, uint8_t _state)
{
  //Increments/decrements the passed uint8_t value between the specified limit. 
  //If wrap is enabled, wraps around when either limit is reached.

  if(isEditMode == false)
  {
    return _val;
  }

  uint8_t _heldBtn = 0;
  if(_state == PRESSED_ONLY)         _heldBtn = 0;
  else if(_state == PRESSED_OR_HELD) _heldBtn = heldButton;
  else if(_state == SLOW_CHANGE && thisLoopNum % (100 / fixedLoopTime) == 1) _heldBtn = heldButton;

  //Default -- UP_KEY increments, DOWN_KEY decrements
  uint8_t incrKey = UP_KEY;
  uint8_t decrKey = DOWN_KEY;
  // UP_KEY decrements, DOWN_KEY increments 
  if(_lowerLimit > _upperLimit) 
  {
    //swap lower and upper limits
    uint8_t _tmp = _lowerLimit;
    _lowerLimit = _upperLimit;
    _upperLimit = _tmp;
    //swap key actions
    incrKey = DOWN_KEY;
    decrKey = UP_KEY;
  }
   
  //create working variables
  int value = _val; 
  int upperLim = _upperLimit;
  int lowerLim = _lowerLimit;
  int delta = 1;
  if(_heldBtn > 0 && ((millis() - buttonStartTime) > (LONGPRESSTIME + 1000UL)))
  {
    if(_state != SLOW_CHANGE) delta = 2; //speed up increment
  }
  
  //inc dec
  if (pressedButton == incrKey || _heldBtn == incrKey)
  {
    value += delta;
    if(value > upperLim)
    {
      if(_enableWrap) value = lowerLim;
      else value = upperLim;
    }
  }
  else if (pressedButton == decrKey || _heldBtn == decrKey)
  {
    value -= delta;
    if(value < lowerLim)
    {
      if(_enableWrap) value = upperLim;
      else value = lowerLim;
    }
  }

  return value & 0xFF;
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

void plotRateExpo(uint8_t _rate, uint8_t _expo)
{
  display.drawVLine(100, 11, 51, BLACK);
  display.drawHLine(74, 36, 52, BLACK);
  for(int i = 0; i <= 25; i++)
  {
    int _output = calcRateExpo(i * 20, _rate, _expo) / 20;
    display.drawPixel(100 + i, 36 - _output, BLACK);
    display.drawPixel(100 - i, 36 + _output, BLACK);
  }
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
    if ((highlightedItem - topItem) >= 5 || (highlightedItem - topItem) <= 0)
    {
      //Move one step down. If at bottom of list, wrap to top
      topItem += 1;
      if (topItem > (_numMenuItems - 4))
        topItem = 1;
    }
  }
  else if (pressedButton == UP_KEY || _heldBtn == UP_KEY)
  {
    highlightedItem -= 1; //highlight next item
    if (highlightedItem == 0)
      highlightedItem = _numMenuItems; //wrap to last item
    if ((highlightedItem - topItem) < 0 || (highlightedItem - topItem) >= 5)
    {
      //Move one step up .If at very top of list, wrapup to bottom
      topItem -= 1;
      if (topItem == 0)
      {
        topItem = highlightedItem - 4;
        if (_numMenuItems <= 5)
          topItem = 1;
      }
    }
  }

  //------show heading------
  strcpy_P(txtBuff, (char *)pgm_read_word(&list[0]));
  drawHeader();

  //------fill menu slots----
  for (int i = 0; i < 5 && i < _numMenuItems; i++) //5 item slots
  {
    strcpy_P(txtBuff, (char *)pgm_read_word(&list[topItem + i]));
    if (highlightedItem == (topItem + i)) //highlight selection
    {
      display.fillRect(6, 9 + i*11, 116, 11, BLACK);
      display.setTextColor(WHITE);
    }
    display.setCursor(14, 11 + i*11);
    display.println(txtBuff);
    display.setTextColor(BLACK);
  }

  //------draw a simple scroll bar ----
  const int viewPortHeight = 55; //5*11
  int contentHeight = _numMenuItems * 11;
  int contentY = (topItem - 1) * 11;
  int barSize = (viewPortHeight * viewPortHeight) / contentHeight;
  barSize += 1; //Add 1 to compensate for truncation error
  int barYPostn = (viewPortHeight * contentY) / contentHeight;
  display.fillRect(125 , 9 + barYPostn, 1, barSize, BLACK);
}

//--------------------------------------------------------------------------------------------------

void resetThrottleTimer()
{
  throttleTimerElapsedTime = 0;
  throttleTimerLastElapsedTime = 0;
  throttleTimerLastPaused = millis();
}

//--------------------------------------------------------------------------------------------------

void makeToast(const __FlashStringHelper* text, unsigned long _duration)
{
  toastText = text;
  toastExpireTime = millis() + _duration;
}

void drawToast()
{
  if (millis() < toastExpireTime)
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
    strcpy_P(txtBuff, (char *)pgm_read_word(&list[i]));
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

bool isDefaultModelName(char* _nameBuff, uint8_t _len)
{
  /* Checks whether the passed model name is default */
  
  uint8_t _count = 0;
  for(uint8_t i = 0; i < _len - 1; i++)
  {
    if(*(_nameBuff + i) == ' ') //check if it is a space character
      _count++;
  }
  if(_count == (_len - 1)) 
    return true;
  else 
    return false;
}

//--------------------------------------------------------------------------------------------------
uint8_t adjustTrim(uint8_t _decrButton, uint8_t _incrButton, uint8_t _val)
{
  uint8_t _heldBtn = 0;
  uint8_t _holdDelay = 200;
  if(millis() - buttonStartTime > 1200) //speed up
    _holdDelay = 100;
  if(thisLoopNum % (_holdDelay / fixedLoopTime) == 1) 
    _heldBtn = heldButton;
  
  if((pressedButton == _decrButton || _heldBtn == _decrButton) && _val > 75)
  {    
    _val--;
    audioToPlay = AUDIO_SWITCHMOVED;
  }
  else if((pressedButton == _incrButton || _heldBtn == _incrButton) && _val < 125)
  {
    _val++;
    audioToPlay = AUDIO_SWITCHMOVED;
  }
  return _val;
}

//--------------------------------------------------------------------------------------------------

void showTrimData(const __FlashStringHelper* text, uint8_t _trimVal)
{
  display.setCursor(86, 32);
  display.print(text);
  display.setCursor(86, 44);
  if(_trimVal > 100)
    display.print(F("+"));
  display.print(_trimVal - 100);
}

//--------------------------------------------------------------------------------------------------

void loadMix( uint8_t _mixNo, uint8_t _in1, uint8_t _weight1, uint8_t _diff1, uint8_t _offset1,
uint8_t _in2, uint8_t _weight2, uint8_t _diff2, uint8_t _offset2, uint8_t _operator, uint8_t _out)
{
  if(_mixNo >= NUM_MIXSLOTS)
  {
    makeToast(F("Out of slots!"), 2000);
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
  Model.MixOut[_mixNo]        = _out;
}
