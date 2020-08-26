
//----------------- Output channels

/*Proportional CHANNLES 1 TO 8
  Centered at 0, range is -500 to 500. All values amplified by factor of 5.
  Earlier we had a range of -100 to 100 but this resolution was poor*/ 
int ChOut[8] = {0, 0, 0, 0, 0, 0, 0, 0};

//Digital channels
uint8_t DigChA = 0, DigChB = 0; /*Digital channels (On/Off). ChA is momentary, ChB is toggle.*/
  
uint8_t PWM_Mode_Ch3 = 1; //1 is servo pwm, 0 is ordinary pwm

//---------------- Sticks and switches
 
int rollIn, pitchIn, throttleIn, yawIn, aux2In; //Scaled stick values, range -500 to 500

//These are changed by stick calibration
int rollMax = 1023, rollMin = 0, rollCenterVal = 512;
int yawMax = 1023, yawMin = 0, yawCenterVal = 512;
int pitchMax = 1023, pitchMin = 0, pitchCenterVal = 512;
int thrtlMax = 1023, thrtlMin = 0;

uint8_t deadZonePerc = 5; //in percentage of stick range
bool isCalibratingSticks = false;

//Switches
//2 position
bool SwAEngaged = false;
bool SwBEngaged = false;
bool SwDEngaged = false;
//3 position
enum {SWUPPERPOS = 2, SWMIDPOS = 0, SWLOWERPOS = 1};
uint8_t SwCState = SWUPPERPOS; 
  
//------------------ Pushbuttons
uint8_t buttonCode = 0; 
enum
{
  SELECT_KEY = 0x01,
  UP_KEY = 0x02,
  DOWN_KEY = 0x04
};

//------------------- Misc MODEL PARAMS 
uint8_t activeModel = 1; ///The current model

char modelName[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'}; //max 8 characters 

bool Reverse[8] = {false, false, false, false, false, false, false, false};
uint8_t EndpointL[8] = {100, 100, 100, 100, 100, 100, 100, 100}; //left endpoint, 0 to 100
uint8_t EndpointR[8] = {100, 100, 100, 100, 100, 100, 100, 100}; //right endpoint, 0 to 100
uint8_t Subtrim[8]   = {50, 50, 50, 50, 50, 50, 50, 50}; //0 to 100, centered at 50
uint8_t Failsafe[8]  = {0,0,1,0,0,0,0,0}; //0 to 201. 0 is off, 1 is -100, 101 is 0, 201 is 100
uint8_t CutValue[8]  = {0,0,1,0,0,0,0,0}; //0 to 201. 0 is off, 1 is -100, 101 is 0, 201 is 100

uint8_t RateNormal[3] = {100, 100, 100}; //0 to 100, 100 is 1:1 ie normal
uint8_t RateSport[3]  = {100, 100, 100}; //0 to 100, 100 is 1:1 ie normal
uint8_t ExpoNormal[3] = {100, 100, 100}; //0 to 200, 100 is no expo
uint8_t ExpoSport[3]  = {100, 100, 100}; //0 to 200, 100 is no expo
bool DualRateEnabled[3] = {false, false, false}; //ail, ele, rud. 
enum{AILRTE = 0,ELERTE,RUDRTE};

uint8_t ThrottlePts[5] = {0, 50, 100, 150, 200}; //for interpolation of throttle
//Note: The above values are automatically scaled afterward to imply range of -100 to 100, which
//are then further upscaled to range of -500 to 500.

///FREE MIXER
#define NUM_MIXSLOTS 10  //caution changing this. If changed, we have to manually update eestore.h
uint8_t MixIn1[NUM_MIXSLOTS]        = {13, 13, 13, 13, 13, 13, 13, 13, 13, 13};  //index in mix sources array
uint8_t MixIn1Offset[NUM_MIXSLOTS]  = {100,100,100,100,100,100,100,100,100,100}; //0 to 200, centered at 100
uint8_t MixIn1Weight[NUM_MIXSLOTS]  = {100,100,100,100,100,100,100,100,100,100}; //0 to 200, centered at 100
uint8_t MixIn1Diff[NUM_MIXSLOTS]    = {100,100,100,100,100,100,100,100,100,100}; //0 to 200, centered at 100
uint8_t MixIn2[NUM_MIXSLOTS]        = {13, 13, 13, 13, 13, 13, 13, 13, 13, 13};  //index in mix sources array
uint8_t MixIn2Offset[NUM_MIXSLOTS]  = {100,100,100,100,100,100,100,100,100,100}; //0 to 200, centered at 100
uint8_t MixIn2Weight[NUM_MIXSLOTS]  = {100,100,100,100,100,100,100,100,100,100}; //0 to 200, centered at 100
uint8_t MixIn2Diff[NUM_MIXSLOTS]    = {100,100,100,100,100,100,100,100,100,100}; //0 to 200, centered at 100
uint8_t MixOperator[NUM_MIXSLOTS]   = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0};   //0 or 1. 0 add, 1 multiply
uint8_t MixOut[NUM_MIXSLOTS]        = {13, 13, 13, 13, 13, 13, 13, 13, 13, 13};  //index in mix sources array

//-------------------------------------
int8_t mixerChOutGraphVals[8];  //for graphing raw mixer output for channels. range -100 to 100

//-------------------------------------
bool rfModuleEnabled = false;

enum{BACKLIGHT_OFF = 0, BACKLIGHT_5S, BACKLIGHT_15S, BACKLIGHT_60S, BACKLIGHT_ON};
uint8_t backlightMode = BACKLIGHT_15S;

uint8_t returnedByte = 0; /*Byte returned by slave mcu. Encodes Switch C state (Bits 7 and 6) and 
the radio packets per second (Bits 5 to 0). */

//----------------------------------Battery 
int battVfactor = 487; //scaling factor
int battVoltsNow; //millivolts
int battVoltsMin = 3400; //millivolts
int battVoltsMax = 3900; //millivolts

enum{_BATTGOOD = 1,_BATTLOW = 0};
uint8_t battState = _BATTGOOD;

//--------------------------------------

//throttle timer
enum {TIMERCOUNTUP = 0, TIMERCOUNTDOWN = 1};
uint8_t throttleTimerType = TIMERCOUNTUP;
uint8_t throttleTimerCntDnInitMinutes = 5; 
uint8_t throttleTimerMinThrottle = 25; // in percentage of throttle stick input: 0 to 100

//----------------- Sounds 
#define AUDIO_NONE          0xD2  
#define AUDIO_TIMERELAPSED  0xD3  
#define AUDIO_THROTTLEWARN  0xD4  
#define AUDIO_BATTERYWARN   0xD5  
#define AUDIO_KEYTONE       0xD6
#define AUDIO_SWITCHMOVED   0xD7

uint8_t audioToPlay = AUDIO_NONE;

enum{SOUND_OFF = 0, SOUND_ALARMS, SOUND_NOKEY, SOUND_ALL};
uint8_t soundMode = SOUND_ALL; 

//-------------------------------

uint32_t thisLoopNum = 0; //main loop counter. For timing certain stuff

bool showPktsPerSec = false; //### debug
