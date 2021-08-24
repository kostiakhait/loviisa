#include "lvs_fs_block.h"
#include "lvs_error.h"
#include "lvs_drv_flash.h"
#include "lvs_mem.h"

static const unsigned char __signature1 = 0xA5;
static const unsigned char __signature2 = 0xF0;
static unsigned char validator[2] = {__signature1, __signature2};
static unsigned char invalidator[2] = {0, 0};

LVS_FS_DEFINE_BANK(fs_mem, 0xF0000000, 4096 * 1024, 4096);

static LVS_ERROR_T __lvs_fs_reread_bank_mem(LVS_BANK_T* bank)
{
  int size = (bank->bank_size / bank->page_size - 2) * sizeof(unsigned short);
  return lvs_FlashRead(bank->bank_address + bank->system_page * bank->page_size + 2, bank->ram_block, size);
};

static LVS_ERROR_T __lvs_fs_init(LVS_BANK_T* bank)
{
  unsigned char signatures[2];
  LVS_ERROR_T result;
  result = lvs_FlashRead(bank->bank_address, signatures, 2);
  if (result != LVS_OK)
    return result;
  if ((signatures[0] == __signature1) && (signatures[1] == __signature2))
  {
    bank->system_page = 0;
    return __lvs_fs_reread_bank_mem(bank);
  };
  result = lvs_FlashRead(bank->bank_address + bank->page_size, signatures, 2);
  if (result != LVS_OK)
    return result;
  if ((signatures[0] == __signature1) && (signatures[1] == __signature2))
  {
    bank->system_page = 1;
    return __lvs_fs_reread_bank_mem(bank);
  };
  result = lvs_FlashErase(bank->bank_address);
  if (result != LVS_OK)
    return result;
  signatures[0] = __signature1;
  signatures[1] = __signature2;
  bank->system_page = 0;
  lvs_memset(bank->ram_block, 0xFF, bank->page_size);
  return lvs_FlashWrite(bank->bank_address, signatures, 2);
};

static LVS_ERROR_T __lvs_fs_write_bank_mem(LVS_BANK_T* bank)
{
  unsigned char new_page = (bank->system_page + 1) % 2;
  LVS_ERROR_T result;
  int size = (bank->bank_size / bank->page_size - 2) * sizeof(unsigned short);
  result = lvs_FlashErase(bank->bank_address + new_page * bank->page_size);
  if (result != LVS_OK)
    return result;
  result = lvs_FlashWrite(bank->bank_address + new_page * bank->page_size + 2, bank->ram_block, bank->page_size - 2);
  if (result != LVS_OK)
    return result;
  result = lvs_FlashWrite(bank->bank_address + new_page * bank->page_size, validator, 2);
  if (result != LVS_OK)
    return result;
  result = lvs_FlashWrite(bank->bank_address + bank->system_page * bank->page_size, invalidator, 2);
  if (result == LVS_OK)
    bank->system_page = new_page;
  return result;
};

static LVS_ERROR_T __lvs_fs_format_bank(LVS_BANK_T* bank)
{
  LVS_ERROR_T result = lvs_FlashErase(bank->bank_address);
  if (result != LVS_OK)
    return result;
  result = lvs_FlashWrite(bank->bank_address, validator, 2);
  if (result != LVS_OK)
    return result;
  bank->system_page = 0;
  lvs_memset(bank->ram_block, 0xFF, bank->page_size);
  return LVS_OK;
};

static LVS_ERROR_T __lvs_fs_allocate_pages(LVS_BANK_T* bank, unsigned short pages, unsigned short* first_page, int write)
{
  unsigned short* mem = (unsigned short*) bank->ram_block;
  unsigned short prev_page;
  LVS_ERROR_T result;
  int found = 0;
  for (int page = 0; page < pages; page++)
  {
    found = 0;
    for (int i = 0; i < bank->bank_size / bank->page_size - 2; i++)
    {
      if (mem[i] == 0xFFFF)
      {
        mem[i] = 0x0000;
        if (page != 0)
          mem[prev_page] = (unsigned short) i;
        else
          *first_page = (unsigned short) i;
        prev_page = (unsigned short) i;
        found = 1;
        break;
      }
    };
    if (!found)
    {
      result = __lvs_fs_reread_bank_mem(bank);
      if (result != LVS_OK)
        return result;
      return LVS_OVERFLOW;
    };
  };
  if (write)
    return __lvs_fs_write_bank_mem(bank);
  else
    return LVS_OK;
};

static unsigned short __lvs_fs_get_next_page(LVS_BANK_T* bank, unsigned short current)
{
  unsigned short* mem = (unsigned short*) bank->ram_block;
  return mem[current];
};

static LVS_ERROR_T __lvs_fs_append(LVS_BANK_T* bank, unsigned short start, unsigned short pages)
{
  unsigned short page_ptr = start;
  unsigned short next;
  unsigned short* mem = (unsigned short*) bank->ram_block;
  do {
    next = __lvs_fs_get_next_page(bank, page_ptr);
    if (next && (next != 0xFFFF))
      page_ptr = next;
  } while (next && (next != 0xFFFF));
  LVS_ERROR_T result = __lvs_fs_allocate_pages(bank, pages, &next, 0);
  if (result != LVS_OK)
  {
    __lvs_fs_reread_bank_mem(bank);
    return result;
  };
  mem[page_ptr] = next;
  return __lvs_fs_write_bank_mem(bank);
}; 

static LVS_ERROR_T __lvs_fs_truncate(LVS_BANK_T* bank, unsigned short start, unsigned short pages)
{
  unsigned short page_ptr = start;
  unsigned short next;
  unsigned short* mem = (unsigned short*) bank->ram_block;
  do {
    pages--;
    if (pages)
    {
      next = __lvs_fs_get_next_page(bank, page_ptr);
      if (next && (next != 0xFFFF))
        page_ptr = next;
    };
  } while (next && (next != 0xFFFF) && pages);
  next = mem[page_ptr];
  mem[page_ptr] = 0x0000;
  while (next)
  {
    page_ptr = next;
    next = __lvs_fs_get_next_page(bank, next);
    mem[page_ptr] = 0xFFFF;
  };
  return __lvs_fs_write_bank_mem(bank);
};

static int __lvs_fs_get_length(LVS_BANK_T* bank, unsigned short start)
{
  unsigned short next = start;
  int len = 0;
  do
  {
    next = __lvs_fs_get_next_page(bank, next);
    len += 1;
  } while (next);
  return len;
};

void __lvs_fs_test()
{
  LVS_ERROR_T result;
  unsigned short first_page;
  int len1, len2;
  result = __lvs_fs_init(&LVS_FS_BANK(fs_mem));
  result = __lvs_fs_format_bank(&LVS_FS_BANK(fs_mem));
  result = __lvs_fs_allocate_pages(&LVS_FS_BANK(fs_mem), 10, &first_page, 1);
  result = __lvs_fs_append(&LVS_FS_BANK(fs_mem), first_page, 15);
  len1 = __lvs_fs_get_length(&LVS_FS_BANK(fs_mem), first_page);
  result = __lvs_fs_truncate(&LVS_FS_BANK(fs_mem), first_page, 5);
  len2 = __lvs_fs_get_length(&LVS_FS_BANK(fs_mem), first_page);
};