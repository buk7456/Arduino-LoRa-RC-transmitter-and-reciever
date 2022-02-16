#ifndef _COMMON_H_
#define _COMMON_H_

void setDefaultSystemParams();
void setDefaultModelName();
void setDefaultModelBasicParams();
void setDefaultModelMixerParams();
void setDefaultModelMixerParams(uint8_t _mixNo);

//====================== MISC =====================================================================

#define NUM_PRP_CHANNLES 9  //Number of proportional channels ## Leave this

//---- Output channels --------------------
extern int channelOut[NUM_PRP_CHANNLES];  //Proportional Channels. Centered at 0, range is -500 to 500.
extern int8_t mixerChOutGraphVals[NUM_PRP_CHANNLES];  //for graphing raw mixer output for channels. range -100 to 100

//---- Sticks -----------------------------
extern int rollIn, pitchIn, throttleIn, yawIn, knobIn; //Scaled stick values, range -500 to 500
extern bool isCalibratingSticks;
extern bool skipThrottleCheck;

//---- Switches ---------------------------
//2 position
extern bool swAEngaged;
extern bool swBEngaged;
extern bool swDEngaged;
extern bool swEEngaged;
extern bool swFEngaged;
//3 position
enum {SWUPPERPOS = 0, SWLOWERPOS = 1, SWMIDPOS = 2};
extern uint8_t swCState; 

//---- Buttons and button events ----------

enum {SELECT_KEY = 1, UP_KEY, DOWN_KEY,};

extern uint8_t buttonCode; 

extern uint32_t buttonStartTime;
extern uint32_t buttonReleaseTime;

//Button events

#define LONGPRESSTIME 350

extern uint8_t pressedButton; //triggered once when the button goes down
extern uint8_t clickedButton; //triggered when the button is released before heldButton event
extern uint8_t heldButton;    //triggered when button is held down long enough

//---- Model timers -----------------------
extern uint32_t timer1ElapsedTime;
extern uint32_t timer1LastElapsedTime;
extern uint32_t timer1LastPaused;

//---- Transmitter Battery -----------------
extern int battVoltsNow; //millivolts

enum{
  BATTLOW = 0, 
  BATTHEALTY
};

extern uint8_t battState;

//---- Audio ------------------------------
enum{  
  AUDIO_NONE = 0, 
  
  AUDIO_BATTERYWARN, 
  AUDIO_THROTTLEWARN, 
  AUDIO_TIMERELAPSED, 
  AUDIO_INACTIVITY, 
  AUDIO_TELEMWARN,
  AUDIO_BIND_SUCCESS,
  AUDIO_TRIM_AIL, 
  AUDIO_TRIM_ELE, 
  AUDIO_TRIM_THR, 
  AUDIO_TRIM_RUD,
  AUDIO_TRIM_MOVED,
  AUDIO_TRIM_MODE_ENTERED,
  AUDIO_TRIM_MODE_EXITED,
  
  AUDIO_SWITCHMOVED,
  
  AUDIO_KEYTONE
};

extern uint8_t audioToPlay;

//---- Misc -------------------------------

extern bool isRequestingBind;
extern uint8_t bindStatusCode;  //1 on success, 2 on fail

extern uint32_t inputsLastMoved; //inactivity detection

extern uint8_t transmitterPacketRate;
extern uint8_t receiverPacketRate;

//---- Telemetry --------------------------
extern uint16_t telem_volts; // in 10mV, sent by receiver with 12bits.  0x0FFF "No data"

//---- Output channel configuration -----
extern uint8_t outputChConfig[9]; 
extern uint8_t maxOutputChConfig[9];
extern bool gotOutputChConfig;
extern bool isRequestingOutputChConfig;
extern bool sendOutputChConfig;
extern uint8_t receiverConfigStatusCode; //1 on success, 2 on fail

//-----------------------------------------
extern uint8_t maxNumOfModels;

//-----------------------------------------
extern bool isAdjustingFuncgenPeriod;

//---- Main loop control -------------------
#define fixedLoopTime 27 
/*in milliseconds. Min 25, Max 30. Should be greater than 
the time taken by radio module to transmit the entire packet or else the window is missed 
resulting in much less throughput. It should also be atleast the time taken to draw the UI, else 
or else the timing becomes inconsistent*/

extern uint32_t thisLoopNum; //main loop counter


//====================== SYSTEM PARAMETERS =========================================================

typedef struct  {
  
  uint8_t activeModel; 
  bool rfOutputEnabled;
  uint8_t rfPower;
  uint8_t inactivityMinutes;
  uint8_t soundMode; 
  uint8_t backlightMode;

  int rollMax, rollMin, rollCenterVal;
  int yawMax, yawMin, yawCenterVal;
  int pitchMax, pitchMin, pitchCenterVal;
  int thrtlMax, thrtlMin;
  uint8_t deadZonePerc; 
  
  bool telemAlarmEnabled;
  bool telemVoltsOnHomeScreen;
  
} sysParams_t;

extern sysParams_t Sys;

enum {
  SOUND_OFF = 0, 
  SOUND_ALARMS, 
  SOUND_NOKEY, 
  SOUND_ALL,
  SOUND_LAST = SOUND_ALL
};

