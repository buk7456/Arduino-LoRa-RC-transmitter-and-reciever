#include "Arduino.h"
#include "config.h"
#include "common.h"
#include "io.h"

int deadzoneAndMap(int _input, int _minVal, int _centerVal, int _maxVal, int _deadzn, int _mapMin, int _mapMax);
int linearInterpolate(int xValues[], int yValues[], uint8_t numValues, int pointX);
int applySlow(int _currentVal, int _targetVal, uint16_t _riseTime, uint16_t _fallTime);
int weightAndOffset(int _input, int _weight, int _offset, int _diff);
bool mixSwitchIsActive(uint8_t _mixNum);
void evaluateTimer1(int16_t srcVal);
int generateWaveform();

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
  swAEngaged = _swa;
  swBEngaged = _swb;
  swDEngaged = _swd;
  //SwC, SwE, SwF read by slave mcu
  
  //-- assign buttonCode --
  if(_selectKey) buttonCode = SELECT_KEY;
  else if(_upKey) buttonCode = UP_KEY;
  else if(_downKey) buttonCode = DOWN_KEY;
  else buttonCode = 0;
  
  if(buttonCode != 0)
    inputsLastMoved = millis();
  
  //-- play audio when switches are moved --
  uint8_t switchesSum = swAEngaged + swBEngaged + swCState + swDEngaged + swEEngaged + swFEngaged;
  static uint8_t lastSwitchesSum = 0;
  if(switchesSum != lastSwitchesSum)
  {
    if(thisLoopNum > 10) //prevent unneccesary beep on startup
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
  if (buttonCode > 0 && !buttonActive && (millis() - buttonReleaseTime > 100)) 
  {
    buttonActive = true;
    buttonStartTime = millis();
    lastButtonCode = buttonCode;
    pressedButton = buttonCode; //event
    audioToPlay = AUDIO_KEYTONE;
  }
  
  //button is down long enough
  if(buttonActive && !longPressActive && (millis() - buttonStartTime > LONGPRESSTIME))
  {
    longPressActive = true;
    heldButton = buttonCode; //event
  }
  
  //button has just been released
  if(buttonCode == 0 && buttonActive)
  {
    buttonActive = false;
    buttonReleaseTime = millis();
    
    if(!longPressActive)
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
  int mixSources[NUM_MIXSOURCES]; 
  memset(mixSources, 0, sizeof(mixSources));

  ///--Mix source sticks
  mixSources[IDX_ROLL] = rollIn;
  mixSources[IDX_PITCH] = pitchIn;
  mixSources[IDX_THRTL_RAW] = throttleIn;
  mixSources[IDX_YAW] = yawIn;
  mixSources[IDX_KNOB] = knobIn;
  
  ///--Mix source switches
  mixSources[IDX_SWA] = swAEngaged ? 500 : -500;
  mixSources[IDX_SWB] = swBEngaged ? 500 : -500;
  mixSources[IDX_SWC] = (swCState == SWLOWERPOS) ? 500 : ((swCState == SWUPPERPOS) ? -500 : 0);
  mixSources[IDX_SWD] = swDEngaged ? 500 : -500;
  mixSources[IDX_SWE] = swEEngaged ? 500 : -500;
  mixSources[IDX_SWF] = swFEngaged ? 500 : -500;
  
  ///--Mix source 100Perc
  mixSources[IDX_100PERC] = 500;
  
  ///--Mix source Ail, Ele, Rud
  int _stickIn[3] = {rollIn, pitchIn, yawIn};
  uint8_t _idx[3] = {IDX_AIL, IDX_ELE, IDX_RUD};
  for(uint8_t i = 0; i < 3; i++)
  {
    if(!swBEngaged || ((Model.dualRate >> i) & 1) == 0)
      mixSources[_idx[i]]  = calcRateExpo(_stickIn[i],  Model.rateNormal[i], Model.expoNormal[i]);
    else
      mixSources[_idx[i]]  = calcRateExpo(_stickIn[i],  Model.rateSport[i], Model.expoSport[i]);
  }
  
  ///--Mix source throttle curve
  int xpoints[5] = {-500, -250, 0, 250, 500};
  int ypoints[5];
  for(uint8_t i = 0; i < 5; i++)
    ypoints[i] = 5 * Model.throttlePts[i];
  mixSources[IDX_THRTL_CURV] = linearInterpolate(xpoints, ypoints, 5, throttleIn);
  
  ///--Mix source Slow1
  static int _valueNow = mixSources[IDX_SLOW1];
  mixSources[IDX_SLOW1] = applySlow(_valueNow, mixSources[Model.slow1Src], Model.slow1Up * 100, Model.slow1Down * 100);
  _valueNow = mixSources[IDX_SLOW1];
  
  ///--Mix source FuncGen
  mixSources[IDX_FUNCGEN] = generateWaveform();
  
  ///--Predefined mixes
  //So we don't waste the limited mixer slots
  //send Ail  to Ch1
  mixSources[IDX_CH1] = mixSources[IDX_AIL] + 5 * Model.trim[0];        
  mixSources[IDX_CH1] = constrain(mixSources[IDX_CH1], -500, 500);
  //send Ele  to Ch2
  mixSources[IDX_CH2] = mixSources[IDX_ELE] + 5 * Model.trim[1];        
  mixSources[IDX_CH2] = constrain(mixSources[IDX_CH2], -500, 500);
  //Send Thrt to Ch3
  mixSources[IDX_CH3] = mixSources[IDX_THRTL_CURV] + 5 * Model.trim[2]; 
  mixSources[IDX_CH3] = constrain(mixSources[IDX_CH3], -500, 500);
  //Send Rud  to Ch4
  mixSources[IDX_CH4] = mixSources[IDX_RUD] + 5 * Model.trim[3];        
  mixSources[IDX_CH4] = constrain(mixSources[IDX_CH4], -500, 500);
  
  ///--FREE MIXER
  for(uint8_t _mixNum = 0; _mixNum < NUM_MIXSLOTS; _mixNum++)
  {
    if(Model.mixOut[_mixNum] == IDX_NONE) //skip to next iteration
      continue;
    
    //---Input1---
    long _operand1 = 0;
    if(Model.mixIn1[_mixNum] != IDX_NONE) 
    {
      _operand1 = weightAndOffset(mixSources[Model.mixIn1[_mixNum]], 
                                  Model.mixIn1Weight[_mixNum], 
                                  Model.mixIn1Offset[_mixNum], 
                                  Model.mixIn1Diff[_mixNum]);
      
      //Handle trim here so that differential works as expected
      if(Model.mixIn1[_mixNum] >= IDX_AIL && Model.mixIn1[_mixNum] <= IDX_RUD)
      {
        uint8_t _idxTrim = Model.mixIn1[_mixNum] - IDX_AIL;
        int _trim = Model.trim[_idxTrim];
        _operand1 += (_trim * 5 * Model.mixIn1Weight[_mixNum]) / 100;
      } 
      
      _operand1 = constrain(_operand1, -500, 500);
    }
    //---Input2---
    long _operand2 = 0;
    if(Model.mixIn2[_mixNum] != IDX_NONE) 
    {
      _operand2 = weightAndOffset(mixSources[Model.mixIn2[_mixNum]], 
                                  Model.mixIn2Weight[_mixNum], 
                                  Model.mixIn2Offset[_mixNum], 
                                  Model.mixIn2Diff[_mixNum]);
                                  
      //Handle trim here so that differential works as expected
      if(Model.mixIn2[_mixNum] >= IDX_AIL && Model.mixIn2[_mixNum] <= IDX_RUD)
      {
        uint8_t _idxTrim = Model.mixIn2[_mixNum] - IDX_AIL;
        int _trim = Model.trim[_idxTrim];
        _operand2 += (_trim * 5 * Model.mixIn2Weight[_mixNum]) / 100;
      } 
      
      _operand2 = constrain(_operand2, -500, 500);
    }
    
    //--- Mix the inputs ---
    long _output = _operand1;
    if(mixSwitchIsActive(_mixNum))
    {
      uint8_t _mixOper = Model.mixOper_N_Switch[_mixNum] >> 6;
      switch(_mixOper)
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
    if(Model.mixOut[_mixNum] < NUM_MIXSOURCES) //protects from potential overruns
      mixSources[Model.mixOut[_mixNum]] = int(_output);
  }

  ///WRITE TO CHANNELS
  for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
  {
    channelOut[i] = mixSources[IDX_CH1 + i];
    
    //export for graphing
    mixerChOutGraphVals[i] = channelOut[i] / 5; //divide by 5 to fit datatype
    
    //---reverse
    if (((Model.reverse >> i) & 0x01) == 1) 
      channelOut[i] = 0 - channelOut[i]; 

    //---subtrim
    channelOut[i] += 5 * Model.subtrim[i]; 
    
    //---Endpoints
    channelOut[i] = constrain(channelOut[i], 5 * Model.endpointL[i], 5 * Model.endpointR[i]); 
  }
  
  ///EVALUATE TIMER1
  if(Model.timer1ControlSrc >= IDX_CH1 && Model.timer1ControlSrc < (IDX_CH1 + NUM_PRP_CHANNLES))
    evaluateTimer1(channelOut[Model.timer1ControlSrc - IDX_CH1]);
  else
    evaluateTimer1(mixSources[Model.timer1ControlSrc]);
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
  y *= _rate;
  y /= 100;
  
  if(_input < 0) 
    y = -y;
    
  return int(y);
}

//--------------------------------------------------------------------------------------------------
int applySlow(int _currentVal, int _targetVal, uint16_t _riseTime, uint16_t _fallTime)
{
  if(_currentVal < _targetVal && _riseTime > 0)
  {
    int _step = (1000L * fixedLoopTime) / _riseTime;
    _currentVal += _step;
    if(_currentVal > _targetVal)
      _currentVal = _targetVal;
  }
  else if(_currentVal > _targetVal && _fallTime > 0) 
  {
    int _step = (1000L * fixedLoopTime) / _fallTime;
    _currentVal -= _step;
    if(_currentVal < _targetVal)
      _currentVal = _targetVal;
  }
  else
  {
    _currentVal = _targetVal;
  }
  
  return _currentVal;
}

//--------------------------------------------------------------------------------------------------

int weightAndOffset(int _input, int _weight, int _offset, int _diff)
{
  //apply weight 
  long _outVal = _input;
  _outVal *= _weight;
  _outVal /= 100;
  
  //apply offset
  _outVal += _offset * 5;
  
  //apply differential
  if(_diff > 0 && _outVal < 0)
  {
    _outVal *= (100 - _diff);
    _outVal /= 100;
  }
  else if(_diff < 0 && _outVal > 0)
  {
    _outVal *= (100 + _diff);
    _outVal /= 100;
  }

  return int(_outVal);
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
  uint8_t _mixSw = Model.mixOper_N_Switch[_mixNum] & 0x3F;
  switch(_mixSw)
  {
    case SW_NONE:   rslt = true; break; //SW_NONE is always active
    
    case SWA_UP:    rslt = !swAEngaged;  break;
    case SWA_DOWN:  rslt =  swAEngaged;  break;
    
    case SWB_UP:    rslt = !swBEngaged;  break;
    case SWB_DOWN:  rslt =  swBEngaged;  break;
    
    case SWC_UP:    rslt = (swCState == SWUPPERPOS); break;
    case SWC_MID:   rslt = (swCState == SWMIDPOS);   break; 
    case SWC_DOWN:  rslt = (swCState == SWLOWERPOS); break;

    case SWC_NOT_UP:   rslt = (swCState != SWUPPERPOS); break;
    case SWC_NOT_MID:  rslt = (swCState != SWMIDPOS);   break;
    case SWC_NOT_DOWN: rslt = (swCState != SWLOWERPOS); break;
    
    case SWD_UP:    rslt = !swDEngaged;  break;
    case SWD_DOWN:  rslt =  swDEngaged;  break;
    
    case SWE_UP:    rslt = !swEEngaged;  break;
    case SWE_DOWN:  rslt =  swEEngaged;  break;
    
    case SWF_UP:    rslt = !swFEngaged;  break;
    case SWF_DOWN:  rslt =  swFEngaged;  break;
  }
  return rslt;
}

//--------------------------------------------------------------------------------------------------

void evaluateTimer1(int16_t srcVal)
{
  bool timerPaused = true;

  if(Model.timer1ControlSrc != IDX_NONE)
  {
    int16_t thresh = Model.timer1Value * 5;
    switch(Model.timer1Operator)
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

//--------------------------------------------------------------------------------------------------

// Table for generating stepped sine waveform. 100 values
// Values have been offset about 100 so range is 0 to 200
static const uint8_t sineForm[] PROGMEM = {
  100, 106, 113, 119, 125, 131, 137, 143, 148, 154, 159, 164, 168, 173, 177, 181, 184, 188, 190, 
  193, 195, 197, 198, 199, 200, 
  200, 200, 199, 198, 197, 195, 193, 190, 188, 184, 181, 177, 173, 168, 164, 159, 154, 148, 143, 
  137, 131, 125, 119, 113, 106, 
  100, 94, 87, 81, 75, 69, 63, 57, 52, 46, 41, 36, 32, 27, 23, 19, 16, 12, 10, 7, 5, 3, 2, 1, 0, 
  0, 0, 1, 2, 3, 5, 7, 10, 12, 16, 19, 23, 27, 32, 36, 41, 46, 52, 57, 63, 69, 75, 81, 87, 94, 
};

int generateWaveform()
{
  int period = Model.funcgenPeriod * 100;
  long timeInstance = millis() % period;
  
  static int result = 0;
  if(isAdjustingFuncgenPeriod) //prevent unpredicatable result
  {
    isAdjustingFuncgenPeriod = false;
    return result;
  }
  
  switch(Model.funcgenWaveform)
  {
    case FUNC_SINE:
    {
      uint8_t index = (timeInstance * 99)/(period - 1);
      //get value at this index. Map index to a time value
      int valL = (5 * pgm_read_byte(&sineForm[index])) - 500;
      int tL = map(index, 0, 99, 0, period - 1);
      //increment index, get value, map index to a time value
      index++; if(index > 99) index = 0;
      int valR = (5 * pgm_read_byte(&sineForm[index])) - 500;
      int tR = map(index, 0, 99, 0, period - 1);
      //linear interpolate
      result = valL + (((timeInstance - tL)*(valR - valL))/(tR - tL));
    }
    break;
    
    case FUNC_SAWTOOTH:
    {
      result = map(timeInstance, 0, period - 1, -500, 500);
    }
    break;
    
    case FUNC_TRIANGLE:
    {
      if(timeInstance < (period/2))
        result = map(timeInstance, 0, period/2 - 1, -500, 500);
      else
        result = map(timeInstance, period/2, period - 1, 500, -500);
    }
    break;
    
    case FUNC_SQUARE:
    {
      if(timeInstance < (period/2))
        result = -500;
      else
        result = 500;
    }
    break;
  }
  
  return result;
}
