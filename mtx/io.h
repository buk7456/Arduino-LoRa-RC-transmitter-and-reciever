
//Called in main loop
void readSwitchesAndButtons(); 
void determineButtonEvent();
void readSticks();   
void computeChannelOutputs(); 

//Helpers
int deadzoneAndMap(int _input, int _minVal, int _centerVal, int _maxVal, int _deadzn, int _mapMin, int _mapMax);
int calcRateExpo(int _input, int _rate, int _expo);
int linearInterpolate(int xValues[], int yValues[], uint8_t numValues, int pointX);
int applySlow(int _currentVal, int _targetVal, uint16_t _riseTime, uint16_t _fallTime);
long weightAndOffset(int _input, int _weight, int _diff, int _offset);
bool mixSwitchIsActive(uint8_t _mixNum);
void evaluateTimer1(int16_t srcVal);

//==================================================================================================

void readSwitchesAndButtons()
{
  digitalWrite(PIN_COL3, LOW);
  digitalWrite(PIN_COL2, LOW);
  digitalWrite(PIN_COL1, HIGH);
  bool _swa = digitalRead(PIN_ROW1); 
  uint8_t _selectKey = digitalRead(PIN_ROW2);
  
  digitalWrite(PIN_COL1, LOW);
  digitalWrite(PIN_COL2, HIGH);
  bool _swb = digitalRead(PIN_ROW1);
  uint8_t _upKey = digitalRead(PIN_ROW2);
  
  digitalWrite(PIN_COL2, LOW);
  digitalWrite(PIN_COL3, HIGH);
  bool _swd = digitalRead(PIN_ROW1);
  uint8_t _downKey = digitalRead(PIN_ROW2);

  //Prevent phantom keys. (These arise if 2 or more buttons are pressed simultanouesly (missing some
  //diodes). This situation can falsely report switches and lead to loss of control :(
  uint8_t keySum = _selectKey + _upKey + _downKey;
  if(keySum > 1) //dont modify anything, just exit
  {
    return;
  }

  //assign
  SwAEngaged = _swa;
  SwBEngaged = _swb;
  SwDEngaged = _swd;
  //SwCState returned from slave mcu.

  //-- assign buttonCode --
  if(_selectKey) buttonCode = SELECT_KEY;
  else if(_upKey) buttonCode = UP_KEY;
  else if(_downKey) buttonCode = DOWN_KEY;
  else buttonCode = 0;
  
  if(buttonCode != 0)
    inputsLastMoved = millis();
  
  //-- play audio when switches are moved --
  uint8_t switchesSum = SwAEngaged + SwBEngaged + SwCState + SwDEngaged;
  static uint8_t lastSwitchesSum = 0;
  if(switchesSum != lastSwitchesSum)
  {
    if(thisLoopNum > 10UL) //prevent unneccesary beep on startup due to SwC
      audioToPlay = AUDIO_SWITCHMOVED;
    lastSwitchesSum = switchesSum;
    inputsLastMoved = millis();
  }
}

//==================================================================================================

void determineButtonEvent()
{
  /* 
  Modifies the pressedButton, clickedButton and heldButton variables, buttonStartTime 
  and buttonReleaseTime.
  Events
  - pressedButton is triggered once when the button goes down
  - clickedButton is triggered when the button is released before heldButton event
  - heldButton is triggered when button is held down long enough  
  */

  static uint8_t lastButtonCode = 0;
  static bool buttonActive = false, longPressActive = false;
  
  //clear events
  pressedButton = 0;
  clickedButton = 0;
  
  //button just went down
  if (buttonCode > 0 && buttonActive == false && (millis() - buttonReleaseTime > 100)) 
  {
    buttonActive = true;
    buttonStartTime = millis();
    lastButtonCode = buttonCode;
    pressedButton = buttonCode; //event
    audioToPlay = AUDIO_KEYTONE;
  }
  
  //button is down long enough
  if(buttonActive == true && longPressActive == false && (millis() - buttonStartTime > LONGPRESSTIME))
  {
    longPressActive = true;
    heldButton = buttonCode; //event
  }
  
  //button has just been released
  if(buttonCode == 0 && buttonActive == true)
  {
    buttonActive = false;
    buttonReleaseTime = millis();
    
    if(longPressActive == false)
      clickedButton = lastButtonCode; //event
    
    heldButton = 0; //clear event
    longPressActive = false;
  }
}

