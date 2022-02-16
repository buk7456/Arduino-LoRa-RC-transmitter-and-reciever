#ifndef _EESTORE_H_
#define _EESTORE_H_

#define EE_FILE_SIGNATURE_ADDR 0  //1 byte
#define EE_INITFLAG_ADDR       1  //1 byte

#define EE_FILE_SIGNATURE  0xBD

void eraseEEPROM();

void eeStoreInit();

void eeReadSysConfig();
void eeSaveSysConfig();

void eeReadModelData(uint8_t _mdlNo); 
void eeSaveModelData(uint8_t _mdlNo);

void eeCopyModelName(char* _buff, uint8_t _mdlNo);

void eeCreateModel(uint8_t _mdlNo);
void eeDeleteModel(uint8_t _mdlNo);

#endif
