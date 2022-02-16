/* 
  Data arranged in eeprom as follows
  ---------------------------------------------------------------------------------------------
  FILE_SIGNATURE | EE_INITFLAG | System Data | 1st Model Data | 2nd Model Data | nth Model Data
  ---------------------------------------------------------------------------------------------
*/

#include "Arduino.h"
#include <EEPROM.h>
#include "common.h"
#include "eestore.h"

const uint8_t eeSysDataStartAddress = 2; 
uint8_t eeModelDataStartAddress;

uint16_t getModelDataOffsetAddr(uint8_t _mdlNo);

//==================================================================================================

void eeStoreInit()
{
  eeModelDataStartAddress = eeSysDataStartAddress + sizeof(Sys);
  maxNumOfModels = (EEPROM.length() - eeModelDataStartAddress) / sizeof(Model);
}

//==================================================================================================

void eeReadSysConfig()
{
  EEPROM.get(eeSysDataStartAddress, Sys);
}

//==================================================================================================

void eeSaveSysConfig()
{
  EEPROM.put(eeSysDataStartAddress, Sys);
}

//==================================================================================================

void eeReadModelData(uint8_t _mdlNo)
{
  uint16_t _mdlOffset_ = getModelDataOffsetAddr(_mdlNo);
  EEPROM.get(eeModelDataStartAddress + _mdlOffset_, Model);
}

//==================================================================================================

void eeSaveModelData(uint8_t _mdlNo)
{
  uint16_t _mdlOffset_ = getModelDataOffsetAddr(_mdlNo);
  EEPROM.put(eeModelDataStartAddress + _mdlOffset_, Model);
}

//==================================================================================================

void eeCopyModelName(char* _buff, uint8_t _mdlNo)
{
  //Copies modelName into specified buffer 
  uint16_t _mdlOffset_ = getModelDataOffsetAddr(_mdlNo);
  for(uint8_t i = 0; i < (sizeof(Model.modelName)/sizeof(Model.modelName[0])); i++) 
  {
    *(_buff + i) = EEPROM.read(eeModelDataStartAddress + _mdlOffset_ + i);
  }
}

//==================================================================================================

void eeCreateModel(uint8_t _mdlNo)
{
  //save active model as we are going to use ram as sketchpad
  eeSaveModelData(Sys.activeModel);
  
  //set defaults
  setDefaultModelBasicParams();
  setDefaultModelMixerParams();
  setDefaultModelName();
  //save to eeprom
  eeSaveModelData(_mdlNo);
  
  //Read back active model
  eeReadModelData(Sys.activeModel);
}

//==================================================================================================

void eeDeleteModel(uint8_t _mdlNo)
{
  //simply remove name setting by all characters in name to 0xFF
  
  uint16_t _mdlOffset_ = getModelDataOffsetAddr(_mdlNo);
  
  uint8_t len = sizeof(Model.modelName)/sizeof(Model.modelName[0]);
  for(uint8_t i = 0; i < len - 1; i++) 
  {
    EEPROM.update(eeModelDataStartAddress + _mdlOffset_ + i, 0xFF);
  }
  EEPROM.update(eeModelDataStartAddress + _mdlOffset_ + len - 1, '\0'); //write termination char
}

//==================================================================================================

uint16_t getModelDataOffsetAddr(uint8_t _mdlNo)
{
  return (sizeof(Model) * (_mdlNo - 1));
}

//==================================================================================================

void eraseEEPROM()
{
  for(uint16_t i = 0; i < EEPROM.length(); i++)
    EEPROM.update(i, 0xFF);
}