//==================================================================================================

void readSticks()
{
  if (isCalibratingSticks)
  {
    return;
  }

  rollIn  = analogRead(PIN_ROLL);
  pitchIn = analogRead(PIN_PITCH);
  throttleIn = analogRead(PIN_THROTTLE);
  yawIn  = analogRead(PIN_YAW);
  knobIn = analogRead(PIN_KNOB);

  //add deadzone to roll, pitch, yaw sticks centers. 
  rollIn  = deadzoneAndMap(rollIn, Sys.rollMin, Sys.rollCenterVal, Sys.rollMax, Sys.deadZonePerc, -500, 500);
  pitchIn = deadzoneAndMap(pitchIn, Sys.pitchMin, Sys.pitchCenterVal, Sys.pitchMax, Sys.deadZonePerc, -500, 500);
  yawIn   = deadzoneAndMap(yawIn, Sys.yawMin, Sys.yawCenterVal, Sys.yawMax, Sys.deadZonePerc, -500, 500);
  
  //add deadband at extremes of knob for stability
  knobIn = map(knobIn, 20, 1003, -500, 500); 
  knobIn = constrain(knobIn, -500, 500);

  throttleIn = map(throttleIn, Sys.thrtlMin, Sys.thrtlMax, -500, 500);
  throttleIn = constrain(throttleIn, -500, 500);
 
  //play audio whenever knob crosses center 
  enum {_POS_SIDE = 0, _CENTER = 1, _NEG_SIDE = 2};
  static uint8_t _knobRegion = _CENTER;
  if((knobIn >= 0 && _knobRegion == _NEG_SIDE) 
    || (knobIn < 0 && _knobRegion == _POS_SIDE)) //crossed center
  {
    audioToPlay = AUDIO_SWITCHMOVED;
    _knobRegion = _CENTER;
  }
  else if(knobIn > 25) _knobRegion = _POS_SIDE;
  else if(knobIn < -25) _knobRegion = _NEG_SIDE;
  
  //detect inactivity
  static int16_t _lastSticksAvg = 0;
  int16_t _sticksAvg = (rollIn + pitchIn + throttleIn + yawIn + knobIn) / 5;
  if(abs(_sticksAvg - _lastSticksAvg) > 6) //3% of 1000 is 30, divide by 5
  {
    _lastSticksAvg = _sticksAvg;
    inputsLastMoved = millis();
  }

}

//==================================================================================================

