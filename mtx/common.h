
void setDefaultModelName();
void setDefaultModelBasicParams();
void setDefaultModelMixerParams();
void setDefaultModelMixerParams(uint8_t _mixNo);

//------------------- SYSTEM PARAMS ----------------------------------------------------------------

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

/* Structure for system parameters
*/
struct sysParams {
  
  uint8_t activeModel = 1; //The current model
  
  bool rfOutputEnabled = false;
  uint8_t rfPower = RFPOWER_10dBm;
  
  uint8_t inactivityMinutes = 10;
  
  uint8_t soundMode = SOUND_ALL; 
  bool telemAlarmEnabled = true;
  uint8_t backlightMode = BACKLIGHT_60S;

  int rollMax  = 1023, rollMin  = 0, rollCenterVal = 512;
  int yawMax   = 1023, yawMin   = 0, yawCenterVal = 512;
  int pitchMax = 1023, pitchMin = 0, pitchCenterVal = 512;
  int thrtlMax = 1023, thrtlMin = 0;
  uint8_t deadZonePerc = 5; //in percentage of stick range
  
  //voltage telemetry
  uint16_t Telem_VoltsThresh = 0; //as 10mV
  
} Sys;

//------------------- MODEL PARAMS -----------------------------------------------------------------

#define NUM_PRP_CHANNLES 8  //Number of proportional channels ## Leave this

#define NUM_MIXSLOTS 10     //More slots results into less models and more ram usage 

/* Structure for model information
*/
struct modelParams {
  //------- first entity is the modelName ----
  char modelName[9]; //8chars + Null
  
  //------- basic params ---------
  
  uint16_t Reverse; // each bit represents a channel. 1 is on, 0 is off
  int8_t EndpointL[NUM_PRP_CHANNLES];   //left endpoint, -100 to 0
  int8_t EndpointR[NUM_PRP_CHANNLES];   //right endpoint, 0 to 100
  int8_t Subtrim[NUM_PRP_CHANNLES];     //-20 to 20
  int8_t Failsafe[NUM_PRP_CHANNLES];    //-101 to 100. -101 means off

  // Ail, Ele, Rud
  uint8_t DualRate;  //Bit0 Ail, Bit1 Ele, Bit2 Rud  
  int8_t RateNormal[3];   //0 to 100
  int8_t RateSport[3];    //0 to 100
  int8_t ExpoNormal[3];   //-100 to 100
  int8_t ExpoSport[3];    //-100 to 100

  int8_t ThrottlePts[5];  //for interpolation of throttle. Range -100 to 100
  
  int8_t Trim[4];         //for Ail, Ele, Thr, Rud inputs. Values -20 to 20
  
  uint8_t Curve1Src;     
  int8_t  Curve1Pts[5];    //interpolation points. Range -100 to 100
  
  uint8_t Slow1Up;   // in tenths of a second
  uint8_t Slow1Down; // in tenths of a second
  uint8_t Slow1Src;  // only switches allowed as source

  uint8_t Timer1ControlSrc;
  uint8_t Timer1Operator;
  int8_t  Timer1Value;      //-100 to 100
  uint8_t Timer1InitMins;   //if 0, timer will count up, else count down

  //------- mixer params ---------
  
  uint8_t MixIn1[NUM_MIXSLOTS];  //index in mix sources array
  int8_t MixIn1Offset[NUM_MIXSLOTS]; //-100 to 100
  int8_t MixIn1Weight[NUM_MIXSLOTS]; //-100 to 100
  int8_t MixIn1Diff[NUM_MIXSLOTS];   //-100 to 100
  
  uint8_t MixIn2[NUM_MIXSLOTS];  //index in mix sources array
  int8_t MixIn2Offset[NUM_MIXSLOTS]; //-100 to 100
  int8_t MixIn2Weight[NUM_MIXSLOTS]; //-100 to 100
  int8_t MixIn2Diff[NUM_MIXSLOTS];   //-100 to 100
  
