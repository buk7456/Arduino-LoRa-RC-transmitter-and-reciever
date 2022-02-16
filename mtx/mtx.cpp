
#include "Arduino.h"
#include <EEPROM.h>
#include <SPI.h>

#include "config.h"
#include "common.h"
#include "eestore.h"
#include "io.h"
#include "ui_128x64.h" 
#include "crc8.h"

bool isRequestingPowerOff = false;

// Declarations 
void sendSerialData(); 
void getSerialData();
void checkBattery();
uint16_t joinBytes(uint8_t _highByte, uint8_t _lowByte); 

//===================================== setup ======================================================

void setup()
{
  //set up pins
  pinMode(PIN_ROW1, INPUT);
  pinMode(PIN_ROW2, INPUT);
  pinMode(PIN_COL1, OUTPUT);
  pinMode(PIN_COL2, OUTPUT);
  pinMode(PIN_COL3, OUTPUT);
  
  //init spi 
  SPI.begin(); 

  //init serial port
  Serial.begin(UART_BAUD_RATE);
  
  //init display
  initialiseDisplay();
  
  //initialise model storage
  eeStoreInit();
  
  //set defaults
  setDefaultSystemParams();
  setDefaultModelName();
  setDefaultModelBasicParams();
  setDefaultModelMixerParams();
  
  ///-------------- Startup menu ----------------
  readSwitchesAndButtons();
  if(buttonCode == SELECT_KEY)
    handleStartupMenu(); //blocking
  
  ///------------- EEPROM check ------------------
  
  uint8_t eeInitFlag = crc8Maxim((uint8_t *) &Sys, sizeof(Sys)) ^ crc8Maxim((uint8_t *) &Model, sizeof(Model));
  
  bool formatEE = false;
 
  /// Check signature. If its not matching, then assume it's a fresh mcu and format the eeprom
  uint8_t fileSignature;
  EEPROM.get(EE_FILE_SIGNATURE_ADDR, fileSignature);
  if(fileSignature != EE_FILE_SIGNATURE) //force format
  {
    showEEWarning();
    while(buttonCode == 0)
    {
      readSwitchesAndButtons();
      delay(30);
    }
    
    buttonCode = 0; 
    formatEE = true;
  }
  ///Check flag. Signature may match but not the data structs
  else if(EEPROM.read(EE_INITFLAG_ADDR) != eeInitFlag)
  {
    showEEFormatConfirmation();
    while(buttonCode != UP_KEY && buttonCode != DOWN_KEY)
    {
      readSwitchesAndButtons();
      delay(30);
    }
    
    if(buttonCode == UP_KEY) //format
      formatEE = true;
    else if(buttonCode == DOWN_KEY)
      EEPROM.write(EE_INITFLAG_ADDR, eeInitFlag);
    
    buttonCode = 0;
  }
  
  if(formatEE)
  {
    showFormattingMsg();
    delay(500);
    
    //erase eeprom
    eraseEEPROM();
    
    //write system data
    eeSaveSysConfig();
    //create a model in slot 1
    eeCreateModel(1);
    
    //write flag
    EEPROM.write(EE_INITFLAG_ADDR, eeInitFlag);
    //write signature
    fileSignature = EE_FILE_SIGNATURE;
    EEPROM.put(EE_FILE_SIGNATURE_ADDR, fileSignature);
    
    startStickCalibration();
    skipThrottleCheck = true;
  }
 
  ///--------- Init battery reading ------------
  battVoltsNow = battVoltsMax; 
  for(uint8_t i = 0; i < 50; i++)
  {
    checkBattery();
    delay(2);
  }
  
  ///----------- Play animation -----------------
  uint32_t qq = millis() + 1000;
  while(millis() < qq)
    showAnimation();

  ///--------- Load data from eeprom -----------
  eeReadSysConfig();
  eeReadModelData(Sys.activeModel);

  ///--------- Warn if throttle is not low -----
  if(!skipThrottleCheck)
  {
    readSticks();
    bool _rfState = Sys.rfOutputEnabled;
    while (throttleIn > -450)
    {
      showThrottleWarning();
      readSticks();
      //play warning sound
      audioToPlay = AUDIO_THROTTLEWARN;
      Sys.rfOutputEnabled = false; //overide
      sendSerialData();
      getSerialData();
      delay(30);
    }
    Sys.rfOutputEnabled = _rfState; //restore
  }

  ///--------- Init timers ---------------------
  timer1LastPaused = millis(); 

}

//===================================== main =======================================================

void loop()
{
  unsigned long loopStartTime = millis();
  thisLoopNum++;
  
  checkBattery();
  readSwitchesAndButtons();
  determineButtonEvent();
  readSticks();
  computeChannelOutputs();
  handleMainUI();
  sendSerialData();
  getSerialData();
  
  //limit max rate of loop
  unsigned long loopTime = millis() - loopStartTime;
  if(loopTime < fixedLoopTime) 
    delay(fixedLoopTime - loopTime);
  
}

