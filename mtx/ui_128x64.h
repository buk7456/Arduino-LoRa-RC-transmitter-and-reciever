//==================================================================================================
void HandleMainUI(); //main user interface
void DrawEEFormatMsg(); //eeprom format screen
void DrawLoadingMsg(); //loader screen
void DrawThrottleWarning(); //throttle warning screen

//helpers
void detectButtonEvents(); 
void toggleEditModeOnSelectPressed();
void drawAndNavMenu(const char *const list[], int8_t _numMenuItems);
void changeToScreen(int8_t _theScrn);
void plotRateExpo(uint8_t _rate, uint8_t _expo);
void resetThrottleTimer();
void drawHeader();
void printTimeAsHHMMSS(unsigned long _milliSecs, int _cursorX, int _cursorY);
void changeFocusOnUPDOWN(uint8_t _maxItemNo);
void drawCursor(int16_t _xpos, int16_t _ypos);
void makeToast(const __FlashStringHelper* text, unsigned long duration);
void drawToast();
void drawWarning(const __FlashStringHelper* text);
void drawPopupMenu(const char *const list[], int8_t _numItems);
void drawCheckbox(int16_t _xcord, int16_t _ycord, bool _val);
uint8_t incrDecrU8tOnUPDOWN(uint8_t _val, uint8_t _lowerLimit, uint8_t _upperLimit, bool _enableWrap, uint8_t _state);
enum {WRAP = true, NOWRAP = false};
enum {PRESSED_ONLY = 0, PRESSED_OR_HELD = 1, SLOW_CHANGE = 2}; 


///================================================================================================

//-- Main menu strings
#define NUM_ITEMS_MAIN_MENU 7
char const main0[] PROGMEM = "Main menu"; //heading
char const main1[] PROGMEM = "Model";
char const main2[] PROGMEM = "Curves";
char const main3[] PROGMEM = "Mixer";
char const main4[] PROGMEM = "Outputs";
char const main5[] PROGMEM = "System";
char const main6[] PROGMEM = "About";
const char *const mainMenu[] PROGMEM = { //This is the table to refer to the strings
  main0, main1, main2, main3, main4, main5, main6 };

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
  MODE_CALIB,
};

//-- Timer popup menu strings
char const tmrStr0[] PROGMEM = "Start timer 2"; //shown if timer2 is paused
char const tmrStr1[] PROGMEM = "Stop timer 2";  //shown if timer2 is playing
char const tmrStr2[] PROGMEM = "Reset timer 2";
char const tmrStr3[] PROGMEM = "Reset timer 1";
char const tmrStr4[] PROGMEM = "Setup timer 1";
char const tmrStr5[] PROGMEM = "Cancel";
#define NUM_ITEMS_TIMER_MENU 5 
const char *const timerMenuA[] PROGMEM = { //table to refer to the strings
  tmrStr0, tmrStr2, tmrStr3, tmrStr4, tmrStr5 };
const char *const timerMenuB[] PROGMEM = { //table to refer to the strings
  tmrStr1, tmrStr2, tmrStr3, tmrStr4, tmrStr5 };

//-- Mixer popup menu strings
#define NUM_ITEMS_MIXER_MENU 6
char const mxrStr0[] PROGMEM = "View mixes"; 
char const mxrStr1[] PROGMEM = "Reset mix"; 
char const mxrStr2[] PROGMEM = "Move mix to";
char const mxrStr3[] PROGMEM = "Copy mix to";
char const mxrStr4[] PROGMEM = "Reset all mixes";
char const mxrStr5[] PROGMEM = "Cancel";
const char *const mixerMenu[] PROGMEM = { //table to refer to the strings
  mxrStr0, mxrStr1, mxrStr2, mxrStr3, mxrStr4, mxrStr5 };
  
//-- mixer sources name strings. 5 chars max

char const srcName0[]  PROGMEM = "swing"; 
char const srcName1[]  PROGMEM = "roll"; 
char const srcName2[]  PROGMEM = "ptch";
char const srcName3[]  PROGMEM = "thrt";
char const srcName4[]  PROGMEM = "yaw";
char const srcName5[]  PROGMEM = "knob";
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

const char *const srcNames[] PROGMEM = { //This is the table to refer to the strings
  srcName0, srcName1, srcName2, srcName3, srcName4, srcName5, srcName6, srcName7, 
  srcName8, srcName9, srcName10,srcName11, srcName12, srcName13, 
  srcName14,srcName15, srcName16, srcName17, srcName18, srcName19, 
  srcName20, srcName21, srcName22, srcName23 };


// ---------------- Globals ------------------

char txtBuff[22]; //generic buffer for working with strings

uint8_t pressedButton = 0; //button event
uint8_t heldButton = 0;    //button event
unsigned long buttonStartTime = 0;
unsigned long buttonReleasedTime = 0;

int8_t theScreen = HOME_SCREEN;
uint8_t focusedItem = 1; //The item that currently has focus in MODE Screens
bool isEditMode = false;

//Dont use unsigned types for these!!!
int8_t topItem = 1;         //in menu
int8_t highlightedItem = 1; //in menu

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

//toast
const __FlashStringHelper* toastText;
unsigned long toastExpireTime;


//================================== EEPROM messages ===============================================
void DrawEEWarning()
{
  display.clearDisplay();
  drawWarning(F("Bad EEPROM data"));
  display.display();
}

void DrawEEFormatMsg()
{
  display.clearDisplay();
  display.setCursor(9, 26);
  display.print(F("Formatting EEPROM.."));
  display.display();
}

//================================== Loading... message ============================================
void DrawLoadingMsg()
{
  display.clearDisplay();
  display.setCursor(37, 26);
  display.print(F("Loading.."));
  display.display();
}

//================================= Throttle warning message =======================================
void DrawThrottleWarning()
{
  display.clearDisplay();
  drawWarning(F("Check throttle"));
  display.display();
}


///=================================================================================================
///                                 Main user interface 
///=================================================================================================

