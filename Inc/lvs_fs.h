#ifndef __LVS_FS_H__
#define __LVS_FS_H__

#include "lvs_fs_block.h"

#ifndef LVS_FS_FILE_NAME_SIZE
#define LVS_FS_FILE_NAME_SIZE 24
#endif

#define LVS_FS_ATTR_DIR 0x0001

typedef struct __LVS_FS_FD
{
  LVS_ERROR_T error;
  LVS_BANK_T* bank;
  unsigned short file_desc;
  int size;
  unsigned short attributes;
} LVSFS_FD;

// Initialize the file system
LVS_ERROR_T lvsFS_Init(LVS_BANK_T* bank);
// Create new file in the given directory
LVSFS_FD lvsFS_CreateFile(LVSFS_FD* dir, char* name);
// Read data from the file
int lvsFS_ReadFile(LVSFS_FD* fd, unsigned long offset, unsigned char* data, int size);
// Write data to the file
int lvsFS_WriteFile(LVSFS_FD* fd, unsigned long offset, unsigned char* data, int size);
// Get file size in bytes
int lvsFS_GetFileSize(LVSFS_FD* fd);
// Format entire memory ban
LVS_ERROR_T lvsFS_Format(LVS_BANK_T* bank);
// Open high level directory (get descriptor)
LVSFS_FD lvsFS_OpenGlobalDir(LVS_BANK_T* bank);
// Set file attributes
void lvsFS_SetAttr(LVSFS_FD* fd, unsigned short attrs);
// Get file attributes
unsigned short lvsFS_GetAttr(LVSFS_FD* fd);
// Open file (find in the directory)
LVSFS_FD lvsFS_OpenFile(LVSFS_FD* dir, char* name);
// Close file (actually do nothing)
LVS_ERROR_T lvsFS_CloseFile(LVSFS_FD* fd);
// Truncate file and potentially free memory
void lvsFS_TruncateFile(LVSFS_FD* fd, int size);
// Remove file from the directory and memory
LVS_ERROR_T lvsFS_RemoveFile(LVSFS_FD* dir, char* name);
// Change file name
LVS_ERROR_T lvsFS_RenameFile(LVSFS_FD* dir, char* old_name, char* new_name);
// Get name by descriptor
char* lvsFS_GetFileName(LVSFS_FD* fd);
// Create directory
LVSFS_FD lvsFS_MakeDirectory(LVSFS_FD* dir, char* name);
// Get number of items in the directory
int lvsFS_GetNumberOfItems(LVSFS_FD* dir);
// Get descriptor by the item number
LVSFS_FD lvsFS_GetItem(LVSFS_FD* dir, int index);

#endif