//==================================================================================================

void sendSerialData()
{
  /* 
    MASTER MCU TO SLAVE MCU COMMUNICATION  
    - The transmit length is always fixed at 24 bytes.
    - General format is as below.
    ---------------------------------------------------------------
      Description |  Status0  Status1  Audio   GeneralData  CRC8
      Size        |  1 byte   1 byte   1 byte  20 bytes     1 byte
      Offset      |  0        1        2       3            23
    ---------------------------------------------------------------
  */

  /* Status0 
      bit0-2 RF power level
      bit3   RF enabled
      bit4   Backlight
      bit5   Power off
  */
  enum {
    FLAG_RF_ENABLED  = 0x08,
    FLAG_BACKLIGHT   = 0x10,
    FLAG_POWER_OFF   = 0x20,
  };
  
  /* Status1
      bit0  failsafe data
      bit1  write receiver config
      bit2  get receiver config 
      bit3  get telemetry
      bit4  enter bind mode 
  */
  enum {
    FLAG_FAILSAFE_DATA    = 0x01,
    FLAG_WRITE_RX_CONFIG  = 0x02,
    FLAG_GET_RX_CONFIG    = 0x04,
    FLAG_GET_TELEMETRY    = 0x08,
    FLAG_ENTER_BIND       = 0x10,
  };


  ///----------- ENCODE ---------

  uint8_t status0 = 0;
  
  //rf power level
  status0 |= (Sys.rfPower & 0x07);
  
  //rf enabled
  if(Sys.rfOutputEnabled)
    status0 |= FLAG_RF_ENABLED;
  
  //power off 
  if(isRequestingPowerOff)
    status0 |= FLAG_POWER_OFF;
  
  //backlight 
  static unsigned long lastBtnDownTime = 0;
  if(buttonCode > 0) 
    lastBtnDownTime = millis();
  unsigned long elapsed = millis() - lastBtnDownTime;
  if(Sys.backlightMode == BACKLIGHT_ON 
     || (Sys.backlightMode == BACKLIGHT_5S  && elapsed < 5000UL )
     || (Sys.backlightMode == BACKLIGHT_15S && elapsed < 15000UL)
     || (Sys.backlightMode == BACKLIGHT_60S && elapsed < 60000UL))
  {     
   status0 |= FLAG_BACKLIGHT;
  }
  
  uint8_t status1 = 0;
  
  //bind
  if(isRequestingBind)
  {
    status1 |= FLAG_ENTER_BIND;
    isRequestingBind = false;
  }
  
  //alternately send failsafe or request telemetry every 300ms
  static bool isRequestingTelemetry = false;
  if(thisLoopNum % (300 / fixedLoopTime) == 1) 
  {
    isRequestingTelemetry = !isRequestingTelemetry;
    if(isRequestingTelemetry)
      status1 |= FLAG_GET_TELEMETRY;
    else
      status1 |= FLAG_FAILSAFE_DATA;
  }
  
  //requesting receiver configuration
  if(isRequestingOutputChConfig)
  {
    status1 |= FLAG_GET_RX_CONFIG;
    isRequestingOutputChConfig = false;
    //unset other flags
    status1 &= ~FLAG_GET_TELEMETRY; 
  }
  
  //sending receiver configuration
  if(sendOutputChConfig)
  {
    status1 |= FLAG_WRITE_RX_CONFIG;
    sendOutputChConfig = false;
    //unset other flags
    status1 &= ~FLAG_FAILSAFE_DATA;
    status1 &= ~FLAG_GET_TELEMETRY;    
  }
 
 
  uint8_t tmpBuff[24];
  memset(tmpBuff, 0, sizeof(tmpBuff));
  
  tmpBuff[0] = status0;
  tmpBuff[1] = status1;
  
  // Audio
  if((Sys.soundMode == SOUND_OFF)
      || (Sys.soundMode == SOUND_ALARMS && audioToPlay >= AUDIO_SWITCHMOVED)
      || (Sys.soundMode == SOUND_NOKEY && audioToPlay == AUDIO_KEYTONE))
    audioToPlay = AUDIO_NONE; 
    
  tmpBuff[2] = audioToPlay; 
  audioToPlay = AUDIO_NONE; 
  
  //
  if(status1 & FLAG_WRITE_RX_CONFIG) //receiver config data
  {
    for(uint8_t i = 0; i < 9; i++) //###
      tmpBuff[3 + i] = outputChConfig[i];
  }
  else if(status1 & FLAG_FAILSAFE_DATA) //failsafe data
  {
    for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
    {
      if(Model.failsafe[i] == -101) //failsafe not specified, send 1023
      {
        tmpBuff[3 + i*2] = 0x03;
        tmpBuff[4 + i*2] = 0xFF;
      }
      else //failsafe specified
      {
        int fsf = 5 * Model.failsafe[i];
        fsf = constrain(fsf, 5 * Model.endpointL[i], 5 * Model.endpointR[i]);
        uint16_t val = (fsf + 500) & 0xFFFF;
        tmpBuff[3 + i*2] = (val >> 8) & 0xFF;
        tmpBuff[4 + i*2] = val & 0xFF;
      }
    }
  }
  else //real time RC data
  {
    for(uint8_t i = 0; i < NUM_PRP_CHANNLES; i++)
    {
      uint16_t val = (channelOut[i] + 500) & 0xFFFF; 
      tmpBuff[3 + i*2] = (val >> 8) & 0xFF;
      tmpBuff[4 + i*2] = val & 0xFF;
    }
  }
  
  //add a crc
  tmpBuff[23] = crc8Maxim(tmpBuff, 23);
  

  //Send to slave mcu
  Serial.write(tmpBuff, sizeof(tmpBuff));
}