  uint8_t MixOperator[NUM_MIXSLOTS];  
  uint8_t MixSwitch[NUM_MIXSLOTS];    
  uint8_t MixOut[NUM_MIXSLOTS];  //index in mix sources array
  
} Model; 

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
  //Mixer sources. These are indexes in mix sources array
  //Same order as the name order in the UI
  IDX_ROLL = 0, IDX_PITCH, IDX_THRTL_RAW, IDX_YAW, IDX_KNOB, 
  IDX_100PERC,
  IDX_SWA, IDX_SWB, IDX_SWC, IDX_SWD,
  IDX_SLOW1, IDX_CRV1, 
  IDX_AIL, IDX_ELE, IDX_THRTL_CURV, IDX_RUD,
  IDX_NONE, 
  IDX_CH1, IDX_CH2, IDX_CH3, IDX_CH4, IDX_CH5, IDX_CH6, IDX_CH7, IDX_CH8,
  IDX_VRT1, IDX_VRT2,
 
  NUM_MIXSOURCES //should be last
};

enum { //possible values in MixSwitch array
  SW_NONE = 0,
  SWA_UP, SWA_DOWN,
  SWB_UP, SWB_DOWN,
  SWC_UP, SWC_MID, SWC_DOWN, SWC_NOT_UP, SWC_NOT_MID, SWC_NOT_DOWN,
  SWD_UP, SWD_DOWN,
  
  NUM_MIXSWITCHES //should be last
};

void setDefaultModelName()
{
  uint8_t len = sizeof(Model.modelName);
  for(uint8_t i = 0; i < len - 1; i++) 
    Model.modelName[i] = ' ';
  Model.modelName[len - 1] = '\0';
}

void setDefaultModelBasicParams()
{
  //reverse 
  Model.Reverse = 0;
  //subtrim, endpoints, failsafe
  for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
  {
    Model.EndpointL[i] = -100;
    Model.EndpointR[i] = 100;
    Model.Subtrim[i]   = 0;
    if(i == 2) //specify failsafe on Channel 3
      Model.Failsafe[i]  = -100;
    else
      Model.Failsafe[i]  = -101;
  }
  
  //trim
  Model.Trim[0] = 0;
  Model.Trim[1] = 0;
  Model.Trim[2] = 0;
  Model.Trim[3] = 0;
  
  //rate and expo, dualrate
  Model.DualRate = 0;
  for(uint8_t i = 0; i < 3; i++)
  {
    Model.RateNormal[i] = 100;
    Model.RateSport[i]  = 100;
    Model.ExpoNormal[i] = 0;
    Model.ExpoSport[i]  = 0;
  }
  
  //throttle curve
  Model.ThrottlePts[0] = -100;
  Model.ThrottlePts[1] = -50;
  Model.ThrottlePts[2] = 0;
  Model.ThrottlePts[3] = 50;
  Model.ThrottlePts[4] = 100;
  
  //user defined curves
  Model.Curve1Src = IDX_KNOB;
  Model.Curve1Pts[0] = -100;
  Model.Curve1Pts[1] = -50;
  Model.Curve1Pts[2] = 0;
  Model.Curve1Pts[3] = 50;
  Model.Curve1Pts[4] = 100;
  
  //custom slowed inputs
  Model.Slow1Src = IDX_SWC; 
  Model.Slow1Up = 5;
  Model.Slow1Down = 5;
  
  //Timers
  Model.Timer1ControlSrc = IDX_NONE;
  Model.Timer1Operator = GREATER_THAN;
  Model.Timer1Value = 0;
  Model.Timer1InitMins = 0;

}

