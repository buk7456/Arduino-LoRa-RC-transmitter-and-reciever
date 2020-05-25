void eeReadSysConfig();
void eeUpdateSysConfig();

void eeCopyModel(uint8_t _src, uint8_t _dest); //copies everything except modelName

void eeReadModelBasicData(uint8_t _mdlNO); 
void eeUpdateModelBasicData(uint8_t _mdlNO);

void eeReadMixData(uint8_t _mdlNO);
void eeUpdateMixData(uint8_t _mdlNO);
void eeCopyMixFrom(uint8_t _mdlNO, uint8_t _mixNo);

void eeReadModelName(uint8_t _mdlNO);
void eeUpdateModelName(uint8_t _mdlNO);


//helpers
int  getModelDataOffsetAddr(uint8_t _mdlNO);
void EEPROMWriteInt(int address, int value);
int  EEPROMReadInt(int address);
long EEPROMReadlong(int address);
void EEPROMWritelong(int address, long value);

///===================================== EEPROM Addresses ==========================================

///------------------ SYSTEM DATA ----------------------------
//Allocated space is addresses 0 to 33

#define _EEPROM_INIT   0
#define _LASTMODEL     1
#define _BACKLIGHT     2
#define _RADIOENABLED  3
#define _SYSSOUNDSMODE 4

//sticks related
#define _DEADZONEPERC 7   //1 BYTE
#define _THRTLMIN     8   //2 bytes
#define _THRTLMAX     10  //2 bytes
#define _YAWMIN       12  //2 bytes
#define _YAWMAX       14  //2 bytes
#define _PITCHMIN     16  //2 bytes
#define _PITCHMAX     18  //2 bytes
#define _ROLLMIN      20  //2 bytes
#define _ROLLMAX      22  //2 bytes
#define _YAWCENTER    24  //2 bytes
#define _ROLLCENTER   26  //2 bytes
#define _PITCHCENTER  28  //2 bytes


///---------------------RESEVED 4 BYTES HERE-------------------------


///---------------------MODEL NAMES ---------------------------------
//Actual location depends on model
#define MODELNAMESTARTADDR 34 
#define MODELNAMESPAN 8 //8 bytes per name (\0 not stored). So for 6 models a total of 48 bytes


/// --------------------RESERVED 8 BYTES HERE------------------------


///---------------------MODEL DATA ----------------------------------

// Actual location depends on selected model.

#define MODELDATASTARTADDR 90
#define MODELDATASPAN     154 //for a single model

//OFFSET ADDRESSES
//NOTE: ADDRESSES SHOULD BE ORDERLY, AS IT HELPS WITH ARRAYS

//Reverses
#define _REVERSE  0 //1byte (1bit per channel) Ch8,Ch7,Ch6,Ch5,Ch4,Ch3,Ch2,Ch1

//Left endpoints
#define _CH1ENDPL 1
#define _CH2ENDPL 2
#define _CH3ENDPL 3
#define _CH4ENDPL 4
#define _CH5ENDPL 5
#define _CH6ENDPL 6
#define _CH7ENDPL 7
#define _CH8ENDPL 8

//Right endpoints
#define _CH1ENDPR 9
#define _CH2ENDPR 10
#define _CH3ENDPR 11
#define _CH4ENDPR 12
#define _CH5ENDPR 13
#define _CH6ENDPR 14
#define _CH7ENDPR 15
#define _CH8ENDPR 16

//Trims
#define _CH1SUBTRIM 17
#define _CH2SUBTRIM 18
#define _CH3SUBTRIM 19
#define _CH4SUBTRIM 20
#define _CH5SUBTRIM 21
#define _CH6SUBTRIM 22
#define _CH7SUBTRIM 23
#define _CH8SUBTRIM 24

//Cuts
#define _CH1CUT 25
#define _CH2CUT 26
#define _CH3CUT 27
#define _CH4CUT 28
#define _CH5CUT 29
#define _CH6CUT 30
#define _CH7CUT 31
#define _CH8CUT 32

