#include "Arduino.h"
#include "common.h"

sysParams_t Sys;
modelParams_t Model; 

int channelOut[NUM_PRP_CHANNLES];  
int8_t mixerChOutGraphVals[NUM_PRP_CHANNLES];  

int rollIn, pitchIn, throttleIn, yawIn, knobIn; 
bool isCalibratingSticks = false;
bool skipThrottleCheck = false;

bool swAEngaged = false;
bool swBEngaged = false;
bool swDEngaged = false;
bool swEEngaged = false;
bool swFEngaged = false;

uint8_t swCState = SWUPPERPOS; 

uint8_t buttonCode = 0; 

uint32_t buttonStartTime = 0;
uint32_t buttonReleaseTime = 0;

uint8_t pressedButton = 0; 
uint8_t clickedButton = 0; 
uint8_t heldButton = 0;

uint32_t timer1ElapsedTime = 0;
uint32_t timer1LastElapsedTime = 0;
uint32_t timer1LastPaused = 0;

int battVoltsNow; 
uint8_t battState = BATTHEALTY;

uint8_t audioToPlay = AUDIO_NONE;

bool isRequestingBind = false;
uint8_t bindStatusCode = 0;  

uint32_t inputsLastMoved = 0; 

uint8_t transmitterPacketRate = 0;
uint8_t receiverPacketRate = 0;

uint16_t telem_volts = 0x0FFF;

uint8_t outputChConfig[9]; 
uint8_t maxOutputChConfig[9];
bool gotOutputChConfig = false;
bool isRequestingOutputChConfig = false;
bool sendOutputChConfig = false;
uint8_t receiverConfigStatusCode = 0; 

uint8_t maxNumOfModels;

uint32_t thisLoopNum = 0; 

void setDefaultSystemParams()
{
  Sys.activeModel = 1;
  Sys.rfOutputEnabled = false;
  Sys.rfPower = RFPOWER_10dBm;
  Sys.inactivityMinutes = 10;
  Sys.soundMode = SOUND_ALL; 
  Sys.backlightMode = BACKLIGHT_60S;

  Sys.rollMax  = 1023, Sys.rollMin  = 0, Sys.rollCenterVal = 512;
  Sys.yawMax   = 1023, Sys.yawMin   = 0, Sys.yawCenterVal = 512;
  Sys.pitchMax = 1023, Sys.pitchMin = 0, Sys.pitchCenterVal = 512;
  Sys.thrtlMax = 1023, Sys.thrtlMin = 0;
  Sys.deadZonePerc = 5; 
  
  Sys.telemAlarmEnabled = true;
  Sys.telemVoltsOnHomeScreen = true;
}

void setDefaultModelName()
{
  uint8_t len = sizeof(Model.modelName);
  for(uint8_t i = 0; i < len - 1; i++) 
    Model.modelName[i] = ' ';
  Model.modelName[len - 1] = '\0';
}

void setDefaultModelBasicParams()
{
  Model.reverse = 0;
  for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
  {
    Model.endpointL[i] = -100;
    Model.endpointR[i] = 100;
    Model.subtrim[i]   = 0;
    Model.failsafe[i]  = -101;
  }
  Model.failsafe[2]  = -100; //specify failsafe on channel 3 (default throttle channel)
  
  Model.trim[0] = 0;
  Model.trim[1] = 0;
  Model.trim[2] = 0;
  Model.trim[3] = 0;
  
  Model.dualRate = 0;
  for(uint8_t i = 0; i < 3; i++)
  {
    Model.rateNormal[i] = 100;
    Model.rateSport[i]  = 100;
    Model.expoNormal[i] = 0;
    Model.expoSport[i]  = 0;
  }
  
  Model.throttlePts[0] = -100;
  Model.throttlePts[1] = -50;
  Model.throttlePts[2] = 0;
  Model.throttlePts[3] = 50;
  Model.throttlePts[4] = 100;
  
  Model.slow1Src = IDX_SWC; 
  Model.slow1Up = 5;
  Model.slow1Down = 5;
  
  Model.funcgenWaveform = FUNC_SINE;
  Model.funcgenPeriod = 5;
  
  Model.timer1ControlSrc = IDX_NONE;
  Model.timer1Operator = GREATER_THAN;
  Model.timer1Value = 0;
  Model.timer1InitMins = 0;
  
  Model.telemVoltsThresh = 0;
}

void setDefaultModelMixerParams(uint8_t _mixNo)
{
  Model.mixIn1[_mixNo]        = IDX_NONE; 
  Model.mixIn1Offset[_mixNo]  = 0;
  Model.mixIn1Weight[_mixNo]  = 0;
  Model.mixIn1Diff[_mixNo]    = 0;
  Model.mixIn2[_mixNo]        = IDX_NONE;
  Model.mixIn2Offset[_mixNo]  = 0;
  Model.mixIn2Weight[_mixNo]  = 0;
  Model.mixIn2Diff[_mixNo]    = 0;
  Model.mixOper_N_Switch[_mixNo] = MIX_ADD << 6 | SW_NONE;
  Model.mixOut[_mixNo]        = IDX_NONE;
}

void setDefaultModelMixerParams()
{
  for(uint8_t i = 0; i < NUM_MIXSLOTS; i++)
    setDefaultModelMixerParams(i);
}