//==================================================================================================

void getSerialData()
{
  /* 
  SLAVE TO MASTER MCU SERIAL COMMUNICATION
  
  Byte0     Bit7     --> Got receiver channel configuration
            Bit 6    --> Request poweroff
            Bits 5,4 --> Bind status
            Bit 3    --> SwF
            Bit 2    --> SwE 
            Bits 1,0 --> 3pos switch (SwC) state
  
  Byte1     Receiver config status code
  Byte2     Transmitter packet rate
  Byte3     Packet rate at receiver side
  Byte4-5   Voltage telemetry
  Byte6-14  Receiver channel config Ch1 to Ch9
  Byte15    CRC8
  */
  
  const uint8_t msgLength = 16;
  if (Serial.available() < msgLength)
  {
    return;
  }
  
  uint8_t tmpBuff[msgLength]; 
  memset(tmpBuff, 0, msgLength);

  uint8_t cntr = 0;
  while (Serial.available() > 0)
  {
    if (cntr < msgLength) 
    {
      tmpBuff[cntr] = Serial.read();
      cntr++;
    }
    else //Discard any extra data
      Serial.read();
  }
  
  //Check if valid and extract
  if(tmpBuff[msgLength - 1] == crc8Maxim(tmpBuff, msgLength - 1))
  {
    bindStatusCode = (tmpBuff[0] >> 4) & 0x03;
    
    swCState = tmpBuff[0] & 0x03;
    swEEngaged = (tmpBuff[0] >> 2) & 0x01;
    swFEngaged = (tmpBuff[0] >> 3) & 0x01;
    
    receiverConfigStatusCode = tmpBuff[1];
    
    transmitterPacketRate = tmpBuff[2];
    receiverPacketRate = tmpBuff[3];
    
    //-- telemetry voltage --
    telem_volts = joinBytes(tmpBuff[4], tmpBuff[5]);

    //-- power off request --
    if((tmpBuff[0] >> 6) & 0x01)
    {
      //save all data to eeprom
      eeSaveSysConfig();
      eeSaveModelData(Sys.activeModel);
      
      //play animation
      uint32_t qq = millis() + 1000;
      while(millis() < qq)
        showAnimation();
      
      //set power off flag and send to slave mcu, and enter infinite loop
      isRequestingPowerOff = true;
      sendSerialData();
      while(1)
      {
      }
    }
    
    //-- receiver channel configuration
    if((tmpBuff[0] >> 7) & 0x01)
    {
      gotOutputChConfig = true;
      for(uint8_t i = 0; i < 9; i++)
      {
        outputChConfig[i] = tmpBuff[6 + i] & 0x0F;
        maxOutputChConfig[i] = tmpBuff[6 + i] >> 4;
      }
    }
  }
}

//==================================================================================================

uint16_t joinBytes(uint8_t _highByte, uint8_t _lowByte)
{
  uint16_t rslt;
  rslt = (uint16_t) _highByte;
  rslt <<= 8;
  rslt |= (uint16_t) _lowByte;
  return rslt;
}

//==================================================================================================

void checkBattery()
{
  //Low pass filtered using exponential smoothing
  //As the implementation here uses integer math and the technique is recursive, 
  //there is loss of precision but this doesn't matter much here.
  const int smoothFactor = 5; //>0,<100
  long sample = ((long)analogRead(PIN_BATTVOLTS) * battVfactor) / 100;
  long battV = ((sample * smoothFactor) + ((100 - smoothFactor) * (long)battVoltsNow)) / 100;
  battVoltsNow = int(battV);
  
  if (battVoltsNow <= battVoltsMin)
    battState = BATTLOW;
  else if (battVoltsNow > (battVoltsMin + 100)) //100mV hysteris
    battState = BATTHEALTY; 
}