//Failsafes
#define _CH1FAILSAFE 33
#define _CH2FAILSAFE 34
#define _CH3FAILSAFE 35
#define _CH4FAILSAFE 36
#define _CH5FAILSAFE 37
#define _CH6FAILSAFE 38
#define _CH7FAILSAFE 39
#define _CH8FAILSAFE 40


//RateNormal
#define _AILRATENORM 41
#define _ELERATENORM 42
#define _RUDRATENORM 43

//RateSport
#define _AILRATESPORT 44
#define _ELERATESPORT 45
#define _RUDRATESPORT 46

//ExpoNormal
#define _AILEXPONORM 47
#define _ELEEXPONORM 48
#define _RUDEXPONORM 49

//ExpoSport
#define _AILEXPOSPORT 50
#define _ELEEXPOSPORT 51
#define _RUDEXPOSPORT 52

//Throttle points
#define _THRTPT0 53
#define _THRTPT1 54
#define _THRTPT2 55
#define _THRTPT3 56
#define _THRTPT4 57

//Throttle timer
#define _STPWTCHTHROTTLETHRESH 58
#define _TIMERTYPE 59
#define _TIMERCOUNTDOWNINIT 60

//Mixer data
#define _MIXIN1       61  //10 bytes
#define _MIXIN1OFFSET 71  //10 bytes
#define _MIXIN1WGT    81 //10 bytes
#define _MIXIN1DIFF   91  //10 bytes

#define _MIXIN2       101 //10 bytes
#define _MIXIN2OFFSET 111 //10 bytes
#define _MIXIN2WGT    121 //10 bytes
#define _MIXIN2DIFF   131 //10 bytes

#define _MIXOUT       141 //10 bytes

#define _MIXOPERATOR_LOWBYTE  151 //1 byte. mix slots 1 to 8
#define _MIXOPERATOR_HIGHBYTE 152 //1 byte mix slots 9 to 10

//DR enabled
#define _DRENABLED 153 //1byte  0,0,0,0,0,Ail,Ele,Rud


//======================================= SYSTEM ===================================================

void eeReadSysConfig()
{
  activeModel      = EEPROM.read(_LASTMODEL);
  rfModuleEnabled  = EEPROM.read(_RADIOENABLED);
  backlightEnabled = EEPROM.read(_BACKLIGHT);
  soundMode    = EEPROM.read(_SYSSOUNDSMODE);

  thrtlMin       = EEPROMReadInt(_THRTLMIN);
  thrtlMax       = EEPROMReadInt(_THRTLMAX);
  
  yawMin         = EEPROMReadInt(_YAWMIN);
  yawCenterVal   = EEPROMReadInt(_YAWCENTER);
  yawMax         = EEPROMReadInt(_YAWMAX);
  
  pitchMin       = EEPROMReadInt(_PITCHMIN);
  pitchCenterVal = EEPROMReadInt(_PITCHCENTER);
  pitchMax       = EEPROMReadInt(_PITCHMAX);
  
  rollMin        = EEPROMReadInt(_ROLLMIN);
  rollCenterVal  = EEPROMReadInt(_ROLLCENTER);
  rollMax        = EEPROMReadInt(_ROLLMAX);
  
  deadZonePerc   = EEPROM.read(_DEADZONEPERC);
}

//--------------------------------------------------------------------------------------------------

void eeUpdateSysConfig()
{
  EEPROM.update(_LASTMODEL, activeModel);
  EEPROM.update(_RADIOENABLED, rfModuleEnabled);
  EEPROM.update(_BACKLIGHT, backlightEnabled);
  EEPROM.update(_SYSSOUNDSMODE, soundMode);

  EEPROMWriteInt(_THRTLMAX, thrtlMax);
  EEPROMWriteInt(_THRTLMIN, thrtlMin);
  EEPROMWriteInt(_YAWMAX, yawMax);
  EEPROMWriteInt(_YAWCENTER, yawCenterVal);
  EEPROMWriteInt(_YAWMIN, yawMin);
  EEPROMWriteInt(_PITCHCENTER, pitchCenterVal);
  EEPROMWriteInt(_PITCHMAX, pitchMax);
  EEPROMWriteInt(_PITCHMIN, pitchMin);
  EEPROMWriteInt(_ROLLCENTER, rollCenterVal);
  EEPROMWriteInt(_ROLLMIN, rollMin);
  EEPROMWriteInt(_ROLLMAX, rollMax);

  EEPROM.update(_DEADZONEPERC, deadZonePerc);
}

