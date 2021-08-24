#ifndef __LVS_FS_BLOCK_H__
#define __LVS_FS_BLOCK_H__

typedef struct __LVS_FS_HEADER
{
  unsigned char signature1;                       // First byte of the signature
  unsigned char signature2;                       // Second byte of the signature
} LVS_FS_BANK_HEADER_T;

typedef struct __LVS_BANK
{
  unsigned long bank_address;                     // Start address of the bank
  unsigned long bank_size;                        // Bank size in bytes
  unsigned long page_size;                        // Single page size
  unsigned char* ram_block;                       // System page representation
  unsigned char system_page;                      // Number of the system page
} LVS_BANK_T;

#define LVS_FS_DEFINE_BANK(name, address, size, page) \
  unsigned char __lvs_fs_bank_mem_##name[page];       \
  LVS_BANK_T __lvs_fs_bank_##name = {address, size, page, &__lvs_fs_bank_mem_##name[0], 0};

#define LVS_FS_USE_BANK(name) extern LVS_BANK_T __lvs_fs_bank_##name;

#define LVS_FS_BANK(name) (__lvs_fs_bank_##name)

#endif