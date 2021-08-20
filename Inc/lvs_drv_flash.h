#ifndef __LVS_DRV_FLASH_H__
#define __LVS_DRV_FLASH_H__

#include "lvs_error.h"

typedef struct __LVS_DRV_FLASH_CONFIG
{
  unsigned long base_address;
  unsigned long size;
  unsigned int  page_size;
} LVS_DRV_FLASH_CONFIG;

typedef LVS_ERROR_T (*LVS_DRV_FLASH_ERASE_METHOD)(unsigned int address);
typedef LVS_ERROR_T (*LVS_DRV_FLASH_INIT_METHOD)();
typedef LVS_DRV_FLASH_CONFIG* (*LVS_DRV_FLASH_GET_CONFIG)(void);
typedef LVS_ERROR_T (*LVS_DRV_FLASH_WRITE)(unsigned int address, unsigned char* data, int size);
typedef LVS_ERROR_T (*LVS_DRV_FLASH_READ)(unsigned int address, unsigned char* data, int size);

typedef struct __LVS_DRV_FLASH_DESC
{
  LVS_DRV_FLASH_INIT_METHOD init;
  LVS_DRV_FLASH_GET_CONFIG get_config;
  LVS_DRV_FLASH_ERASE_METHOD erase;
  LVS_DRV_FLASH_READ read;
  LVS_DRV_FLASH_WRITE write;
  struct __LVS_DRV_FLASH_DESC* next;
} LVS_DRV_FLASH_DESC;

extern LVS_DRV_FLASH_DESC* __lvs_drv_flash_ctrl;

#define LVS_DRV_DEFINE_FLASH(name, mInit, mGetConfig, mErase, mRead, mWrite)  \
  LVS_DRV_FLASH_DESC __lvs_drv_flash_##name;                                  \
  LVS_DRV_FLASH_INIT_METHOD __lvs_drv_flash_init_##name = &mInit;             \
  LVS_DRV_FLASH_GET_CONFIG __lvs_drv_flash_get_config_##name = &mGetConfig;   \
  LVS_DRV_FLASH_ERASE_METHOD __lvs_drv_flash_erase_##name = &mErase;          \
  LVS_DRV_FLASH_READ __lvs_drv_flash_read_##name = &mRead;                    \
  LVS_DRV_FLASH_WRITE __lvs_drv_flash_write_##name = &mWrite;

#define LVS_DRV_USE_FLASH(name)  \
  extern LVS_DRV_FLASH_DESC __lvs_drv_flash_##name;                           \
  extern LVS_DRV_FLASH_INIT_METHOD __lvs_drv_flash_init_##name;               \
  extern LVS_DRV_FLASH_GET_CONFIG __lvs_drv_flash_get_config_##name;          \
  extern LVS_DRV_FLASH_ERASE_METHOD __lvs_drv_flash_erase_##name;             \
  extern LVS_DRV_FLASH_READ __lvs_drv_flash_read_##name;                      \
  extern LVS_DRV_FLASH_WRITE __lvs_drv_flash_write_##name;

#define LVS_DRV_INIT_FLASH(name)  \
  __lvs_drv_flash_##name.init = __lvs_drv_flash_init_##name;                  \
  __lvs_drv_flash_##name.get_config = __lvs_drv_flash_get_config_##name;      \
  __lvs_drv_flash_##name.erase = __lvs_drv_flash_erase_##name;                \
  __lvs_drv_flash_##name.read = __lvs_drv_flash_read_##name;                  \
  __lvs_drv_flash_##name.write =  __lvs_drv_flash_write_##name;               \
  __lvs_drv_flash_##name.init();                                                                    \
  __lvs_drv_flash_##name.next = __lvs_drv_flash_ctrl;                         \
  __lvs_drv_flash_ctrl = &__lvs_drv_flash_##name; 

LVS_ERROR_T lvs_FlashErase(unsigned long address);
LVS_ERROR_T lvs_FlashWrite(unsigned long address, unsigned char* data, int size);
LVS_ERROR_T lvs_FlashRead(unsigned long address, unsigned char* data, int size);


#endif