//===================================== MODEL NAMES ================================================

void eeReadModelName(uint8_t _mdlNO)
{
  int _mdlNameOffset_ = MODELNAMESTARTADDR + (MODELNAMESPAN * (_mdlNO - 1));
  
  for(uint8_t i = 0; i < (sizeof(modelName)/sizeof(modelName[0])) - 1; i++) // \0 not stored so subtract 1
  {
    *(modelName + i) = EEPROM.read(_mdlNameOffset_ + i);
  }
}

//--------------------------------------------------------------------------------------------------

void eeUpdateModelName(uint8_t _mdlNO)
{
  int _mdlNameOffset_ = MODELNAMESTARTADDR + (MODELNAMESPAN * (_mdlNO - 1));

  for(uint8_t i = 0; i < (sizeof(modelName)/sizeof(modelName[0])) - 1; i++) // \0 not stored so subtract 1
  {
    EEPROM.update(_mdlNameOffset_ + i, *(modelName + i) );
  }
}


//========================================== MODEL =================================================

void eeReadModelBasicData(uint8_t _mdlNO)
{
  int _mdlOffset_ = getModelDataOffsetAddr(_mdlNO);
  
  //Reverse, Diff, Endpoints, Subtrim, Cut, Failsafe 
  uint8_t reverse = EEPROM.read(_mdlOffset_ + _REVERSE);
  for(int i = 0; i < 8; i++) 
  {
    Reverse[i]      = (reverse >> i) & 0x01;    
    EndpointL[i]    = EEPROM.read(_mdlOffset_ + _CH1ENDPL   + i);
    EndpointR[i]    = EEPROM.read(_mdlOffset_ + _CH1ENDPR   + i);
    Subtrim[i]      = EEPROM.read(_mdlOffset_ + _CH1SUBTRIM + i);
    CutValue[i]     = EEPROM.read(_mdlOffset_ + _CH1CUT     + i);
    Failsafe[i]     = EEPROM.read(_mdlOffset_ + _CH1FAILSAFE + i);
  }
  
  //Rates, expo, we have 3 channels
  uint8_t drEnabled = EEPROM.read(_mdlOffset_ + _DRENABLED);
  for(int i = 0; i < 3; i++)
  {
    RateNormal[i] = EEPROM.read(_mdlOffset_ + _AILRATENORM + i);
    RateSport[i] = EEPROM.read(_mdlOffset_ + _AILRATESPORT + i);
    ExpoNormal[i] = EEPROM.read(_mdlOffset_ + _AILEXPONORM + i);
    ExpoSport[i] = EEPROM.read(_mdlOffset_ + _AILEXPOSPORT + i);
    DualRateEnabled[i] = (drEnabled >> i) & 0x01; 
  }
  
  //Throttle curve points. We have 5 points
  for(int i=0; i < 5; i++)
  {
    ThrottlePts[i] = EEPROM.read(_mdlOffset_ + _THRTPT0 + i);
  }
  

  //Throttle timer 
  throttleThreshold_stopwatch = EEPROM.read(_mdlOffset_ + _STPWTCHTHROTTLETHRESH);
  timerType = EEPROM.read(_mdlOffset_ + _TIMERTYPE);
  timerCountDownInitMins = EEPROM.read(_mdlOffset_ + _TIMERCOUNTDOWNINIT);
}

//--------------------------------------------------------------------------------------------------

