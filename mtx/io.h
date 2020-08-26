
//Called in main loop
void readSwitchesAndButtons(); 
void readSticks();   
void computeChannelOutputs(); 

//Helpers
int deadzoneAndMap(int _theInpt, int _minVal, int _centerVal, int _maxVal, int _deadzn, int _mapMin, int _mapMax);
int calcRateExpo(int _input, int _rate, int _expo);
int linearInterpolate(int xValues[], int yValues[], int numValues, int pointX);
long weightAndOffset(int _input, int _weight, int _diff, int _offset);

// Table for generating stepped sine waveform in mixer
// SIZE IS 100 BYTES
// Values have been offset about 100 decimal so value range is 0 to 200 in decimal
static const unsigned char sineForm[] PROGMEM = {
  0x64, 0x6A, 0x71, 0x77, 0x7D, 0x83, 0x89, 0x8F, 0x95, 0x9A, 
  0x9F, 0xA4, 0xA9, 0xAD, 0xB2, 0xB5, 0xB9, 0xBC, 0xBF, 0xC1, 
  0xC3, 0xC5, 0xC6, 0xC7, 0xC8, 0xC8, 0xC8, 0xC7, 0xC6, 0xC4, 
  0xC3, 0xC0, 0xBE, 0xBB, 0xB7, 0xB4, 0xB0, 0xAB, 0xA7, 0xA2, 
  0x9D, 0x97, 0x92, 0x8C, 0x86, 0x80, 0x7A, 0x74, 0x6E, 0x67, 
  0x61, 0x5A, 0x54, 0x4E, 0x48, 0x42, 0x3C, 0x36, 0x31, 0x2B, 
  0x26, 0x21, 0x1D, 0x18, 0x14, 0x11, 0x0D, 0x0A, 0x08, 0x05, 
  0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x05, 
  0x07, 0x09, 0x0C, 0x0F, 0x13, 0x16, 0x1B, 0x1F, 0x24, 0x29, 
  0x2E, 0x33, 0x39, 0x3F, 0x45, 0x4B, 0x51, 0x57, 0x5E, 0x64
};

//==================================================================================================
void readSwitchesAndButtons()
{
  //Read keymatrix COLUMN 1, ROWS 1 & 2
  digitalWrite(COL3_MTRX_PIN, LOW);
  digitalWrite(COL2_MTRX_PIN, LOW);
  digitalWrite(COL1_MTRX_PIN, HIGH);
  SwAEngaged = !digitalRead(ROW1_MTRX_PIN); 
  uint8_t val1 = digitalRead(ROW2_MTRX_PIN);
  
  //Read keymatrix COLUMN 2, ROWS 1 & 2
  digitalWrite(COL1_MTRX_PIN, LOW);
  digitalWrite(COL2_MTRX_PIN, HIGH);
  SwBEngaged = digitalRead(ROW1_MTRX_PIN);
  uint8_t val2 = digitalRead(ROW2_MTRX_PIN);
  
  //Read keymatrix COLUMN 3, ROWS 1 & 2
  digitalWrite(COL2_MTRX_PIN, LOW);
  digitalWrite(COL3_MTRX_PIN, HIGH);
  SwDEngaged = digitalRead(ROW1_MTRX_PIN);
  uint8_t val3 = digitalRead(ROW2_MTRX_PIN);
  
  buttonCode = val1 | val2 << 1 | val3 << 2;
  
  //SwCState gotten from slave mcu.
  SwCState = (returnedByte >> 6) & 0x03;
  
  //Play audio when switches are moved
  uint8_t switchesSum = SwAEngaged + SwBEngaged + SwDEngaged + SwCState;
  static uint8_t lastSwitchesSum = switchesSum;
  if(switchesSum != lastSwitchesSum)
    audioToPlay = AUDIO_SWITCHMOVED;
  lastSwitchesSum = switchesSum;
}