void computeChannelOutputs()
{
  if (isCalibratingSticks)
  {
    return;
  }

  ///Declare mix source array, init it to zero, then populate it afterwards 
  int MixSources[NUM_MIXSOURCES]; 
  memset(MixSources, 0, sizeof(MixSources));

  ///--Mix source sticks
  MixSources[IDX_ROLL] = rollIn;
  MixSources[IDX_PITCH] = pitchIn;
  MixSources[IDX_THRTL_RAW] = throttleIn;
  MixSources[IDX_YAW] = yawIn;
  MixSources[IDX_KNOB] = knobIn;
  
  ///--Mix source switches
  
  //Switch A
  if(SwAEngaged == true) 
    MixSources[IDX_SWA] = 500;
  else
    MixSources[IDX_SWA] = -500;
  
  //Switch B
  if(SwBEngaged == true) 
    MixSources[IDX_SWB] = 500;
  else
    MixSources[IDX_SWB] = -500;
  
  //Switch C (3 pos)
  if(SwCState == SWLOWERPOS) 
    MixSources[IDX_SWC] = 500;
  else if(SwCState == SWUPPERPOS)
    MixSources[IDX_SWC] = -500;
  
  //Switch D
  if(SwDEngaged == true) 
    MixSources[IDX_SWD] = 500;
  else 
    MixSources[IDX_SWD] = -500;
 
  ///--Mix source 100Perc
  MixSources[IDX_100PERC] = 500;
  
  ///--Mix source Ail, Ele, Thr, Rud, Custom curves
  
  //Ail
  if(SwBEngaged == false || ((Model.DualRate >> AILRTE) & 1) == 0)
    MixSources[IDX_AIL]  = calcRateExpo(rollIn,  Model.RateNormal[AILRTE], Model.ExpoNormal[AILRTE]);
  else
    MixSources[IDX_AIL]  = calcRateExpo(rollIn,  Model.RateSport[AILRTE], Model.ExpoSport[AILRTE]);
  MixSources[IDX_AIL] += 5 * Model.Trim[0];
  MixSources[IDX_AIL] = constrain(MixSources[IDX_AIL], -500, 500);
  
  //Ele
  if(SwBEngaged == false || ((Model.DualRate >> ELERTE) & 1) == 0)
    MixSources[IDX_ELE]  = calcRateExpo(pitchIn, Model.RateNormal[ELERTE], Model.ExpoNormal[ELERTE]);
  else
    MixSources[IDX_ELE]  = calcRateExpo(pitchIn, Model.RateSport[ELERTE], Model.ExpoSport[ELERTE]);
  MixSources[IDX_ELE] += 5 * Model.Trim[1];
  MixSources[IDX_ELE] = constrain(MixSources[IDX_ELE], -500, 500);
  
  //Rud
  if(SwBEngaged == false || ((Model.DualRate >> RUDRTE) & 1) == 0)
    MixSources[IDX_RUD] = calcRateExpo(yawIn, Model.RateNormal[RUDRTE], Model.ExpoNormal[RUDRTE]);
  else
    MixSources[IDX_RUD] = calcRateExpo(yawIn, Model.RateSport[RUDRTE], Model.ExpoSport[RUDRTE]);
  MixSources[IDX_RUD] += 5 * Model.Trim[3];
  MixSources[IDX_RUD] = constrain(MixSources[IDX_RUD], -500, 500);
  
  //Thr
  int xpoints[5] = {-500, -250, 0, 250, 500};
  int ypoints[5];
  for(uint8_t i = 0; i < 5; i++)
    ypoints[i] = 5 * Model.ThrottlePts[i];
  MixSources[IDX_THRTL_CURV] = linearInterpolate(xpoints, ypoints, 5, throttleIn);
  MixSources[IDX_THRTL_CURV] += 5 * Model.Trim[2];
  MixSources[IDX_THRTL_CURV] = constrain(MixSources[IDX_THRTL_CURV], -500, 500);
  
  //custom curve 1
  curve1SrcVal = MixSources[Model.Curve1Src];
  for(uint8_t i = 0; i < 5; i++)
    ypoints[i] = 5 * Model.Curve1Pts[i];
  MixSources[IDX_CRV1] = linearInterpolate(xpoints, ypoints, 5, curve1SrcVal);
  
  ///--Mix source Slow1
  static int _valueNow = MixSources[IDX_SLOW1];
  MixSources[IDX_SLOW1] = applySlow(_valueNow, MixSources[Model.Slow1Src], Model.Slow1Up, Model.Slow1Down);
  _valueNow = MixSources[IDX_SLOW1];
  
  ///--Predefined mixes
  //So we don't waste the limited mixer slots
  MixSources[IDX_CH1] = MixSources[IDX_AIL];  //send Ail  to Ch1
  MixSources[IDX_CH2] = MixSources[IDX_ELE];  //send Ele  to Ch2
  MixSources[IDX_CH3] = MixSources[IDX_THRTL_CURV]; //Send Thrt to Ch3
  MixSources[IDX_CH4] = MixSources[IDX_RUD];  //Send Rud  to Ch4
  
  ///--FREE MIXER
  for(uint8_t _mixNum = 0; _mixNum < NUM_MIXSLOTS; _mixNum++)
  {
    if(Model.MixOut[_mixNum] == IDX_NONE) //skip to next iteration
      continue;
    
    //---Input1---
    long _operand1 = 0;
    if(Model.MixIn1[_mixNum] != IDX_NONE) 
    {
      _operand1 = weightAndOffset(MixSources[Model.MixIn1[_mixNum]], Model.MixIn1Weight[_mixNum], 
                                  Model.MixIn1Diff[_mixNum], Model.MixIn1Offset[_mixNum]);
    }
    //---Input2---
    long _operand2 = 0;
    if(Model.MixIn2[_mixNum] != IDX_NONE) 
    {
      _operand2 = weightAndOffset( MixSources[Model.MixIn2[_mixNum]], Model.MixIn2Weight[_mixNum], 
                                   Model.MixIn2Diff[_mixNum], Model.MixIn2Offset[_mixNum]);
    }
    
    //--- Mix the inputs ---
    long _output = _operand1;
    if(mixSwitchIsActive(_mixNum))
    {
      switch(Model.MixOperator[_mixNum])
      {
        case MIX_ADD:
          _output += _operand2;
          break;
        case MIX_MULTIPLY:
          _output *= _operand2;
          _output /= 500; 
          break;
        case MIX_REPLACE:
          _output = _operand2;
          break; 
      }
    }
    
    //--- Clamp -----
    _output = constrain(_output, -500, 500);
    
    //---Update sources array for next iteration---
    if(Model.MixOut[_mixNum] < NUM_MIXSOURCES) //protects from potential overruns
      MixSources[Model.MixOut[_mixNum]] = int(_output);
  }

  ///WRITE TO CHANNELS
  for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
  {
    ChOut[i] = MixSources[IDX_CH1 + i];
    
    //export for graphing
    mixerChOutGraphVals[i] = ChOut[i] / 5; //divide by 5 to fit datatype
    
    //---Reverse
    if (((Model.Reverse >> i) & 0x01) == 1) 
      ChOut[i] = 0 - ChOut[i]; 

    //---Subtrim
    ChOut[i] += 5 * Model.Subtrim[i]; 
    
    //---Endpoints
    ChOut[i] = constrain(ChOut[i], 5 * Model.EndpointL[i], 5 * Model.EndpointR[i]); 
  }
  
  ///EVALUATE TIMER1
  if(Model.Timer1ControlSrc >= IDX_CH1 && Model.Timer1ControlSrc < (IDX_CH1 + NUM_PRP_CHANNLES))
    evaluateTimer1(ChOut[Model.Timer1ControlSrc - IDX_CH1]);
  else
    evaluateTimer1(MixSources[Model.Timer1ControlSrc]);
}