void eeUpdateModelBasicData(uint8_t _mdlNO)
{
  int _mdlOffset_ = getModelDataOffsetAddr(_mdlNO);
  
  //Reverse, Subtrim, Diff, Offset, Endpoints, Cut, Failsafe
  uint8_t reverse = 0;
  for(int i = 0; i < 8; i++) 
  {
    reverse = reverse | (Reverse[i] << i);
    EEPROM.update(_mdlOffset_ + _CH1SUBTRIM  + i,   Subtrim[i]);
    EEPROM.update(_mdlOffset_ + _CH1ENDPL    + i,   EndpointL[i]);
    EEPROM.update(_mdlOffset_ + _CH1ENDPR    + i,   EndpointR[i]);
    EEPROM.update(_mdlOffset_ + _CH1CUT      + i,   CutValue[i]);
    EEPROM.update(_mdlOffset_ + _CH1FAILSAFE + i,   Failsafe[i]);
  }
  EEPROM.update(_mdlOffset_ + _REVERSE, reverse);
  
  //Rates, expo, we have 3 channels
  uint8_t drEnabled = 0;
  for(int i = 0; i < 3; i++)
  {
    EEPROM.update(_mdlOffset_ + _AILRATENORM  + i, RateNormal[i]);
    EEPROM.update(_mdlOffset_ + _AILRATESPORT + i, RateSport[i]);
    EEPROM.update(_mdlOffset_ + _AILEXPONORM  + i, ExpoNormal[i]);
    EEPROM.update(_mdlOffset_ + _AILEXPOSPORT + i, ExpoSport[i]);
    drEnabled = drEnabled | (DualRateEnabled[i] << i);
  }
  EEPROM.update(_mdlOffset_ + _DRENABLED, drEnabled);

  //Throttle curve points. We have 5 points
  for(int i=0; i < 5; i++)
  {
    EEPROM.update(_mdlOffset_ + _THRTPT0 + i, ThrottlePts[i]);
  }

  EEPROM.update(_mdlOffset_ + _STPWTCHTHROTTLETHRESH, throttleThreshold_stopwatch);
  EEPROM.update(_mdlOffset_ + _TIMERTYPE, timerType);
  EEPROM.update(_mdlOffset_ + _TIMERCOUNTDOWNINIT, timerCountDownInitMins);
}

//--------------------------------------------------------------------------------------------------

void eeCopyModel(uint8_t _src, uint8_t _dest)
{
  int _srcOffset_  =  getModelDataOffsetAddr(_src);
  int _destOffset_ =  getModelDataOffsetAddr(_dest);
  uint8_t _readByte_;
  for (int i = 0; i < MODELDATASPAN; i += 1)
  {
    _readByte_ = EEPROM.read(_srcOffset_ + i);
    EEPROM.update(_destOffset_ + i, _readByte_);
  }
}

//========================================== MIX DATA ==============================================

void eeReadMixData(uint8_t _mdlNO)
{
  int _mdlOffset_ = getModelDataOffsetAddr(_mdlNO);
  
  uint8_t mixOperLB = EEPROM.read(_mdlOffset_ + _MIXOPERATOR_LOWBYTE);
  uint8_t mixOperHB = EEPROM.read(_mdlOffset_ + _MIXOPERATOR_HIGHBYTE);
  
  for(int _mixNo = 0; _mixNo < NUM_MIXSLOTS; _mixNo++)
  {
    MixIn1[_mixNo]        = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN1);
    MixIn1Offset[_mixNo]  = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN1OFFSET);
    MixIn1Weight[_mixNo]  = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN1WGT);
    MixIn1Diff[_mixNo]    = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN1DIFF);

    MixIn2[_mixNo]        = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN2);
    MixIn2Offset[_mixNo]  = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN2OFFSET);
    MixIn2Weight[_mixNo]  = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN2WGT);
    MixIn2Diff[_mixNo]    = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN2DIFF);
  
    MixOut[_mixNo]        = EEPROM.read(_mdlOffset_ + _mixNo + _MIXOUT);
    
    if(_mixNo < 8) //is in low byte ordered mix nums 76543210
      MixOperator[_mixNo] = (mixOperLB >> _mixNo) & 0x01;
    else //is in high byte
      MixOperator[_mixNo] = (mixOperHB >> (_mixNo - 8)) & 0x01;
  }
}

//--------------------------------------------------------------------------------------------------