//==================================================================================================
void readSticks()
{
  if (isCalibratingSticks)
  {
    return;
  }

  rollIn = 0;
  pitchIn = 0;
  yawIn = 0;
  throttleIn = 0;
  aux2In = 0;

  #define _NUMSTICKSAMPLES 5
  for (int i = 0; i < _NUMSTICKSAMPLES; i += 1)
  {
    rollIn += analogRead(ROLLINPIN);
    pitchIn += analogRead(PITCHINPIN);
    throttleIn += analogRead(THROTTLEINPIN);
    yawIn += analogRead(YAWINPIN);
    aux2In += analogRead(AUX2PIN);
  }
  rollIn /= _NUMSTICKSAMPLES;
  pitchIn /= _NUMSTICKSAMPLES;
  throttleIn /= _NUMSTICKSAMPLES;
  yawIn /= _NUMSTICKSAMPLES;
  aux2In /= _NUMSTICKSAMPLES;
  
  //add deadzone to roll, pitch, yaw sticks centers. Also add deadband at ends of knob for stability
  rollIn = deadzoneAndMap(rollIn, rollMin, rollCenterVal, rollMax, deadZonePerc, -500, 500);
  rollIn = constrain(rollIn, -500, 500);
  pitchIn = deadzoneAndMap(pitchIn, pitchMin, pitchCenterVal, pitchMax, deadZonePerc, -500, 500);
  pitchIn = constrain(pitchIn, -500, 500);
  yawIn = deadzoneAndMap(yawIn, yawMin, yawCenterVal, yawMax, deadZonePerc, -500, 500);
  yawIn = constrain(yawIn, -500, 500);
  aux2In = map(aux2In, 20, 1003, -500, 500); 
  aux2In = constrain(aux2In, -500, 500);

  throttleIn = map(throttleIn, thrtlMin, thrtlMax, -500, 500);
  throttleIn = constrain(throttleIn, -500, 500);
 
  //play audio whenever knob crosses center 
  enum {_POS_SIDE = 0, _CENTER = 1, _NEG_SIDE = 2};
  static uint8_t _aux2Region = _CENTER;
  int _aux2Cntr = 0;
  int _posSideStart =  25;  //has deadband
  int _negSideStart =  -25; //has deadband
  if((aux2In >= _aux2Cntr && _aux2Region == _NEG_SIDE) 
    || (aux2In < _aux2Cntr && _aux2Region == _POS_SIDE)) //crossed center
  {
    audioToPlay = AUDIO_SWITCHMOVED;
    _aux2Region = _CENTER;
  }
  else if(aux2In > _posSideStart)
    _aux2Region = _POS_SIDE;
  else if (aux2In < _negSideStart)
    _aux2Region = _NEG_SIDE;
}

