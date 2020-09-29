
//Called in main loop
void readSwitchesAndButtons(); 
void determineButtonEvent();
void readSticks();   
void computeChannelOutputs(); 

//Helpers

int deadzoneAndMap(int _input, int _minVal, int _centerVal, int _maxVal, int _deadzn, int _mapMin, int _mapMax);
int calcRateExpo(int _input, int _rate, int _expo);
int linearInterpolate(int xValues[], int yValues[], int numValues, int pointX);
long weightAndOffset(int _input, int _weight, int _diff, int _offset);

bool cutIsActivated();

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
  //SwCState gotten from slave mcu.
  SwCState = (returnedByte >> 6) & 0x03;

  //-- assign buttonCode --
  if(_selectKey) buttonCode = SELECT_KEY;
  else if(_upKey) buttonCode = UP_KEY;
  else if(_downKey) buttonCode = DOWN_KEY;
  else buttonCode = 0;
  
  //-- play audio when switches are moved --
  uint8_t switchesSum = SwAEngaged + SwBEngaged + SwCState + SwDEngaged;
  static uint8_t lastSwitchesSum = switchesSum;
  if(switchesSum != lastSwitchesSum)
    audioToPlay = AUDIO_SWITCHMOVED;
  lastSwitchesSum = switchesSum;
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
  
  pressedButton = 0; //clear 
  
  if (buttonCode > 0 && (millis() - buttonReleaseTime > 100)) //button down  
  {
    if (buttonActive == false)
    {
      buttonActive = true;
      buttonStartTime = millis();
      lastButtonCode = buttonCode;
      pressedButton = buttonCode; //event
    }

    if ((millis() - buttonStartTime > LONGPRESSTIME) && longPressActive == false)
    {
      longPressActive = true;
      heldButton = buttonCode; //event
    }
  }

  else //button released
  {
    if(buttonActive == true)
    {
      buttonReleaseTime = millis();
      buttonActive = false;
    }
    
    if (longPressActive == true)
    {
      longPressActive = false;
      heldButton = 0;
      lastButtonCode = 0; //avoids falsely triggering clickedButton event
    }
    else
    {
      clickedButton = lastButtonCode;
      lastButtonCode = 0; //enables setting clickedButton event only once
    }
  }
  
  //play tones
  if(pressedButton > 0) 
    audioToPlay = AUDIO_KEYTONE;
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
  aux2In = analogRead(PIN_KNOB);

  //add deadzone to roll, pitch, yaw sticks centers. 
  rollIn  = deadzoneAndMap(rollIn, Sys.rollMin, Sys.rollCenterVal, Sys.rollMax, Sys.deadZonePerc, -500, 500);
  pitchIn = deadzoneAndMap(pitchIn, Sys.pitchMin, Sys.pitchCenterVal, Sys.pitchMax, Sys.deadZonePerc, -500, 500);
  yawIn   = deadzoneAndMap(yawIn, Sys.yawMin, Sys.yawCenterVal, Sys.yawMax, Sys.deadZonePerc, -500, 500);
  
  //add deadband at extremes of knob for stability
  aux2In = map(aux2In, 20, 1003, -500, 500); 
  aux2In = constrain(aux2In, -500, 500);

  throttleIn = map(throttleIn, Sys.thrtlMin, Sys.thrtlMax, -500, 500);
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
  else if(aux2In > _posSideStart) _aux2Region = _POS_SIDE;
  else if(aux2In < _negSideStart) _aux2Region = _NEG_SIDE;
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
  for(int i = 0; i < NUM_MIXSOURCES; i++) 
    MixSources[i] = 0;

  ///--Mix source sticks
  MixSources[IDX_ROLL] = rollIn;
  MixSources[IDX_PITCH] = pitchIn;
  MixSources[IDX_THRTL_RAW] = throttleIn;
  MixSources[IDX_YAW] = yawIn;
  MixSources[IDX_KNOB] = aux2In;
  
  ///--Mix source Switches
  
  //switches are slowed down to prevent abrupt change
  int _delta = 2 * fixedLoopTime;
  
  //Switch A
  static int _SwAVal = -500;
  if(SwAEngaged == true) _SwAVal += _delta;
  else _SwAVal -= _delta;
  _SwAVal = constrain(_SwAVal, -500, 500);
  MixSources[IDX_SWA] = _SwAVal;
  
  //Switch B
  static int _SwBVal = -500;
  if(SwBEngaged == true) _SwBVal += _delta;
  else _SwBVal -= _delta;
  _SwBVal = constrain(_SwBVal, -500, 500);
  MixSources[IDX_SWB] = _SwBVal;
  
  //Switch C (3 pos)
  static int _SwCVal = 0;
  int _target;
  if(SwCState == SWLOWERPOS) _target = 500;
  else if(SwCState == SWUPPERPOS) _target = -500;
  else _target = 0;
  if(_target < _SwCVal) 
  { 
    _SwCVal -= _delta;
    if(_SwCVal < _target)
      _SwCVal = _target;
  }
  else if(_target > _SwCVal) 
  {
    _SwCVal += _delta;
    if(_SwCVal > _target)
      _SwCVal = _target;
  }
  MixSources[IDX_SWC] = _SwCVal;
  
  //Switch D
  static int _SwDVal = -500;
  if(SwDEngaged == true) _SwDVal += _delta;
  else _SwDVal -= _delta;
  _SwDVal = constrain(_SwDVal, -500, 500);
  MixSources[IDX_SWD] = _SwDVal;
  
  ///--Mix source Ail, Ele, Thr, Rud
  
  //Ail
  if(SwBEngaged == false || Model.DualRateEnabled[AILRTE] == false)
    MixSources[IDX_AIL]  = calcRateExpo(rollIn,  Model.RateNormal[AILRTE], Model.ExpoNormal[AILRTE]);
  else
    MixSources[IDX_AIL]  = calcRateExpo(rollIn,  Model.RateSport[AILRTE], Model.ExpoSport[AILRTE]);
  MixSources[IDX_AIL] += 5 * (Model.Trim[0] - 100);
  MixSources[IDX_AIL] = constrain(MixSources[IDX_AIL], -500, 500);
  
  //Ele
  if(SwBEngaged == false || Model.DualRateEnabled[ELERTE] == false)
    MixSources[IDX_ELE]  = calcRateExpo(pitchIn, Model.RateNormal[ELERTE], Model.ExpoNormal[ELERTE]);
  else
    MixSources[IDX_ELE]  = calcRateExpo(pitchIn, Model.RateSport[ELERTE], Model.ExpoSport[ELERTE]);
  MixSources[IDX_ELE] += 5 * (Model.Trim[1] - 100);
  MixSources[IDX_ELE] = constrain(MixSources[IDX_ELE], -500, 500);
  
  //Thr
  int xpoints[5] = {-500, -250, 0, 250, 500};
  int ypoints[5];
  for(int i = 0; i < 5; i++)
    ypoints[i] = 5 * (Model.ThrottlePts[i] - 100);
  MixSources[IDX_THRTL_CURV] = linearInterpolate(xpoints, ypoints, 5, throttleIn);
  MixSources[IDX_THRTL_CURV] += 5 * (Model.Trim[2] - 100);
  MixSources[IDX_THRTL_CURV] = constrain(MixSources[IDX_THRTL_CURV], -500, 500);
  
  //Rud
  if(SwBEngaged == false || Model.DualRateEnabled[RUDRTE] == false)
    MixSources[IDX_RUD] = calcRateExpo(yawIn, Model.RateNormal[RUDRTE], Model.ExpoNormal[RUDRTE]);
  else
    MixSources[IDX_RUD] = calcRateExpo(yawIn, Model.RateSport[RUDRTE], Model.ExpoSport[RUDRTE]);
  MixSources[IDX_RUD] += 5 * (Model.Trim[3] - 100);
  MixSources[IDX_RUD] = constrain(MixSources[IDX_RUD], -500, 500);

  ///--Predefined mixes
  //So we don't waste the limited mixer slots
  MixSources[IDX_CH1] = MixSources[IDX_AIL];  //send Ail  to Ch1
  MixSources[IDX_CH2] = MixSources[IDX_ELE];  //send Ele  to Ch2
  MixSources[IDX_CH3] = MixSources[IDX_THRTL_CURV]; //Send Thrt to Ch3
  MixSources[IDX_CH4] = MixSources[IDX_RUD];  //Send Rud  to Ch4
  
  ///--FREE MIXER
  //Takes two input sources, adds or multiplies them, then writes result to specified output
  for(int _mixNum = 0; _mixNum < NUM_MIXSLOTS; _mixNum++)
  {
    if(Model.MixOut[_mixNum] == IDX_NONE) //skip to next iteration
      continue;  
    
    //---Input1---
    long _operand1 = 0;
    if(Model.MixIn1[_mixNum] != IDX_NONE) //only calculate if input is other than "None"
    {
      _operand1 = weightAndOffset(MixSources[Model.MixIn1[_mixNum]], Model.MixIn1Weight[_mixNum], 
                                  Model.MixIn1Diff[_mixNum], Model.MixIn1Offset[_mixNum]);
    }
    //---Input2---
    long _operand2 = 0;
    if(Model.MixIn2[_mixNum] != IDX_NONE) //only calculate if input is other than "None"
    {
      _operand2 = weightAndOffset( MixSources[Model.MixIn2[_mixNum]], Model.MixIn2Weight[_mixNum], 
                                   Model.MixIn2Diff[_mixNum], Model.MixIn2Offset[_mixNum]);
    }
    //---Mix the two inputs---
    long _output;      
    if(Model.MixOperator[_mixNum] == 0) //add
      _output = _operand1 + _operand2; 
    else //multiply
      _output = (_operand1 * _operand2) / 500; 
    //---Clamp-----
    _output = constrain(_output, -500, 500);
    //---Update sources array for next iteration--- 
    MixSources[Model.MixOut[_mixNum]] = int(_output);
  }
  
  //export mix results for graphing
  for(int i = 0; i < NUM_PRP_CHANNLES; i++)
    mixerChOutGraphVals[i] = MixSources[IDX_CH1 + i] / 5; //divide by 5 to fit datatype
  
  ///WRITE TO CHANNELS
  for(int i = 0; i < NUM_PRP_CHANNLES; i++)
  {
    ChOut[i] = MixSources[IDX_CH1 + i];
   
    //---Reverse
    if (Model.Reverse[i] == true) 
      ChOut[i] = 0 - ChOut[i]; 

    //---Subtrim
    ChOut[i] += 5 * (Model.Subtrim[i] - 50); 
    
    //---Check Cut. If specified and switch engaged, overide
    if(SwAEngaged == true && Model.CutValue[i] > 0)
      ChOut[i] = 5 * (Model.CutValue[i] - 101);
    
    //---Endpoints
    ChOut[i] = constrain(ChOut[i], 5 * (0 - Model.EndpointL[i]), 5 * Model.EndpointR[i]); 
  }
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
  
  _input = constrain(_input, -500, 500); //limit input 
  
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

//--------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------

bool cutIsActivated()
{
  for(int i = 0; i < NUM_PRP_CHANNLES; i++)
  {
    if(Model.CutValue[i] > 0 && SwAEngaged == false)
    {
      return true;
    }
  }
  return false;
}