void setDefaultModelMixerParams(uint8_t _mixNo)
{
  Model.MixIn1[_mixNo]        = IDX_NONE; 
  Model.MixIn1Offset[_mixNo]  = 0;
  Model.MixIn1Weight[_mixNo]  = 0;
  Model.MixIn1Diff[_mixNo]    = 0;
  Model.MixIn2[_mixNo]        = IDX_NONE;
  Model.MixIn2Offset[_mixNo]  = 0;
  Model.MixIn2Weight[_mixNo]  = 0;
  Model.MixIn2Diff[_mixNo]    = 0;
  Model.MixOperator[_mixNo]   = MIX_ADD; 
  Model.MixSwitch[_mixNo]     = SW_NONE;
  Model.MixOut[_mixNo]        = IDX_NONE;
}

void setDefaultModelMixerParams()
{
  for(uint8_t i = 0; i < NUM_MIXSLOTS; i++)
    setDefaultModelMixerParams(i);
}

//---------------------- MISC ----------------------------------------------------------------------

//---Output channels---
int ChOut[NUM_PRP_CHANNLES];    //Proportional Channels. Centered at 0, range is -500 to 500.
uint8_t DigChA = 0, DigChB = 0; //Digital channels. ChA is momentary, ChB is toggle.

int8_t mixerChOutGraphVals[NUM_PRP_CHANNLES];  //for graphing raw mixer output for channels. range -100 to 100

int curve1SrcVal = 0;

//---Sticks---
int rollIn, pitchIn, throttleIn, yawIn, knobIn; //Scaled stick values, range -500 to 500
bool isCalibratingSticks = false;

bool skipThrottleCheck = false;

//---Switches---
//2 position
bool SwAEngaged = false;
bool SwBEngaged = false;
bool SwDEngaged = false;
//3 position
enum {SWUPPERPOS = 0, SWLOWERPOS = 1, SWMIDPOS = 2};
uint8_t SwCState = SWUPPERPOS; 

//---buttons---
uint8_t buttonCode = 0; 
enum {
  SELECT_KEY = 1, 
  UP_KEY, 
  DOWN_KEY,
};

uint32_t buttonStartTime = 0;
uint32_t buttonReleaseTime = 0;

//Button events
#define LONGPRESSTIME 350
uint8_t pressedButton = 0; //triggered once when the button goes down
uint8_t clickedButton = 0; //triggered when the button is released before heldButton event
uint8_t heldButton = 0;    //triggered when button is held down long enough

//Model timers
uint32_t timer1ElapsedTime = 0;
uint32_t timer1LastElapsedTime = 0;
uint32_t timer1LastPaused = 0;

//---Transmitter Battery---
int battVoltsNow; //millivolts
enum{BATTLOW = 0, BATTHEALTY};
uint8_t battState = BATTHEALTY;

//---Audio----
enum{  
  AUDIO_NONE = 0, 
  AUDIO_BATTERYWARN, AUDIO_THROTTLEWARN, AUDIO_TIMERELAPSED, AUDIO_INACTIVITY, AUDIO_TELEMWARN,
  AUDIO_SWITCHMOVED, AUDIO_TRIM_AIL, AUDIO_TRIM_ELE, AUDIO_TRIM_THR, AUDIO_TRIM_RUD,
  AUDIO_KEYTONE
};
uint8_t audioToPlay = AUDIO_NONE;

//---etc------

bool bindActivated = false;
uint8_t bindStatus = 0;  //1 on success, 2 on fail

uint32_t inputsLastMoved = 0; //inactivity detection

uint8_t transmitterPacketRate = 0;
uint8_t receiverPacketRate = 0;

//Telemetry
uint16_t telem_volts = 0x0FFF;     // in 10mV, sent by receiver with 12bits.  0x0FFF "No data"

//Main loop control
const uint8_t fixedLoopTime = 25; /*in milliseconds. Min 25, Max 30. Should be greater than 
the time taken by radio module to transmit the entire packet or else the window is missed 
resulting in much less throughput. It should also be atleast the time taken to draw the UI, else 
or else the timing becomes inconsistent*/
uint32_t thisLoopNum = 0; //main loop counter