//==================================================================================================
void computeChannelOutputs()
{
  if (isCalibratingSticks)
  {
    return;
  }

  ///Declare mix source array, init it to zero, then populate it afterwards 
  int MixSources[24];   //See the names in UI.
  for(int i = 0; i < 24; i++) 
    MixSources[i] = 0;
  
  enum { 
    //same order as the name order in UI.
    Idxsine = 0, 
    Idxroll, Idxpitch, Idxthrottle, Idxyaw, Idxknob, IdxSwB, IdxSwC, IdxSwD, IdxAil, IdxEle, IdxThrt, IdxRud,
    IdxNone, 
    IdxCh1, IdxCh2, IdxCh3, IdxCh4, IdxCh5, IdxCh6, IdxCh7, IdxCh8, IdxVrt1, IdxVrt2 
  };
        
  ///--Mix source - sine waveform
  //move back and forth btn -500 and 500. Period 2 seconds
  long _timeInstance = millis() % 2000; 
  uint8_t _index = map(_timeInstance, 0, 1999, 0, 99) & 0xFF; //map to table indices
  int _sineVal = pgm_read_byte(&sineForm[_index]); //read from table
  _sineVal = (_sineVal - 100) * 5; //remove offset and scale to range -500 to 500
  MixSources[Idxsine] = _sineVal;
  
  ///--Mix source sticks
  MixSources[Idxroll] = rollIn;
  MixSources[Idxpitch] = pitchIn;
  MixSources[Idxthrottle] = throttleIn;
  MixSources[Idxyaw] = yawIn;
  MixSources[Idxknob] = aux2In;
  
  ///--Mix source Switches B, C, and D
  enum {_DELTA_ = 60}; //Slowed to prevent abrupt change
  //Switch B
  static int _SwBVal = -500;
  if(SwBEngaged == true) _SwBVal += _DELTA_;
  else _SwBVal -= _DELTA_;
  _SwBVal = constrain(_SwBVal, -500, 500);
  MixSources[IdxSwB] = _SwBVal;
  //Switch C (3 pos)
  static int _SwCVal = 0;
  int _target;
  if(SwCState == SWLOWERPOS) _target = 500;
  else if(SwCState == SWUPPERPOS) _target = -500;
  else _target = 0;
  if(_target < _SwCVal) 
  { 
    _SwCVal -= _DELTA_;
    if(_SwCVal < _target)
      _SwCVal = _target;
  }
  else if(_target > _SwCVal) 
  {
    _SwCVal += _DELTA_;
    if(_SwCVal > _target)
      _SwCVal = _target;
  }
  MixSources[IdxSwC] = _SwCVal;
  //Switch D
  static int _SwDVal = -500;
  if(SwDEngaged == true) _SwDVal += _DELTA_;
  else _SwDVal -= _DELTA_;
  _SwDVal = constrain(_SwDVal, -500, 500);
  MixSources[IdxSwD] = _SwDVal;
  
  ///--Mix source Ail, Ele, Thr, Rudd
  //apply rate and expo 
  if(SwBEngaged == false || DualRateEnabled[AILRTE] == false)
    MixSources[IdxAil]  = calcRateExpo(rollIn,  RateNormal[AILRTE], ExpoNormal[AILRTE]);
  else
    MixSources[IdxAil]  = calcRateExpo(rollIn,  RateSport[AILRTE], ExpoSport[AILRTE]);
  if(SwBEngaged == false || DualRateEnabled[ELERTE] == false)
    MixSources[IdxEle]  = calcRateExpo(pitchIn, RateNormal[ELERTE], ExpoNormal[ELERTE]);
  else
    MixSources[IdxEle]  = calcRateExpo(pitchIn, RateSport[ELERTE], ExpoSport[ELERTE]);
  if(SwBEngaged == false || DualRateEnabled[RUDRTE] == false)
    MixSources[IdxRud] = calcRateExpo(yawIn, RateNormal[RUDRTE], ExpoNormal[RUDRTE]);
  else
    MixSources[IdxRud] = calcRateExpo(yawIn, RateSport[RUDRTE], ExpoSport[RUDRTE]);
  //apply throttle curve
  int xpoints[5] = {-500, -250, 0, 250, 500};
  int ypoints[5];
  for(int i = 0; i < 5; i++)
    ypoints[i] = ((int)ThrottlePts[i] - 100) * 5;
  MixSources[IdxThrt] = linearInterpolate(xpoints, ypoints, 5, throttleIn);

  ///Predefined mixes
  //So we don't waste the limited mixer slots
  MixSources[IdxCh1] = MixSources[IdxAil];  //send Ail  to Ch1
  MixSources[IdxCh2] = MixSources[IdxEle];  //send Ele  to Ch2
  MixSources[IdxCh3] = MixSources[IdxThrt]; //Send Thrt to Ch3
  MixSources[IdxCh4] = MixSources[IdxRud];  //Send Rud  to Ch4
  
  ///FREE MIXER
  //Takes two input sources, adds or multiplies them, then writes result to specified output
  for(int _mixNum = 0; _mixNum < NUM_MIXSLOTS; _mixNum++)
  {
    if(MixOut[_mixNum] == IdxNone) //skip to next iteration
      continue;  
    
    //---Input1---
    long _operand1 = 0;
    if(MixIn1[_mixNum] != IdxNone) //only calculate if input is other than "None"
    {
      _operand1 = weightAndOffset(MixSources[MixIn1[_mixNum]], MixIn1Weight[_mixNum], 
                                  MixIn1Diff[_mixNum], MixIn1Offset[_mixNum]);
    }
    //---Input2---
    long _operand2 = 0;
    if(MixIn2[_mixNum] != IdxNone) //only calculate if input is other than "None"
    {
      _operand2 = weightAndOffset( MixSources[MixIn2[_mixNum]], MixIn2Weight[_mixNum], 
                                   MixIn2Diff[_mixNum], MixIn2Offset[_mixNum]);
    }
    //---Mix the two inputs---
    long _output;      
    if(MixOperator[_mixNum] == 0) //add
      _output = _operand1 + _operand2; 
    else //multiply
      _output = (_operand1 * _operand2) / 500; 
    //---Clamp-----
    _output = constrain(_output, -500, 500);
    //---Update sources array for next iteration--- 
    MixSources[MixOut[_mixNum]] = int(_output);
  }
  
  //export mix results for graphing
  for(int i = 0; i < 8; i++)
    mixerChOutGraphVals[i] = MixSources[IdxCh1 + i] / 5; //divide by 5 to fit datatype
  
  ///WRITE TO CHANNELS
  for(int i = 0; i < 8; i++)
  {
    ChOut[i] = MixSources[IdxCh1 + i];
   
    //---Reverse--
    if (Reverse[i] == true) 
      ChOut[i] = 0 - ChOut[i]; 

    //---Subtrim---
    ChOut[i] += 5 * (Subtrim[i] - 50); 
    
    //---Check Cut. If specified and switch engaged, overide--
    if(SwAEngaged == true && CutValue[i] > 0)
      ChOut[i] = 5 * (CutValue[i] - 101);
    
    //---Endpoints--
    ChOut[i] = constrain(ChOut[i], 5 * (0 - EndpointL[i]), 5 * EndpointR[i]); 
  }
}