//====================================Helpers=======================================================

int deadzoneAndMap(int _input, int _minVal, int _centerVal, int _maxVal, int _deadzn, int _mapMin, int _mapMax)
{
  long _ddZnTmp = (long)(_maxVal - _minVal) * _deadzn;
  _ddZnTmp /= 100;
  int _ddZnVal = int(_ddZnTmp) / 2; //divide by 2 as we apply deadzone about center
  int _mapCenter = (_mapMin / 2) + (_mapMax / 2);
  //add dead zone and map the input
  int _output;
  if (_input > _centerVal + _ddZnVal)
    _output = map(_input, _centerVal + _ddZnVal, _maxVal, _mapCenter + 1, _mapMax);
  else if (_input < _centerVal - _ddZnVal)
    _output = map(_input, _minVal, _centerVal - _ddZnVal, _mapMin, _mapCenter - 1);
  else
    _output = _mapCenter;
  
  _output = constrain(_output, _mapMin, _mapMax);
  return _output;
}

//--------------------------------------------------------------------------------------------------

int calcRateExpo(int _input, int _rate, int _expo)
{
  /* This function is for applying rate and cubic 'expo' to aileron, elevator and rudder channels.
     Ranges: 
     _input  -500 to 500, 
     _rate   0 to 100
     _expo   -100 to 100. 0 is linear
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
  
  _expo += 100;
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

//--------------------------------------------------------------------------------------------------
int applySlow(int _currentVal, int _targetVal, uint16_t _riseTime, uint16_t _fallTime)
{
  //convert times to ms
  _riseTime *= 100; 
  _fallTime *= 100;
  
  //scale by 10
  _currentVal *= 10;
  _targetVal *= 10;
  
  //calc
  if(_currentVal < _targetVal && _riseTime > 0)
  {
    int _step = (1000L * fixedLoopTime * 10) / _riseTime;
    _currentVal += _step;
    if(_currentVal > _targetVal)
      _currentVal = _targetVal;
  }
  else if(_currentVal > _targetVal && _fallTime > 0) 
  {
    int _step = (1000L * fixedLoopTime * 10) / _fallTime;
    _currentVal -= _step;
    if(_currentVal < _targetVal)
      _currentVal = _targetVal;
  }
  else
  {
    _currentVal = _targetVal;
  }
  
  return _currentVal / 10; //scale back
}

//--------------------------------------------------------------------------------------------------

long weightAndOffset(int _input, int _weight, int _diff, int _offset)
{
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
  _outVal += _offset * 5;
  return _outVal;
}

//--------------------------------------------------------------------------------------------------

int linearInterpolate(int xValues[], int yValues[], uint8_t numValues, int pointX)
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
    for(uint8_t i = 0; i < numValues - 1; i++)
    {
      if(pointX >= xValues[i] && pointX <= xValues[i + 1])
      {
        long x0 = xValues[i];
        long x1 = xValues[i + 1];
        long y0 = yValues[i];
        long y1 = yValues[i + 1];
        long x = pointX;
        y = (((x - x0) * (y1 - y0)) + (y0 * (x1 - x0))) / (x1 - x0);
        break;
      }
    }
  }
  return int(y); 
}

//--------------------------------------------------------------------------------------------------

bool mixSwitchIsActive(uint8_t _mixNum)
{
  bool rslt = false;
  switch(Model.MixSwitch[_mixNum])
  {
    case SW_NONE: rslt = true; break; //SW_NONE is always active
    
    case SWA_UP:  rslt = !SwAEngaged; break;
    case SWA_DOWN:  rslt =  SwAEngaged;  break;
    
    case SWB_UP:  rslt = !SwBEngaged; break;
    case SWB_DOWN:  rslt =  SwBEngaged;  break;
    
    case SWC_UP:  rslt = SwCState == SWUPPERPOS? true : false; break;
    case SWC_MID: rslt = SwCState == SWMIDPOS?   true : false; break; 
    case SWC_DOWN:  rslt = SwCState == SWLOWERPOS? true : false; break;

    case SWC_NOT_UP:  rslt = SwCState == SWUPPERPOS? false : true; break;
    case SWC_NOT_MID: rslt = SwCState == SWMIDPOS?   false : true; break;
    case SWC_NOT_DOWN:  rslt = SwCState == SWLOWERPOS? false : true; break;
    
    case SWD_UP:  rslt = !SwDEngaged; break;
    case SWD_DOWN:  rslt =  SwDEngaged;  break;
  }
  return rslt;
}

//--------------------------------------------------------------------------------------------------

void evaluateTimer1(int16_t srcVal)
{
  bool timerPaused = true;

  if(Model.Timer1ControlSrc != IDX_NONE)
  {
    int16_t thresh = Model.Timer1Value * 5;
    switch(Model.Timer1Operator)
    {
      case GREATER_THAN:
        if(srcVal > thresh) timerPaused = false;
      break;
     
      case LESS_THAN:
        if(srcVal < thresh) timerPaused = false;
      break;
      
      case ABS_GREATER_THAN:
        if(abs(srcVal) > thresh) timerPaused = false;
      break;
      
      case ABS_LESS_THAN:
        if(abs(srcVal) < thresh) timerPaused = false;
      break;
    }
  }
  
  if(timerPaused)
  {
    timer1LastElapsedTime = timer1ElapsedTime;
    timer1LastPaused = millis();
  }
  else
    timer1ElapsedTime = timer1LastElapsedTime + millis() - timer1LastPaused;
}
