
//----------------- Output channels

/*Proportional CHANNLES 1 TO 8
  Centered at 0, range is -500 to 500. All values amplified by factor of 5.
  Earlier we had a range of -100 to 100 but this resolution was poor*/ 
int ChOut[8] = {0, 0, 0, 0, 0, 0, 0, 0};
enum {CH1OUT = 0, CH2OUT, CH3OUT, CH4OUT, CH5OUT, CH6OUT, CH7OUT, CH8OUT};

//Digital channels
bool DigChA = 0, DigChB = 0; /*These are digital channels (binary). ChA is momentary,
  ChB toggle.*/
  
//---------------- Sticks and switches

///These hold scaled stick values
int rollIn = 0;     // range -500 to 500
int pitchIn = 0;    // -500 to 500
int throttleIn = 0; // -500 to 500 Interpretation is -500 is idle
int yawIn = 0;      // -500 to 500
int aux2In = 0;     // -500 to 500

//These are changed by stick calibration
int rollMax = 1023, rollMin = 0, rollCenterVal = 512;
int yawMax = 1023, yawMin = 0, yawCenterVal = 512;
int pitchMax = 1023, pitchMin = 0, pitchCenterVal = 512;
int thrtlMax = 1023, thrtlMin = 0;

uint8_t deadZonePerc = 5; //in percentage of stick range

bool isCalibratingSticks = false;

//Switches
bool SwAEngaged = false;
bool SwBEngaged = false;
bool SwDEngaged = false;
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

char modelName[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'}; //max 8 characters. Dont change this. 

bool Reverse[8] = {false, false, false, false, false, false, false, false};
uint8_t EndpointL[8] = {100, 100, 100, 100, 100, 100, 100, 100}; //left endpoint, 0 to 100
uint8_t EndpointR[8] = {100, 100, 100, 100, 100, 100, 100, 100}; //right endpoint, 0 to 100
uint8_t Subtrim[8] = {50, 50, 50, 50, 50, 50, 50, 50}; //0 to 100, centered at 50
uint8_t Failsafe[8] = {0,0,1,0,0,0,0,0}; //0 to 201. 0 means off, 1 is -100, 101 is 0, 201 is 100.
uint8_t CutValue[8] = {0,0,1,0,0,0,0,0}; //0 to 201. 0 means off, 1 is -100, 101 is 0, 201 is 100.

uint8_t RateNormal[3] = {100, 100, 100};    //0 to 100, 100 is 1:1 ie normal
uint8_t RateSport[3] = {100, 100, 100};    //0 to 100, 100 is 1:1 ie normal
uint8_t ExpoNormal[3] = {100, 100, 100}; //0 to 200, 100 is no expo. For UI, represented as -100 to 100
uint8_t ExpoSport[3] = {100, 100, 100}; //0 to 200, 100 is no expo. For UI, represented as -100 to 100
bool DualRateEnabled[3] = {false, false, false}; //ail, ele, rud. 
enum{AILRTE = 0,ELERTE,RUDRTE};

uint8_t ThrottlePts[5] = {0, 50, 100, 150, 200}; //for interpolation of throttle
//Note: The above values are automatically scaled afterward to imply range of -100 to 100, which
//are then further upscaled to range of -500 to 500.


///FREE MIXER
#define NUM_MIXSLOTS 10
uint8_t MixIn1[10]        = {13,  13,  13,  13,  13,  13,  13,  13, 13, 13}; //index in mix sources array
uint8_t MixIn1Offset[10]  = {100, 100, 100, 100, 100, 100, 100, 100,100,100}; //0 to 200, centered at 100
uint8_t MixIn1Weight[10]  = {100, 100, 100, 100, 100, 100, 100, 100,100,100}; //0 to 200, centered at 100
uint8_t MixIn1Diff[10]    = {100, 100, 100, 100, 100, 100, 100, 100,100,100}; //0 to 200, centered at 100
uint8_t MixIn2[10]        = {13,  13,  13,  13,  13,  13,  13,  13,  13,  13}; //index in mix sources array
uint8_t MixIn2Offset[10]  = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100}; //0 to 200, centered at 100
uint8_t MixIn2Weight[10]  = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100}; //0 to 200, centered at 100
uint8_t MixIn2Diff[10]    = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100}; //0 to 200, centered at 100
uint8_t MixOperator[10]   = {0,0,0,0,0,0,0,0,0,0}; //0 or 1 where 0 means add, 1 means multiply
uint8_t MixOut[10]        = {13, 13, 13, 13, 13, 13, 13, 13, 13, 13}; //index in mix sources array

//-------------------------------------
int8_t ChOutMixer[8];  //for graphing raw mixer output for channels. range -100 to 100

//-------------------------------------

bool rfModuleEnabled = false;
bool backlightEnabled = true;

uint8_t returnedByte = 0; /*Byte returned by slave mcu. Contains Switch C state (Bits 7 and 6) and 
the radio packets per second (Bits 5 to 0). */

//----------------------------------Battery 
int battVoltsNow = BATTV_MAX;  //in millivolts.
enum{_BATTHEALTHY_ = 1,_BATTLOW_ = 0};
uint8_t battState = _BATTHEALTHY_;

//--------------------------------------

//throttle timer
enum {TIMERCOUNTUP = 0, TIMERCOUNTDOWN = 1};
uint8_t timerType = TIMERCOUNTUP;
uint8_t timerCountDownInitMins = 5; 
uint8_t throttleThreshold_stopwatch = 25; // in percentage of throttle stick input: 0 to 100

//----------------- Sounds 
#define AUDIO_NONE          0xD2  
#define AUDIO_TIMERELAPSED  0xD3  
#define AUDIO_THROTTLEWARN  0xD4  
#define AUDIO_BATTERYWARN   0xD5  
#define AUDIO_KEYTONE       0xD6
#define AUDIO_SWITCHMOVED   0xD7
#define AUDIO_EXTREMEREACHED 0xD8

uint8_t audioToPlay = AUDIO_NONE;

enum{SYSSOUNDS_OFF = 0, SYSSOUNDS_ALERTS = 1, SYSSOUNDS_ALL = 2};
uint8_t sysSoundsMode = SYSSOUNDS_ALL; 