//====================================Helpers=======================================================

int deadzoneAndMap(int _theInpt, int _minVal, int _centerVal, int _maxVal, int _deadzn, int _mapMin, int _mapMax)
{
  long _ddZnTmp = (long)(_maxVal - _minVal) * _deadzn;
  _ddZnTmp /= 100;
  int _ddZnVal = int(_ddZnTmp) / 2; //divide by 2 as we apply deadzone about center
  int _mapCenter = (_mapMin / 2) + (_mapMax / 2);
  //add dead zone and map the input
  int _outpt;
  if (_theInpt > _centerVal + _ddZnVal)
    _outpt = map(_theInpt, _centerVal + _ddZnVal, _maxVal, _mapCenter + 1, _mapMax);
  else if (_theInpt < _centerVal - _ddZnVal)
    _outpt = map(_theInpt, _minVal, _centerVal - _ddZnVal, _mapMin, _mapCenter - 1);
  else
    _outpt = _mapCenter;

  return _outpt;
}

int calcRateExpo(int _input, int _rate, int _expo)
{
  /* This function is for applying rate and cubic 'expo' to aileron, elevator and rudder channels.
     Ranges: 
     _input  -500 to 500, 
     _rate   0 to 100
     _expo   0 to 200. 100 is linear, < 100 is negative expo, > 100 is positive expo 
  */
 
  /**
    The cubic equation used is 
    y = k*x + (1-k)*x^3  where k is expo factor. 
    This equation applies for input output ranges from -1 to +1. 
    k = 1 is linear. 
    0 < k < 1    is less sensitive in center and more sensitive at ends.  
    1 < k <= 1.5 is more sensitive in center and less senstive at outside.
    For our implementation, we only use the range 0 < k < 1 and simply 'invert' the result if 
    positive expo is specified.
    We need to scale up this equation to avoid floating point maths. After calculation, we scale back
    the result. 
    Thus the modified equation taking into account the range of our parameters (to ensure no overflow)
    is as follows. 
    y = ( (k/100 * x/500)  +  ((1 - k/100)*((x*x*x)/(500*500*500))) ) * 500
    This simplifies to 
    y =  (((250000*k  + x*x*(100-k)) / 250000 ) * x) / 100
    We also work with only positive x
  */
  
  long x = _input;
  if(_input < 0) 
    x = -x;
    
  long k = _expo;
  if(_expo > 100)
  {
    k = 200 - k;
    x -= 500;
  }

  //apply expo factor
  long y = 250000L * k;
  y += ((x*x)*(100-k));
  y /= 250000L;
  y *= x;
  y /= 100;
  if(_expo > 100) 
    y += 500;

  //apply rate
  long rate = _rate;
  y *= rate;
  y /= 100;
  
  if(_input < 0) 
    y = -y;
    
  return int(y);
}

long weightAndOffset(int _input, int _weight, int _diff, int _offset)
{
  //map passed values
  _weight -= 100; 
  _diff -= 100;
  _offset = (_offset - 100) * 5;
  //calc left and right weights basing on differential
  int _wtR, _wtL; 
  if(_diff >= 0)
  {
    _wtR = _weight;
    _wtL = (_weight * (100 - _diff))/100;
  }
  else 
  {
    _wtL = _weight;
    _wtR = (_weight * (100 + _diff))/100;
  }
  //Compute output and add offset
  _weight = _input > 0 ? _wtR : _wtL;
  long _outVal = _input;
  _outVal *= _weight;
  _outVal /= 100;
  _outVal += _offset;
  return _outVal;
}

int linearInterpolate(int xValues[], int yValues[], int numValues, int pointX)
{
  //Implementation of linear Interpolation using integers
  //Formula used is y = ((x - x0)*(y1 - y0))/(x1 - x0) + y0;
  long y = 0;
  if(pointX < xValues[0])
    y = yValues[0]; 
  else if(pointX > xValues[numValues - 1])
    y = yValues[numValues - 1];
  else  //point is in range, interpolate
  { 
    for(int i = 0; i < numValues - 1; i++)
    {
      if(pointX >= xValues[i] && pointX <= xValues[i+1])
      {
        long x0 = xValues[i];
        long x1 = xValues[i+1];
        long y0 = yValues[i];
        long y1 = yValues[i+1];
        long x = pointX;
        y = (((x - x0) * (y1 - y0)) + (y0 * (x1 - x0))) / (x1 - x0);
        break;
      }
    }
  }
  return int(y); 
}