void HandleMainUI()
{
  /* This function handles the main user interface, allowing us to view, navigate and adjust values, etc.
    Three buttons are used for interaction; select, up, and down. Longpressing select acts as back.
  */

  ///------------ DETECT BUTTON EVENTS -----------------------------
  detectButtonEvents();
  //play keytones on button events
  if(pressedButton != 0 || heldButton != 0)
    audioToPlay = AUDIO_KEYTONE;

  ///------------ THROTTLE TIMER -----------------------------------
  //controlled by throttle stick value. If throttle is above threshold, run, else pause.
  
  int thStpwtch = -500 + (10 * int(throttleThreshold_stopwatch));
  unsigned long timerCountDownInitVal = timerCountDownInitMins * 60000UL;
  if (throttleIn >= thStpwtch && SwAEngaged == false) //run throttle timer
    throttleTimerElapsedTime = throttleTimerLastElapsedTime + millis() - throttleTimerLastPaused;
  else //pause timer
  {
    throttleTimerLastElapsedTime = throttleTimerElapsedTime;
    throttleTimerLastPaused = millis();
  }
  //play audio
  if(timerType == TIMERCOUNTDOWN && throttleTimerElapsedTime > timerCountDownInitVal)
  {
    if((throttleTimerElapsedTime - timerCountDownInitVal) < 1000)
      audioToPlay = AUDIO_TIMERELAPSED;
  }
  
  ///--------------- GENERIC STOPWATCH --------------------
  if(stopwatchIsPaused == false) //play
    stopwatchElapsedTime = stopwatchLastElapsedTime + millis() - stopwatchLastPaused;
  else //pause
  {
    stopwatchLastElapsedTime = stopwatchElapsedTime;
    stopwatchLastPaused = millis();
  }

  /// -------------- LOW BATTERY WARN ----------------------
  if (battState == _BATTLOW_)
  {
    static bool battWarnDismissed = false;
    static unsigned long battWarnDismissTime = 0;
  
    if (pressedButton > 0 && battWarnDismissed == false)
    {
      battWarnDismissed = true;
      battWarnDismissTime = millis();
      pressedButton = 0;
    }
    
    if(battWarnDismissed == true && (millis() - battWarnDismissTime > 600000UL))
      battWarnDismissed = false; //remind low battery 

    if(battWarnDismissed == false)
    {
      display.clearDisplay();
      drawWarning(F("Battery Low"));
      display.display();
      audioToPlay = AUDIO_BATTERYWARN; 
      
      return;
    }
  }
  
  ///----------------- MAIN STATE MACHINE ---------------------------
  switch (theScreen)
  {
    case HOME_SCREEN:
      {
        static bool isExtendedMode = false;
        
        //----------Show a graphical battery gauge---------
        //This crude battery fuel gauge doesnt cater for state of charge measurement...
        //only battery voltage.
        display.drawRect(0, 0, 18, 7, BLACK);
        display.drawLine(18, 2, 18, 4, BLACK);
        
        int numOfBars;
        if(battVoltsNow < BATTV_MIN)
          numOfBars = 0;
        else
        {
          numOfBars = (battVoltsNow - BATTV_MIN) * 5; 
          numOfBars /= BATTV_MAX - BATTV_MIN;
          numOfBars += 1;
          if(numOfBars > 5)
            numOfBars = 5;
        }

        static uint8_t lastNumOfBars = 5; 
        if (numOfBars > lastNumOfBars) 
          numOfBars = lastNumOfBars;
        else 
          lastNumOfBars = numOfBars;
        
        for(int i = 0; i < numOfBars; i++)
          display.fillRect(2 + i*3, 2, 2, 3, BLACK);
        
        //---------show cut icon -----------
        if (SwAEngaged == true)
          display.drawBitmap(63, 1, cut_icon, 13, 6, 1);
        
        //---------show dualrate icon --------
        if (SwBEngaged && (DualRateEnabled[AILRTE] || DualRateEnabled[ELERTE] || DualRateEnabled[RUDRTE]))
          display.drawBitmap(79, 1, dualrate_icon, 13, 6, 1);
        
        //--------show rf icon------------
        if (rfModuleEnabled == true)
          display.drawBitmap(97, 0, rf_icon, 7, 7, 1);
        
        //--------show mute icon------------
        if (soundMode == SOUND_OFF)
          display.drawBitmap(41, 0, mute_icon, 7, 7, 1);

        //------show model name-----------
        display.setCursor(20, 16);
        strcpy_P(txtBuff, PSTR("        "));
        if(strcmp(modelName, txtBuff) == 0) //if modelName not specified
        {
          display.print(F("MODEL"));
          display.print(activeModel);
        }
        else
          display.print(modelName);
        
        // draw separator
        display.drawLine(20,27,103,27,BLACK);

        //----show throttle timer---------
        display.drawBitmap(13, 33, pow_icon, 4, 5, 1);
        if(timerType == TIMERCOUNTUP)
          printTimeAsHHMMSS(throttleTimerElapsedTime, 20, 32);
        else if(timerType == TIMERCOUNTDOWN)
        {
          unsigned long timerCountDownInitVal = timerCountDownInitMins * 60000UL;
          if(throttleTimerElapsedTime < timerCountDownInitVal)
          {
            unsigned long ttqq = timerCountDownInitVal - throttleTimerElapsedTime;
            printTimeAsHHMMSS(ttqq + 999, 20, 32); /*add 999ms so the displayed time doesnt 
            change immediately upon running the timer*/
          }
          else
          {
            unsigned long ttqq = throttleTimerElapsedTime - timerCountDownInitVal;
            if(ttqq >= 1000) //prevents displaying -00:00
            { 
              display.setCursor(20, 32);
              display.print(F("-"));
              printTimeAsHHMMSS(ttqq, 26, 32);
            }
            else
              printTimeAsHHMMSS(ttqq, 20, 32);
          }
        }

        //-------show generic timer ------------
        printTimeAsHHMMSS(stopwatchElapsedTime, 20, 45);

        //---- Extended mode. Show digital channels -----
        if(isExtendedMode == true)
        {
          DigChA = (pressedButton == UP_KEY || heldButton == UP_KEY)? 1 : 0;
          if (pressedButton == SELECT_KEY || heldButton == SELECT_KEY)
          {
            DigChB = !DigChB;
            heldButton = 0; //prevents false toggles
          }

          //show on lcd
          bool _outChAB[2] = {DigChA, DigChB};
          for (int i = 0; i < 2; i++)
          {
            if (_outChAB[i])
            {
              display.fillRect(109, 28 * i, 18, 11, BLACK);
              display.setTextColor(WHITE);
            }
            else
              display.drawRect(109, 28 * i, 18, 11, BLACK);
            display.setCursor(113, 2 + 28 * i);
            display.print(9 + i);
            display.setTextColor(BLACK);
          }
          
          //show lock icon
          display.drawBitmap(53, 0, lock_icon, 6, 7, 1); 
        }
        
        //--------Handle other key presses------------
        if (pressedButton == DOWN_KEY)
          changeToScreen(POPUP_TIMER_MENU);
        else if (pressedButton == SELECT_KEY && isExtendedMode == false)
          changeToScreen(MAIN_MENU);
        else if (heldButton == DOWN_KEY)
        {
          audioToPlay = AUDIO_NONE; //overide
          if(((millis() - buttonStartTime) > (LONGPRESSTIME + 400UL)))
          {
            audioToPlay = AUDIO_KEYTONE;
            isExtendedMode = !isExtendedMode;
            heldButton = 0;
          }
        }
        
      }
      break;
      
    case POPUP_TIMER_MENU:
      {
        changeFocusOnUPDOWN(5);

        if(stopwatchIsPaused)
          drawPopupMenu(timerMenuA, NUM_ITEMS_TIMER_MENU);
        else 
          drawPopupMenu(timerMenuB, NUM_ITEMS_TIMER_MENU);
        
        uint8_t _selection = pressedButton == SELECT_KEY ? focusedItem : 0;
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
        else if(_selection == 5|| heldButton == SELECT_KEY) //exit
          changeToScreen(HOME_SCREEN);
      }
      break;
      
    case MODE_TIMER_SETUP:
      {
        strcpy_P(txtBuff, PSTR("Timer 1"));
        drawHeader();
      
        display.setCursor(1, 10);
        display.print(F("Throttle >=  "));
        display.print(throttleThreshold_stopwatch);
        display.print(F("%"));
        
        display.setCursor(1, 19);
        display.print(F("Timer type:  "));
        if(timerType == TIMERCOUNTDOWN)
          display.print(F("CntDn"));
        else if(timerType == TIMERCOUNTUP)
          display.print(F("CntUp"));
        
        uint8_t _maxFocusableItems = 2;
        
        if(timerType == TIMERCOUNTDOWN)
        {
          _maxFocusableItems = 3;
          display.setCursor(31, 28);
          display.print(F("Start:  "));
          display.print(timerCountDownInitMins);
          display.print(F(" min"));
        }
      
        changeFocusOnUPDOWN(_maxFocusableItems);
        toggleEditModeOnSelectPressed();
        drawCursor(71, (focusedItem * 9) + 1);
        
        if (focusedItem == 1)
          throttleThreshold_stopwatch = incrDecrU8tOnUPDOWN(throttleThreshold_stopwatch, 0, 100, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 2)
          timerType = incrDecrU8tOnUPDOWN(timerType, TIMERCOUNTUP, TIMERCOUNTDOWN, WRAP, PRESSED_ONLY);
        else if(focusedItem == 3)
          timerCountDownInitMins = incrDecrU8tOnUPDOWN(timerCountDownInitMins, 1, 240, NOWRAP, PRESSED_OR_HELD);
      
        if (heldButton == SELECT_KEY)
        {
          eeUpdateModelBasicData(activeModel);
          changeToScreen(HOME_SCREEN);
        }
      }
      break;

    case MAIN_MENU:
      {
        drawAndNavMenu(mainMenu, NUM_ITEMS_MAIN_MENU);
        if (pressedButton == SELECT_KEY)
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

        enum{LOADMODEL = 1, COPYMODEL, RENAMEMODEL, RESETMODEL, DELETEMODEL};
        static uint8_t _action_ = LOADMODEL;
        static uint8_t _thisMdl_ = activeModel;

        display.setCursor(49, 12);
        if (_action_ == LOADMODEL)
          display.print(F("Load"));
        else if (_action_ == COPYMODEL)
          display.print(F("Copy from"));
        else if (_action_ == RENAMEMODEL)
          display.print(F("Rename"));
        else if (_action_ == RESETMODEL)
          display.print(F("Reset"));
        else if (_action_ == DELETEMODEL)
          display.print(F("Delete"));
        
        if(_action_ == LOADMODEL || _action_ == COPYMODEL)
        {
          display.setCursor(49, 22);
          display.print(_thisMdl_);
          eeReadModelName(_thisMdl_);
          display.setCursor(61, 22);
          display.print(modelName);
        }
        else
        {
          display.setCursor(49, 22);
          eeReadModelName(activeModel);
          strcpy_P(txtBuff, PSTR("        "));
          if(strcmp(modelName, txtBuff) == 0) 
          { 
            //if modelName not specified
            display.print(F("MODEL"));
            display.print(activeModel);
          }
          else
            display.print(modelName);
        }

        display.setCursor(49, 32);
        display.print(F("Confirm"));

        changeFocusOnUPDOWN(3);
        toggleEditModeOnSelectPressed();
        drawCursor(41, (focusedItem * 10) + 2);
        
        if (focusedItem == 1) 
          _action_ = incrDecrU8tOnUPDOWN(_action_, 1, 5, WRAP, PRESSED_ONLY);
        else if (focusedItem == 2 && (_action_ == LOADMODEL || _action_ == COPYMODEL))
          _thisMdl_ = incrDecrU8tOnUPDOWN(_thisMdl_, 1, 5, WRAP, SLOW_CHANGE);
        else if (focusedItem == 3 && isEditMode) //confirm action
        {
          if(_action_ == RENAMEMODEL)
          {
            _action_ = LOADMODEL; //reinit
            _thisMdl_ = activeModel; //reinit
            eeReadModelName(activeModel);
            changeToScreen(POPUP_RENAME_MODEL);
          }
          else
          {
            if(_action_ == LOADMODEL)
            {
              makeToast(F("Loaded"), 1500);
              activeModel = _thisMdl_;
              eeReadModelName(activeModel);
            }
            else if(_action_ == COPYMODEL)
            {
              makeToast(F("Copied"), 1500);
              eeCopyModel(_thisMdl_, activeModel);
            }
            else if(_action_ == RESETMODEL)
            {
              makeToast(F("Reset"), 1500);
              eeCopyModel(6, activeModel); 
            }
            else if(_action_ == DELETEMODEL)
            {
              makeToast(F("Deleted"), 1500);
              eeCopyModel(6, activeModel);
              eeReadModelName(6);
              eeUpdateModelName(activeModel);
            }
              
            eeReadModelBasicData(activeModel);
            eeReadMixData(activeModel);
            rfModuleEnabled = false;
            resetThrottleTimer();
            eeUpdateSysConfig();
            _action_ = LOADMODEL;
            _thisMdl_ = activeModel;
            eeReadModelName(activeModel);
            changeToScreen(HOME_SCREEN);
          }
        }

        if (heldButton == SELECT_KEY)
        {
          _action_ = LOADMODEL;
          _thisMdl_ = activeModel;
          eeReadModelName(activeModel);
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case POPUP_RENAME_MODEL:
      {
        display.drawRect(15,11,97,40,BLACK); //draw bounding box
        
        display.setCursor(19,14);
        display.print(F("Rename Model"));
        display.print(activeModel); 
        display.setCursor(19,23);
        display.print(F("Name:  "));
        display.print(modelName);
        
        isEditMode = true;

        static uint8_t charPos = 0;
        uint8_t thisChar = *(modelName+charPos) ;
        
        if(thisChar == 32) thisChar = 0; //map ascii 32 (space) to 0
        else if(thisChar >= 65 && thisChar <= 90) thisChar -= 64; //map ascii 65..90 to 1..26
        else if(thisChar >= 45 && thisChar <= 57) thisChar -= 18; //map ascii 45..57 to 27..39

        thisChar = incrDecrU8tOnUPDOWN(thisChar, 0, 39, NOWRAP, SLOW_CHANGE);

        //map back
        if(thisChar == 0) thisChar = 32; //map 0 to ascii 32 (space)
        else if(thisChar >= 1 && thisChar <= 26) thisChar += 64; //map 1..26 to ascii 65..90
        else if(thisChar >= 27 && thisChar <= 39) thisChar += 18; //map 27..39 to ascii 45..57

        //write
        *(modelName+charPos) = thisChar;
        
        //draw blinking cursor
        if ((millis() - buttonReleasedTime) % 1000 < 500 || heldButton > 0)
          display.fillRect(61 + 6*charPos, 31, 5, 2, BLACK);
        
        //change to next character
        if(pressedButton == SELECT_KEY)
          charPos++;
        else if(heldButton == SELECT_KEY && charPos > 0)
        {
          charPos--;
          heldButton = 0; //override. prevents false triggers
        }
          
        if(charPos == (sizeof(modelName)/sizeof(modelName[0])) - 1) //done renaming. Exit
        {
          charPos = 0;
          eeUpdateModelName(activeModel);
          changeToScreen(HOME_SCREEN); 
          makeToast(F("Done"), 1500);
        }
      }
      break;

    case MODE_CURVES:
      {
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_CURVES])));
        drawHeader();

        enum{AIL_CURVE = 0, ELE_CURVE = 1, RUD_CURVE = 2, THR_CURVE = 3};
        static uint8_t displayedCurve = AIL_CURVE;
        
        if (focusedItem == 1) //switch to another curve
          displayedCurve = incrDecrU8tOnUPDOWN(displayedCurve, AIL_CURVE, THR_CURVE, WRAP, PRESSED_ONLY);
          
        uint8_t _maxFocusableItems = 1;
    
        /////////////////RATES AND EXPO////////////////////////////////////////
        if(displayedCurve < THR_CURVE)  
        {  
          _maxFocusableItems = 4;
          
          uint8_t theRates[2] = {0, 0};
          uint8_t theExpo[2] = {0, 0};
          //-----Pass parameters to the arrays for rates and expo-----
          if (displayedCurve == AIL_CURVE)
          {
            theRates[0] = RateNormal[AILRTE];
            theRates[1] = RateSport[AILRTE];
            theExpo[0] = ExpoNormal[AILRTE];
            theExpo[1] = ExpoSport[AILRTE];
          }
          else if (displayedCurve == ELE_CURVE)
          {
            theRates[0] = RateNormal[ELERTE];
            theRates[1] = RateSport[ELERTE];
            theExpo[0] = ExpoNormal[ELERTE];
            theExpo[1] = ExpoSport[ELERTE];
          }
          else if (displayedCurve == RUD_CURVE)
          {
            theRates[0] = RateNormal[RUDRTE];
            theRates[1] = RateSport[RUDRTE];
            theExpo[0] = ExpoNormal[RUDRTE];
            theExpo[1] = ExpoSport[RUDRTE];
          }
          
          //-----Adjudt values on key presses----
          if (focusedItem == 2) //adjust rate
          {
            if(SwBEngaged == false || DualRateEnabled[displayedCurve] == false)
              theRates[0] = incrDecrU8tOnUPDOWN(theRates[0], 0, 100, NOWRAP, PRESSED_OR_HELD); 
            else
              theRates[1] = incrDecrU8tOnUPDOWN(theRates[1], 0, 100, NOWRAP, PRESSED_OR_HELD); 
          }
          else if (focusedItem == 3) //adjust expo
          {
            if(SwBEngaged == false || DualRateEnabled[displayedCurve] == false)
              theExpo[0] = incrDecrU8tOnUPDOWN(theExpo[0], 0, 200, NOWRAP, PRESSED_OR_HELD);
            else 
              theExpo[1] = incrDecrU8tOnUPDOWN(theExpo[1], 0, 200, NOWRAP, PRESSED_OR_HELD);
          }
          else if (focusedItem == 4) //toggle dualrate
            DualRateEnabled[displayedCurve] = incrDecrU8tOnUPDOWN(DualRateEnabled[displayedCurve],0,1,WRAP,PRESSED_ONLY);
        
          
          //------ Write the values ------
          if (displayedCurve == AIL_CURVE)
          {
            RateNormal[AILRTE] = theRates[0];
            RateSport[AILRTE] = theRates[1];
            ExpoNormal[AILRTE] = theExpo[0];
            ExpoSport[AILRTE] = theExpo[1];
          }
          else if (displayedCurve == ELE_CURVE)
          {
            RateNormal[ELERTE] = theRates[0];
            RateSport[ELERTE] = theRates[1];
            ExpoNormal[ELERTE] = theExpo[0];
            ExpoSport[ELERTE] = theExpo[1];
          }
          else if (displayedCurve == RUD_CURVE)
          {
            RateNormal[RUDRTE] = theRates[0];
            RateSport[RUDRTE] = theRates[1];
            ExpoNormal[RUDRTE] = theExpo[0];
            ExpoSport[RUDRTE] = theExpo[1];
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
          if(SwBEngaged == false || DualRateEnabled[displayedCurve] == false)
            _datIDX = 0; 
          
          display.setCursor(0, 20);
          display.print(F("Rate:  "));
          display.print(theRates[_datIDX]);
          display.print(F("%"));
          display.setCursor(0, 29);
          display.print(F("Expo:  "));
          display.print(theExpo[_datIDX] - 100);
          display.print(F("%"));
          
          display.setCursor(0, 38);
          display.print(F("D/R :  "));
          drawCheckbox(42, 38, DualRateEnabled[displayedCurve]);

          if( SwBEngaged == true 
              && ((displayedCurve == AIL_CURVE && DualRateEnabled[AILRTE] == true)
                   || (displayedCurve == ELE_CURVE && DualRateEnabled[ELERTE] == true)
                   || (displayedCurve == RUD_CURVE && DualRateEnabled[RUDRTE] == true)))
          { 
            display.fillRect(0,47,33,9,BLACK);
            display.setCursor(2,48);
            display.setTextColor(WHITE);
            display.print(F("Sport"));
            display.setTextColor(BLACK);
          }

          //draw graph 
          plotRateExpo(theRates[_datIDX], theExpo[_datIDX]);
          //draw stick input marker
          int qq = calcRateExpo(_stickInpt, theRates[_datIDX], theExpo[_datIDX]) / 20;
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
            ThrottlePts[_idx] = incrDecrU8tOnUPDOWN(ThrottlePts[_idx], 0, 200, NOWRAP, PRESSED_OR_HELD);
          }
   
          display.setCursor(0,20);
          display.print(F("Pt"));
          for (int i = 0; i < 5; i++)
          {
            display.setCursor(18, 20 + i * 9);
            display.write(101 - i); //e,d,c,b,a
            display.print(F(":  "));
            display.print(ThrottlePts[4 - i] - 100);
          }
          
          //----Show graph 
          
          //draw the axes
          display.drawLine(100, 11, 100, 61, BLACK);
          display.drawLine(74, 36, 125, 36, BLACK);
          
          //Interpolate and draw points. We use x cordinate to estimate corresponding y cordinate
          //Actual plot area is 50x50.
          int xpts[5] = {0, 250, 500, 750, 1000};
          int ypts[5];
          for(int i = 0; i < 5; i++)
            ypts[i] = (int)ThrottlePts[i] * 5;
                            
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
        toggleEditModeOnSelectPressed();
        drawCursor(34, (focusedItem * 9) + 2);

        ////// Exit
        if (heldButton == SELECT_KEY)
        {
          eeUpdateModelBasicData(activeModel);
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
        int _outNameIndex = MixOut[thisMixNum];
        strcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_outNameIndex])));
        display.print(txtBuff);
        
        display.setCursor(12, 24);
        display.print(F("Input:  "));
        int _In1NameIndex = MixIn1[thisMixNum];
        strcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_In1NameIndex])));
        display.print(txtBuff);
        
        display.setCursor(97, 24);
        int _In2NameIndex = MixIn2[thisMixNum];
        strcpy_P(txtBuff, (char *)pgm_read_word(&(srcNames[_In2NameIndex])));
        display.print(txtBuff);
        
        display.setCursor(6, 32);
        display.print(F("Weight:  "));
        display.print(MixIn1Weight[thisMixNum] - 100);
        display.print(F("%"));
        display.setCursor(97, 32);
        display.print(MixIn2Weight[thisMixNum] - 100);
        display.print(F("%"));
        
        display.setCursor(18, 40);
        display.print(F("Diff:  "));
        display.print(MixIn1Diff[thisMixNum] - 100);
        display.print(F("%"));
        display.setCursor(97, 40);
        display.print(MixIn2Diff[thisMixNum] - 100);
        display.print(F("%"));
        
        display.setCursor(6, 48);
        display.print(F("Offset:  "));
        display.print(MixIn1Offset[thisMixNum] - 100);
        display.setCursor(97, 48);
        display.print(MixIn2Offset[thisMixNum] - 100);
        
        display.setCursor(24, 56);
        display.print(F("Mux:  "));
        if(MixOperator[thisMixNum] == 0)
          display.print(F("Add"));
        else 
          display.print(F("Multiply"));

        changeFocusOnUPDOWN(11);
        toggleEditModeOnSelectPressed();
        if(focusedItem < 8)
          drawCursor(52, focusedItem * 8);
        else
          drawCursor(89, (focusedItem * 8) - 40);
        
        if (focusedItem == 1)     //Change to another mixer slot
          thisMixNum = incrDecrU8tOnUPDOWN(thisMixNum, 0, NUM_MIXSLOTS - 1, WRAP, PRESSED_ONLY);
        else if(focusedItem == 2) //change output
          MixOut[thisMixNum] = incrDecrU8tOnUPDOWN(MixOut[thisMixNum], 9, 23, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 3) //change input 1
          MixIn1[thisMixNum] = incrDecrU8tOnUPDOWN(MixIn1[thisMixNum], 0, 23, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 4) //adjust weight 1
          MixIn1Weight[thisMixNum] = incrDecrU8tOnUPDOWN(MixIn1Weight[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 5) //adjust differential 1
          MixIn1Diff[thisMixNum] = incrDecrU8tOnUPDOWN(MixIn1Diff[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 6) //adjust offset 1
          MixIn1Offset[thisMixNum] = incrDecrU8tOnUPDOWN(MixIn1Offset[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 7) //change operator
          MixOperator[thisMixNum] = incrDecrU8tOnUPDOWN(MixOperator[thisMixNum], 0, 1, WRAP, PRESSED_ONLY);
        else if(focusedItem == 8) //change input 2
          MixIn2[thisMixNum] = incrDecrU8tOnUPDOWN(MixIn2[thisMixNum], 0, 23, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 9) //adjust weight 2
          MixIn2Weight[thisMixNum] = incrDecrU8tOnUPDOWN(MixIn2Weight[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 10) //adjust differential 2
          MixIn2Diff[thisMixNum] = incrDecrU8tOnUPDOWN(MixIn2Diff[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        else if(focusedItem == 11) //adjust offset 2
          MixIn2Offset[thisMixNum] = incrDecrU8tOnUPDOWN(MixIn2Offset[thisMixNum], 0, 200, NOWRAP, PRESSED_OR_HELD);
        
        /// --- Popup if up or down key held
        if(isEditMode == false && (heldButton == UP_KEY || heldButton == DOWN_KEY))
          changeToScreen(POPUP_MIXER_MENU);
        
        if (heldButton == SELECT_KEY)
        {
          eeUpdateMixData(activeModel);
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case POPUP_MIXER_MENU:
      {
        changeFocusOnUPDOWN(NUM_ITEMS_MIXER_MENU);
        
        drawPopupMenu(mixerMenu, NUM_ITEMS_MIXER_MENU);
        
        uint8_t _selection = pressedButton == SELECT_KEY ? focusedItem : 0;
        
        if(_selection == 1) //view outputs
        {
          eeUpdateMixData(activeModel);
          changeToScreen(MODE_MIXER_OUTPUT);
        }
        else if(_selection == 2) //reset this mix
        {
          eeCopyMixFrom(6, thisMixNum); //copy from hidden default model
          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 3) //move mix
        {
          destMixNum = thisMixNum;
          changeToScreen(POPUP_MOVE_MIX);
        }  
        else if(_selection == 4) //copy mix
        {
           destMixNum = thisMixNum;
          changeToScreen(POPUP_COPY_MIX);
        }
        else if(_selection == 5) //reset all mixes
        {
          for(int _thisMix = 0; _thisMix < NUM_MIXSLOTS; _thisMix++)
            eeCopyMixFrom(6, _thisMix); //copy from hidden default model

          thisMixNum = 0;
          destMixNum = 0;
          changeToScreen(MODE_MIXER);
        }
        else if(_selection == 6 || heldButton == SELECT_KEY) //exit
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
        for (int i = 0; i < 8; i++)
        {
          int _outVal = ChOutMixer[i] / 5;
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
          display.drawLine(8, 33, 119, 33, BLACK);
          //Show channel numbers
          display.setCursor(16 + _xOffset,56);
          display.print(i+1);
        }

        if(heldButton == SELECT_KEY || pressedButton == SELECT_KEY)
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
        display.setCursor(67,41);
        display.print(F("Move"));
        
        changeFocusOnUPDOWN(2);
        toggleEditModeOnSelectPressed();
        drawCursor(59, (focusedItem*18) + 5);
        
        if(focusedItem == 1)
          destMixNum = incrDecrU8tOnUPDOWN(destMixNum, 0, NUM_MIXSLOTS - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 2 && pressedButton == SELECT_KEY)
        {
          uint8_t oldPostn = thisMixNum;
          uint8_t newPostn = destMixNum;
          
          //If we have an array [a,b,c,d,e] and we would like to move c to a's position,
          //we achieve this by temporarily storing c, shifting b to c's position, then shifting
          //a to b's position, then writing our temp stored c to the new position. 
          //Likewise if we wanted a to assume c's position, we temporarily store a, shift b to a's
          //position, shift c to b's position, then write our temp stored a to the new position. 
        
          //first store temporarily the old position's data
          uint8_t _mixout      =   MixOut[oldPostn];
          uint8_t _mix1in      =   MixIn1[oldPostn];
          uint8_t _mix1weight  =   MixIn1Weight[oldPostn];
          uint8_t _mix1offset1 =   MixIn1Offset[oldPostn];
          uint8_t _mix1diff    =   MixIn1Diff[oldPostn];
          uint8_t _mixOper     =   MixOperator[oldPostn];
          uint8_t _mix2in      =   MixIn2[oldPostn];
          uint8_t _mix2weight  =   MixIn2Weight[oldPostn];
          uint8_t _mix2offset1 =   MixIn2Offset[oldPostn];
          uint8_t _mix2diff    =   MixIn2Diff[oldPostn];
          
          uint8_t thisPostn = oldPostn;
          if(newPostn < oldPostn)
          {
            while(thisPostn > newPostn)
            {
              MixOut[thisPostn]       = MixOut[thisPostn-1];
              MixIn1[thisPostn]       = MixIn1[thisPostn-1];
              MixIn1Weight[thisPostn] = MixIn1Weight[thisPostn-1];
              MixIn1Offset[thisPostn] = MixIn1Offset[thisPostn-1];
              MixIn1Diff[thisPostn]   = MixIn1Diff[thisPostn-1];
              MixOperator[thisPostn]  = MixOperator[thisPostn-1];
              MixIn2[thisPostn]       = MixIn2[thisPostn-1];
              MixIn2Weight[thisPostn] = MixIn2Weight[thisPostn-1];
              MixIn2Offset[thisPostn] = MixIn2Offset[thisPostn-1];
              MixIn2Diff[thisPostn]   = MixIn2Diff[thisPostn-1];
              
              thisPostn--;
            }
          }
          else if(newPostn > oldPostn) 
          {
            while(thisPostn < newPostn)
            {
              MixOut[thisPostn]       = MixOut[thisPostn+1];
              MixIn1[thisPostn]       = MixIn1[thisPostn+1];
              MixIn1Weight[thisPostn] = MixIn1Weight[thisPostn+1];
              MixIn1Offset[thisPostn] = MixIn1Offset[thisPostn+1];
              MixIn1Diff[thisPostn]   = MixIn1Diff[thisPostn+1];
              MixOperator[thisPostn]  = MixOperator[thisPostn+1];
              MixIn2[thisPostn]       = MixIn2[thisPostn+1];
              MixIn2Weight[thisPostn] = MixIn2Weight[thisPostn+1];
              MixIn2Offset[thisPostn] = MixIn2Offset[thisPostn+1];
              MixIn2Diff[thisPostn]   = MixIn2Diff[thisPostn+1];
              
              thisPostn++;
            }
          }
          
          //copy from temporary into new position
          MixOut[newPostn]       = _mixout;     
          MixIn1[newPostn]       = _mix1in;    
          MixIn1Weight[newPostn] = _mix1weight;
          MixIn1Offset[newPostn] = _mix1offset1;
          MixIn1Diff[newPostn]   = _mix1diff;  
          MixOperator[newPostn]  = _mixOper;   
          MixIn2[newPostn]       = _mix2in;    
          MixIn2Weight[newPostn] = _mix2weight; 
          MixIn2Offset[newPostn] = _mix2offset1;
          MixIn2Diff[newPostn]   = _mix2diff;  

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
        display.setCursor(67,41);
        display.print(F("Copy"));
        
        changeFocusOnUPDOWN(2);
        toggleEditModeOnSelectPressed();
        drawCursor(59, (focusedItem * 18) + 5);
        
        if(focusedItem == 1)
          destMixNum = incrDecrU8tOnUPDOWN(destMixNum, 0, NUM_MIXSLOTS - 1, NOWRAP, SLOW_CHANGE);
        else if(focusedItem == 2 && pressedButton == SELECT_KEY)
        {
          MixOut[destMixNum]       = MixOut[thisMixNum];
          MixIn1[destMixNum]       = MixIn1[thisMixNum];
          MixIn1Weight[destMixNum] = MixIn1Weight[thisMixNum];
          MixIn1Offset[destMixNum] = MixIn1Offset[thisMixNum];
          MixIn1Diff[destMixNum]   = MixIn1Diff[thisMixNum];
          MixOperator[destMixNum]  = MixOperator[thisMixNum];
          MixIn2[destMixNum]       = MixIn2[thisMixNum];
          MixIn2Weight[destMixNum] = MixIn2Weight[thisMixNum];
          MixIn2Offset[destMixNum] = MixIn2Offset[thisMixNum];
          MixIn2Diff[destMixNum]   = MixIn2Diff[thisMixNum];
           
          thisMixNum = destMixNum; //display the destination when we go back to mixer screen
          destMixNum = thisMixNum;
          changeToScreen(MODE_MIXER); 
        }

        if(heldButton == SELECT_KEY)
          changeToScreen(MODE_MIXER);
      }
      break;
      
    case MODE_OUTPUTS:
      {
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_OUTPUTS])));
        drawHeader();

        changeFocusOnUPDOWN(7);
        toggleEditModeOnSelectPressed();
        drawCursor(71, focusedItem * 8);
        
        static uint8_t _selectedChannel = 0; //0 is ch1, 1 is ch2, etc.
    
        if (focusedItem == 1)
          _selectedChannel = incrDecrU8tOnUPDOWN(_selectedChannel, 0, 7, WRAP, PRESSED_ONLY); //8 channels
        else if (focusedItem == 2)
          Reverse[_selectedChannel] = incrDecrU8tOnUPDOWN(Reverse[_selectedChannel], 0, 1, WRAP, PRESSED_ONLY);
        else if (focusedItem == 3)
          Subtrim[_selectedChannel] = incrDecrU8tOnUPDOWN(Subtrim[_selectedChannel], 25, 75, NOWRAP, PRESSED_ONLY);
        else if (focusedItem == 4)
          CutValue[_selectedChannel] = incrDecrU8tOnUPDOWN(CutValue[_selectedChannel], 0, 201, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 5)
          Failsafe[_selectedChannel] = incrDecrU8tOnUPDOWN(Failsafe[_selectedChannel], 0, 201, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 6)
          EndpointL[_selectedChannel] = incrDecrU8tOnUPDOWN(EndpointL[_selectedChannel], 0, 100, NOWRAP, PRESSED_OR_HELD);
        else if (focusedItem == 7)
          EndpointR[_selectedChannel] = incrDecrU8tOnUPDOWN(EndpointR[_selectedChannel], 0, 100, NOWRAP, PRESSED_OR_HELD);

        //-------Show on lcd-----------------------
        display.setCursor(49, 8);
        display.print(F("Ch:  "));
        display.print(_selectedChannel + 1);

        display.setCursor(19, 16);
        display.print(F("Reverse:  "));
        drawCheckbox(79, 16, Reverse[_selectedChannel]);

        display.setCursor(19, 24);
        display.print(F("Subtrim:  "));
        int _trmQQ = int(Subtrim[_selectedChannel]) - 50;
        display.print(_trmQQ); //show as centered about 50
        
        display.setCursor(43, 32);
        display.print(F("Cut:  "));
        if(CutValue[_selectedChannel]== 0)
          display.print(F("Off"));
        else
          display.print(CutValue[_selectedChannel] - 101);
        
        display.setCursor(19, 40);
        display.print(F("Failsaf:  "));
        if(Failsafe[_selectedChannel]== 0)
          display.print(F("Off"));
        else
          display.print(Failsafe[_selectedChannel] - 101);

        display.setCursor(25, 48);
        display.print(F("Travel:  "));
        display.print(0 - EndpointL[_selectedChannel]); //show as negative

        display.setCursor(49, 56);
        display.print(F("to:  "));
        display.print(EndpointR[_selectedChannel]);
        
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
        display.setTextColor(BLACK);
        
        if (heldButton == SELECT_KEY)
        {
          eeUpdateModelBasicData(activeModel);
          changeToScreen(MAIN_MENU);
        }
      }
      break;
      
    case MODE_SYSTEM:
      {
        strcpy_P(txtBuff, (char *)pgm_read_word(&(mainMenu[MODE_SYSTEM])));
        drawHeader();

        display.setCursor(7, 10);
        display.print(F("RF module:  "));
        drawCheckbox(79, 10, rfModuleEnabled);
        
        display.setCursor(7, 19);
        display.print(F("Backlight:  "));
        drawCheckbox(79, 19, backlightEnabled);
        
        display.setCursor(31, 28);
        display.print(F("Sound:  "));
        if (soundMode == SOUND_OFF)
          display.print(F("Quiet"));
        else if (soundMode == SOUND_ALERTS)
          display.print(F("Alarm"));
        else if (soundMode == SOUND_ALERTS_SWITCHES)
          display.print(F("NoKey"));
        else if (soundMode == SOUND_ALL)
          display.print(F("All"));

        display.setCursor(25, 37);
        display.print(F("Sticks:  "));
        display.print(F("Calib?"));

        changeFocusOnUPDOWN(4);
        toggleEditModeOnSelectPressed();
        drawCursor(71, 10 + (focusedItem - 1) * 9);
        
        if (focusedItem == 1)
          rfModuleEnabled = incrDecrU8tOnUPDOWN(rfModuleEnabled, 0, 1, WRAP, PRESSED_ONLY);
        else if (focusedItem == 2)
          backlightEnabled = incrDecrU8tOnUPDOWN(backlightEnabled, 0, 1, WRAP, PRESSED_ONLY);
        else if (focusedItem == 3)
          soundMode = incrDecrU8tOnUPDOWN(soundMode, 0, 3, WRAP, PRESSED_ONLY);
        else if (focusedItem == 4 && pressedButton == SELECT_KEY)
          changeToScreen(MODE_CALIB);

        if (heldButton == SELECT_KEY)
        {
          eeUpdateSysConfig();
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
          rollMin = 1023;  rollMax = 0;
          pitchMin = 1023; pitchMax = 0;
          yawMin = 1023;   yawMax = 0;
          thrtlMin = 1023; thrtlMax = 0;

          calibInitialised = true;
        }

        display.setCursor(24, 16);
        display.print(F("[OK] when done"));
        
        if(calibStage != STICKS_DEADZONE)
        {
          display.setCursor(37, 32);
          display.print(F("roll:"));
          display.setCursor(37, 40);
          display.print(F("ptch:"));
          display.setCursor(37, 56);
          display.print(F("yaw :"));
        }
        
        if (calibStage == STICKS_MOVE) 
        {
          display.setCursor(37, 48);
          display.print(F("thrt:"));

          display.setCursor(13, 8);
          display.print(F("Move sticks fully"));

          //get min and max
          int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
          for (int i = 0; i < 10; i++) //read in 10 samples per stick
          {
            x1 += analogRead(ROLLINPIN);
            x2 += analogRead(YAWINPIN);
            y1 += analogRead(PITCHINPIN);
            y2 += analogRead(THROTTLEINPIN);
            delay(1);
          }
          x1 = x1 / 10;
          x2 = x2 / 10;
          y1 = y1 / 10;
          y2 = y2 / 10;

          if (x1 < rollMin)
            rollMin = x1;
          else if (x1 > rollMax)
            rollMax = x1;
          if (x2 < yawMin)
            yawMin = x2;
          else if (x2 > yawMax)
            yawMax = x2;
          if (y1 < pitchMin)
            pitchMin = y1;
          else if (y1 > pitchMax)
            pitchMax = y1;
          if (y2 < thrtlMin)
            thrtlMin = y2;
          else if (y2 > thrtlMax)
            thrtlMax = y2;

          //show data
          int _theMinMax[10] = { rollMin, rollMax, pitchMin, pitchMax, thrtlMin, thrtlMax, yawMin, yawMax };
          for (int i = 0; i < 10; i += 2)
          {
            display.setCursor(73, 32 + ((i * 8) / 2));
            display.print(_theMinMax[i]);
            display.print(F(","));
            display.print(_theMinMax[i + 1]);
          }

          if (pressedButton == SELECT_KEY)
            calibStage = STICKS_CENTER;
        }
        
        else if (calibStage == STICKS_CENTER)
        {
          display.setCursor(25, 8);
          display.print(F("Center sticks"));

          //get stick centers
          int x1 = 0, x2 = 0, y1 = 0;
          for (int i = 0; i < 8; i++) //read in 8 samples
          {
            x1 += analogRead(ROLLINPIN);
            x2 += analogRead(YAWINPIN);
            y1 += analogRead(PITCHINPIN);
            delay(1);
          }
          rollCenterVal = x1 / 8;
          yawCenterVal = x2 / 8;
          pitchCenterVal = y1 / 8;
          
          //show data
          int _theCenters[4] = {rollCenterVal, pitchCenterVal, 512, yawCenterVal};
          for (int i = 0; i < 4; i += 1)
          {
            if(i == 2) i = 3; //skip as throttle stick center doesnt make sense
            display.setCursor(73, 33 + i * 8);
            display.print(_theCenters[i]);
          }

          if (pressedButton == SELECT_KEY)
          {
            //Add slight deadband(about 1.5%) at each stick ends to stabilise readings at ends
            //For a range of 0 to 5V, min max are 0.07V and 4.92V
            int ddznQQ = (rollMax - rollMin) / 64;
            rollMax -= ddznQQ;
            rollMin += ddznQQ;

            ddznQQ = (pitchMax - pitchMin) / 64;
            pitchMax -= ddznQQ;
            pitchMin += ddznQQ;

            ddznQQ = (thrtlMax - thrtlMin) / 64;
            thrtlMax -= ddznQQ;
            thrtlMin += ddznQQ;

            ddznQQ = (yawMax - yawMin) / 64;
            yawMax -= ddznQQ;
            yawMin += ddznQQ;
            
            calibStage = STICKS_DEADZONE;
          }
        }
        
        else if (calibStage == STICKS_DEADZONE)
        {
          display.setCursor(20, 8);
          display.print(F("Adjust deadzone"));
          
          isEditMode = true;
          drawCursor(52, 34);
          deadZonePerc = incrDecrU8tOnUPDOWN(deadZonePerc, 0, 15, NOWRAP, PRESSED_OR_HELD);
          display.setCursor(59,34);
          display.print(deadZonePerc);
          display.print(F("%"));
          isEditMode = false;
          
          display.setCursor(11, 56);
          display.print(F("(roll, pitch, yaw)"));

          if (pressedButton == SELECT_KEY) //exit
          {
            isCalibratingSticks = false;
            calibInitialised = false;
            calibStage = STICKS_MOVE;
            eeUpdateSysConfig();
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
        display.setCursor(25,10);
        display.print(F("Batt   "));
        int _battV = battVoltsNow / 10;
        display.print(_battV / 100);
        display.print(F("."));
        _battV = _battV % 100;
        if (_battV < 10)
          display.print(F("0"));
        display.print(_battV);
        display.print(F("V"));

        //Show uptime
        display.setCursor(25,19);
        display.print(F("Uptime "));
        printTimeAsHHMMSS(millis(), 67, 19);
        
        //show sketch version and date
        display.setCursor(25, 28);
        display.print(F("Ver    "));
        display.print(F(_SKETCHVERSION));

        // show author contact
        display.setCursor(25, 37);
        display.print(F("Dev    "));
        display.print(F("buk7456"));
        display.setCursor(67, 46);
        display.print(F("@gmail.com"));
        
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
    display.fillRect(116,0,12,7,WHITE);
    display.setCursor(117,0);
    uint8_t pktRate = returnedByte & 0x3F;
    if(pktRate < 10)
      display.print(F(" "));
    display.print(pktRate);
  }
  
  ///----------------- SHOW ON PHYSICAL LCD -------------------
  display.display(); //show on real lcd
  display.clearDisplay(); //clear memory buffer
}

//================================= Helpers ========================================================

void detectButtonEvents()
{
  ///------ DETECT BUTTON PRESSES AND HOLDS -------
  /// Modifies the pressedButton and heldButton variables, buttonStartTime and buttonReleasedTime
  
  /*For the held button, we need to detect it without triggering that a button was pressed first.
    So we need to trigger Pressed after the button has been released, but not on long press*/
    
  static uint8_t lastButtonCode = 0;
  static bool buttonActive = false, longPressActive = false;
  
  if (buttonCode > 0 && (millis() - buttonReleasedTime > 100)) //button down  
  {
    if (buttonActive == false)
    {
      buttonActive = true;
      buttonStartTime = millis();
      lastButtonCode = buttonCode;
    }

    if ((millis() - buttonStartTime > LONGPRESSTIME) && longPressActive == false)
    {
      longPressActive = true;
      heldButton = buttonCode;
    }
  }

  else //button released
  {
    if(buttonActive == true)
    {
      buttonReleasedTime = millis();
      buttonActive = false;
    }
    
    if (longPressActive == true)
    {
      longPressActive = false;
      heldButton = 0;
      lastButtonCode = 0; //avoids falsely triggering pressedButton event
    }
    else
    {
      pressedButton = lastButtonCode;
      lastButtonCode = 0; //enables setting pressedButton event only once
    }
  }
}

//--------------------------------------------------------------------------------------------------
void toggleEditModeOnSelectPressed()
{
  if (pressedButton == SELECT_KEY)
    isEditMode = !isEditMode;
}

//--------------------------------------------------------------------------------------------------
void printTimeAsHHMMSS(unsigned long _milliSecs, int _cursorX, int _cursorY)
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
  else if(_state == SLOW_CHANGE && thisLoopNum % 4 == 1) _heldBtn = heldButton;

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
  if(isEditMode == false)
  { 
    isEditMode = true; //overide
    focusedItem = incrDecrU8tOnUPDOWN(focusedItem, _maxItemNo, 1, WRAP, PRESSED_ONLY);
    isEditMode = false; 
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
    if ((millis() - buttonReleasedTime) % 1000 < 500 || heldButton > 0)
      display.fillRect(_xpos + 3, _ypos - 1, 2, 9, BLACK);
  }
  else 
    display.drawBitmap(_xpos, _ypos, point, 6, 7, 1); //draw arrow ->
}

//--------------------------------------------------------------------------------------------------
void plotRateExpo(uint8_t _rate, uint8_t _expo)
{
  display.drawLine(100, 11, 100, 61, BLACK);
  display.drawLine(74, 36, 125, 36, BLACK);
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
  display.drawLine(0, 3, headingX_offset - 2, 3, BLACK);
  display.drawLine(headingX_offset + _txtWidthPix, 3, 127, 3, BLACK);
}

//--------------------------------------------------------------------------------------------------
void drawAndNavMenu(const char *const list[], int8_t _numMenuItems)
{
  _numMenuItems -= 1; //exclude menu heading in count
  
  //------handle menu navigation (up and down keys)------
  if (pressedButton == DOWN_KEY)
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
  else if (pressedButton == UP_KEY)
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
void drawWarning(const __FlashStringHelper* text)
{
  display.drawBitmap(56, 17, warningTriangle, 16, 15, BLACK);
  int _txtWidthPix = 6 * strlen_P((const char *)text); //(const char*) casts
  int x_offset = (display.width() - _txtWidthPix) / 2; //middle align
  display.setCursor(x_offset, 40);
  display.print(text);
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
