/* 
  Data arranged in eeprom as follows
  ---------------------------------------------------------------------------------------------
  FILE_SIGNATURE | EE_INITFLAG | System Data | 1st Model Data | 2nd Model Data | nth Model Data
  ---------------------------------------------------------------------------------------------
*/

#define EE_FILE_SIGNATURE_ADDR 0  //2 bytes
#define EE_INITFLAG_ADDR       2  //1 byte
const uint8_t eeSysDataStartAddress = 3; 
uint8_t eeModelDataStartAddress; //determined in setup()
uint8_t numOfModels;             //determined in setup()

void eeReadSysConfig();
void eeSaveSysConfig();
void eeReadModelData(uint8_t _mdlNO); 
void eeSaveModelData(uint8_t _mdlNO);
void eeCopyModelName(uint8_t _mdlNO, char* _buff);

//helpers
int getModelDataOffsetAddr(uint8_t _mdlNO);

//--------------------------------------------------------------------------------------------------

void eeReadSysConfig()
{
  EEPROM.get(eeSysDataStartAddress, Sys);
}

void eeSaveSysConfig()
{
  EEPROM.put(eeSysDataStartAddress, Sys);
}

//--------------------------------------------------------------------------------------------------

void eeReadModelData(uint8_t _mdlNO)
{
  int _mdlOffset_ = getModelDataOffsetAddr(_mdlNO);
  EEPROM.get(eeModelDataStartAddress + _mdlOffset_, Model);
}

void eeSaveModelData(uint8_t _mdlNO)
{
  int _mdlOffset_ = getModelDataOffsetAddr(_mdlNO);
  EEPROM.put(eeModelDataStartAddress + _mdlOffset_, Model);
}

//Copies modelName into specified buffer 
void eeCopyModelName(uint8_t _mdlNO, char* _buff)
{
  int _mdlOffset_ = getModelDataOffsetAddr(_mdlNO);
  
  for(uint8_t i = 0; i < (sizeof(Model.modelName)/sizeof(Model.modelName[0])); i++) 
  {
    *(_buff + i) = EEPROM.read(eeModelDataStartAddress + _mdlOffset_ + i);
  }
}

//--------------------------------------------------------------------------------------------------
int getModelDataOffsetAddr(uint8_t _mdlNO)
{
  return (sizeof(Model) * (_mdlNO - 1));
}