void eeUpdateMixData(uint8_t _mdlNO)
{
  int _mdlOffset_ = getModelDataOffsetAddr(_mdlNO);
  
  uint8_t mixOperLB = 0x00;
  uint8_t mixOperHB = 0x00;
  
  for(int _mixNo = 0; _mixNo < NUM_MIXSLOTS; _mixNo++)
  {
    EEPROM.update(_mdlOffset_ + _mixNo + _MIXIN1       , MixIn1[_mixNo]        );
    EEPROM.update(_mdlOffset_ + _mixNo + _MIXIN1OFFSET , MixIn1Offset[_mixNo]  );
    EEPROM.update(_mdlOffset_ + _mixNo + _MIXIN1WGT    , MixIn1Weight[_mixNo] );
    EEPROM.update(_mdlOffset_ + _mixNo + _MIXIN1DIFF   , MixIn1Diff[_mixNo] );

    EEPROM.update(_mdlOffset_ + _mixNo + _MIXIN2       , MixIn2[_mixNo]        );
    EEPROM.update(_mdlOffset_ + _mixNo + _MIXIN2OFFSET , MixIn2Offset[_mixNo]  );
    EEPROM.update(_mdlOffset_ + _mixNo + _MIXIN2WGT    , MixIn2Weight[_mixNo] );
    EEPROM.update(_mdlOffset_ + _mixNo + _MIXIN2DIFF   , MixIn2Diff[_mixNo] );

    EEPROM.update(_mdlOffset_ + _mixNo + _MIXOUT       , MixOut[_mixNo]        );
    
    if(_mixNo < 8)
      mixOperLB = mixOperLB | (MixOperator[_mixNo] << _mixNo);
    else
      mixOperHB = mixOperHB | (MixOperator[_mixNo] << (_mixNo - 8));
      
  }
  
  EEPROM.update(_mdlOffset_ + _MIXOPERATOR_LOWBYTE, mixOperLB);
  EEPROM.update(_mdlOffset_ + _MIXOPERATOR_HIGHBYTE, mixOperHB);
}

//--------------------------------------------------------------------------------------------------

void eeCopyMixFrom(uint8_t _mdlNO, uint8_t _mixNo)
{
  int _mdlOffset_ = getModelDataOffsetAddr(_mdlNO);
  
  MixIn1[_mixNo]       = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN1);
  MixIn1Offset[_mixNo] = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN1OFFSET);
  MixIn1Weight[_mixNo] = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN1WGT);
  MixIn1Diff[_mixNo]   = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN1DIFF);

  MixIn2[_mixNo]       = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN2);
  MixIn2Offset[_mixNo] = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN2OFFSET);
  MixIn2Weight[_mixNo] = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN2WGT);
  MixIn2Diff[_mixNo]   = EEPROM.read(_mdlOffset_ + _mixNo + _MIXIN2DIFF);

  MixOut[_mixNo]       = EEPROM.read(_mdlOffset_ + _mixNo + _MIXOUT);
  
  if(_mixNo < 8)
  {
    uint8_t mixOperLB = EEPROM.read(_mdlOffset_ + _MIXOPERATOR_LOWBYTE);
    MixOperator[_mixNo] = (mixOperLB >> _mixNo) & 0x01;
  }
  else
  {
    uint8_t mixOperHB = EEPROM.read(_mdlOffset_ + _MIXOPERATOR_HIGHBYTE);
    MixOperator[_mixNo] = (mixOperHB >> (_mixNo - 8)) & 0x01;
  }
}


//====================================== HELPERS ===================================================

int getModelDataOffsetAddr(uint8_t _mdlNO)
{
  return MODELDATASTARTADDR + (MODELDATASPAN * (_mdlNO - 1));
}

//--------------------------------------------------------------------------------------------------

int EEPROMReadInt(int address)
{
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}

//--------------------------------------------------------------------------------------------------

void EEPROMWriteInt(int address, int value)
{
  uint8_t two = (value & 0xFF);
  uint8_t one = ((value >> 8) & 0xFF);

  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}