enum {
  BACKLIGHT_OFF = 0, 
  BACKLIGHT_5S, 
  BACKLIGHT_15S, 
  BACKLIGHT_60S, 
  BACKLIGHT_ON,
  BACKLIGHT_LAST = BACKLIGHT_ON
};

enum {
  RFPOWER_3dBm = 0,
  RFPOWER_7dBm,
  RFPOWER_10dBm,
  RFPOWER_14dBm,
  RFPOWER_17dBm,
  RFPOWER_LAST = RFPOWER_17dBm
};

//====================== MODEL PARAMETERS ==========================================================

#define NUM_MIXSLOTS 12     //More slots results into less models and more ram usage 

typedef struct {
  //------- first entity is the modelName ----
  char modelName[7]; //6 chars + Null
  
  //------- basic params ---------
  
  uint16_t reverse;       // each bit represents a channel. 1 is on, 0 is off
  int8_t endpointL[NUM_PRP_CHANNLES];   //left endpoint, -100 to 0
  int8_t endpointR[NUM_PRP_CHANNLES];   //right endpoint, 0 to 100
  int8_t subtrim[NUM_PRP_CHANNLES];     //-20 to 20
  int8_t failsafe[NUM_PRP_CHANNLES];    //-101 to 100. -101 means off

  // Ail, Ele, Rud
  uint8_t dualRate;       //Bit0 Ail, Bit1 Ele, Bit2 Rud  
  int8_t rateNormal[3];   //0 to 100
  int8_t rateSport[3];    //0 to 100
  int8_t expoNormal[3];   //-100 to 100
  int8_t expoSport[3];    //-100 to 100

  int8_t throttlePts[5];  //for interpolation of throttle. Range -100 to 100
  
  int8_t trim[4];         //for Ail, Ele, Thr, Rud inputs. Values -20 to 20
  
  uint8_t slow1Up;        // in tenths of a second
  uint8_t slow1Down;      // in tenths of a second
  uint8_t slow1Src;       // only switches allowed as source
  
  uint8_t funcgenWaveform;
  uint8_t funcgenPeriod;  // in tenths of a second

  uint8_t timer1ControlSrc;
  uint8_t timer1Operator;
  int8_t  timer1Value;    //-100 to 100
  uint8_t timer1InitMins; //if 0, timer will count up, else count down
  
  uint16_t telemVoltsThresh; //as 10mV

  //------- mixer params ---------
  
  uint8_t mixIn1[NUM_MIXSLOTS];  //index in mix sources array
  int8_t mixIn1Offset[NUM_MIXSLOTS]; //-100 to 100
  int8_t mixIn1Weight[NUM_MIXSLOTS]; //-100 to 100
  int8_t mixIn1Diff[NUM_MIXSLOTS];   //-100 to 100
  
  uint8_t mixIn2[NUM_MIXSLOTS];  //index in mix sources array
  int8_t mixIn2Offset[NUM_MIXSLOTS]; //-100 to 100
  int8_t mixIn2Weight[NUM_MIXSLOTS]; //-100 to 100
  int8_t mixIn2Diff[NUM_MIXSLOTS];   //-100 to 100
  
  uint8_t mixOper_N_Switch[NUM_MIXSLOTS]; //upper 2 bits for operator, other 6 bits for the switch

  uint8_t mixOut[NUM_MIXSLOTS];  //index in mix sources array
  
} modelParams_t;

extern modelParams_t Model; 


enum {
  AILRTE = 0, 
  ELERTE, 
  RUDRTE
};

enum {
  GREATER_THAN = 0,
  LESS_THAN,
  ABS_GREATER_THAN,
  ABS_LESS_THAN,
  NUM_TIMER_OPERATORS
};

enum {
  MIX_ADD = 0,
  MIX_MULTIPLY,
  MIX_REPLACE,
  NUM_MIXOPERATORS //should be last
};

enum {
  FUNC_SINE = 0,
  FUNC_SAWTOOTH,
  FUNC_TRIANGLE,
  FUNC_SQUARE,
  NUM_FUNC_WAVEFORMS
};

enum { 
  //Mixer sources. These are indexes in mix sources array
  //Same order as the name order in the UI
  IDX_ROLL = 0, IDX_PITCH, IDX_THRTL_RAW, IDX_YAW, IDX_KNOB, 
  IDX_100PERC, IDX_FUNCGEN,
  IDX_SWA, IDX_SWB, IDX_SWC, IDX_SWD, IDX_SWE, IDX_SWF,
  IDX_SLOW1,
  IDX_AIL, IDX_ELE, IDX_THRTL_CURV, IDX_RUD,
  IDX_NONE, 
  IDX_CH1, IDX_CH2, IDX_CH3, IDX_CH4, IDX_CH5, IDX_CH6, IDX_CH7, IDX_CH8, IDX_CH9,
  IDX_VRT1, IDX_VRT2,
 
  NUM_MIXSOURCES //should be last
};

enum { //possible values in MixSwitch array. Max 64 values
  SW_NONE = 0,
  SWA_UP, SWA_DOWN,
  SWB_UP, SWB_DOWN,
  SWC_UP, SWC_MID, SWC_DOWN, SWC_NOT_UP, SWC_NOT_MID, SWC_NOT_DOWN,
  SWD_UP, SWD_DOWN,
  SWE_UP, SWE_DOWN,
  SWF_UP, SWF_DOWN,
  
  NUM_MIXSWITCHES //should be last
};


#endif
