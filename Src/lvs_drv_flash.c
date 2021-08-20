#include "lvs_drv_flash.h"

LVS_DRV_FLASH_DESC* __lvs_drv_flash_ctrl = 0L;

LVS_ERROR_T lvs_FlashErase(unsigned long address)
{
  LVS_DRV_FLASH_DESC* current = __lvs_drv_flash_ctrl;
  while (current)
  {
    LVS_DRV_FLASH_CONFIG* desc = current->get_config();
    if ((address >= desc->base_address) && (address < desc->base_address + desc->size))
      return current->erase(address - desc->base_address);
  };
  return LVS_DRV_NOT_FOUND;
};

LVS_ERROR_T lvs_FlashWrite(unsigned long address, unsigned char* data, int size)
{
  LVS_DRV_FLASH_DESC* current = __lvs_drv_flash_ctrl;
  while (current)
  {
    LVS_DRV_FLASH_CONFIG* desc = current->get_config();
    if ((address >= desc->base_address) && (address < desc->base_address + desc->size))
      if (address + size >= desc->base_address + desc->size)
        return LVS_DRV_OUT_OF_RANGE;
      else
        return current->write(address - desc->base_address, data, size);
  };
  return LVS_DRV_NOT_FOUND;
};

LVS_ERROR_T lvs_FlashRead(unsigned long address, unsigned char* data, int size)
{
  LVS_DRV_FLASH_DESC* current = __lvs_drv_flash_ctrl;
  while (current)
  {
    LVS_DRV_FLASH_CONFIG* desc = current->get_config();
    if ((address >= desc->base_address) && (address < desc->base_address + desc->size))
      if (address + size >= desc->base_address + desc->size)
        return LVS_DRV_OUT_OF_RANGE;
      else
        return current->read(address - desc->base_address, data, size);
  };
  return LVS_DRV_NOT_FOUND;
};
