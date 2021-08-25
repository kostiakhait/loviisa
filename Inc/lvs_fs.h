#ifndef __LVS_FS_H__
#define __LVS_FS_H__

#include "lvs_fs_block.h"

#ifndef LVS_FS_FILE_NAME_SIZE
#define LVS_FS_FILE_NAME_SIZE 24
#endif

typedef struct __LVS_FS_FD
{
  LVS_ERROR_T error;
  LVS_BANK_T* bank;
  unsigned short file_desc;
} LVSFS_FD;

LVSFS_FD lvsFS_CreateFile(LVSFS_FD* dir, char* name);
int lvsFS_ReadFile(LVSFS_FD* fd, unsigned long offset, unsigned char* data, int size);
int lvsFS_WriteFile(LVSFS_FD* fd, unsigned long offset, unsigned char* data, int size);
int lvsFS_GetFileSize(LVSFS_FD* fd);
LVS_ERROR_T lvsFS_Format(LVS_BANK_T* bank);
LVSFS_FD lvsFS_OpenGlobalDir(LVS_BANK_T* bank);



#endif