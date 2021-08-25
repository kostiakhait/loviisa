#include "lvs_fs.h"
#include "lvs_drv_flash.h"
#include "lvs_mem.h"

LVS_FS_USE_BANK(fs_mem);

LVS_ERROR_T __lvs_fs_create_global_dir(LVS_BANK_T* bank);

typedef struct __LVS_FS_FILE_HEADER
{
  char name[LVS_FS_FILE_NAME_SIZE];
  unsigned short attributes;
  long size;
} LVS_FS_FILE_HEADER;

static void __lvs_fs_read_file_header(LVSFS_FD* fd, LVS_FS_FILE_HEADER* header)
{
  fd->error = lvsFS_Read(fd->bank, fd->file_desc, 0, (unsigned char*) header, sizeof(LVS_FS_FILE_HEADER));
};

static void __lvs_fs_write_file_header(LVSFS_FD* fd, LVS_FS_FILE_HEADER* header)
{
  fd->error = lvsFS_Rewrite(fd->bank, fd->file_desc, 0, (unsigned char*) header, sizeof(LVS_FS_FILE_HEADER));
};

int lvsFS_ReadFile(LVSFS_FD* fd, unsigned long offset, unsigned char* data, int size)
{
  LVS_FS_FILE_HEADER fh;
  __lvs_fs_read_file_header(fd, &fh);
  if (fd->error != LVS_OK)
    return 0;
  if (fh.size < size + offset)
    size = fh.size - offset;
  if (size < 0)
    return 0;
  unsigned long page = (offset + sizeof(LVS_FS_FILE_HEADER)) / fd->bank->page_size;
  offset = (offset + sizeof(LVS_FS_FILE_HEADER)) % fd->bank->page_size;
  unsigned long remain = size;
  if (offset)
  {
    int to_read = fd->bank->page_size - offset;
    if (remain < to_read)
      to_read = remain;
    unsigned short page_id = lvsFS_GetPage(fd->bank, fd->file_desc, page);
    fd->error = lvsFS_Read(fd->bank, page_id, offset, data, to_read);
    if (fd->error != LVS_OK)
      return size - remain;
    else
    {
      remain -= to_read;
      data += to_read;
    };
  };
  while (remain)
  {
    page += 1;
    int to_read = fd->bank->page_size;
    if (remain < to_read)
      to_read = remain;
    unsigned short page_id = lvsFS_GetPage(fd->bank, fd->file_desc, page);
    fd->error = lvsFS_Read(fd->bank, page_id, 0, data, to_read);
    if (fd->error != LVS_OK)
      return size - remain;
    else
    {
      remain -= to_read;
      data += to_read;
    };    
  };
  return size;
};

int lvsFS_WriteFile(LVSFS_FD* fd, unsigned long offset, unsigned char* data, int size)
{
  LVS_FS_FILE_HEADER fh;
  __lvs_fs_read_file_header(fd, &fh);
  if (fd->error != LVS_OK)
    return 0;
  if (fh.size < size + offset)
  {
    int pages_to_be = (size + offset - fh.size + fd->bank->page_size - 1) / fd->bank->page_size;
    int pages_now = lvsFS_GetLength(fd->bank, fd->file_desc);
    if (pages_to_be > pages_now)
    {
      fd->error = lvsFS_Append(fd->bank, fd->file_desc, pages_to_be - pages_now);
      if (fd->error != LVS_OK)
        return 0;
    };
    fh.size = size + offset;
    __lvs_fs_write_file_header(fd, &fh);
  };
  unsigned long page = (offset + sizeof(LVS_FS_FILE_HEADER)) / fd->bank->page_size;
  offset = (offset + sizeof(LVS_FS_FILE_HEADER)) % fd->bank->page_size;
  unsigned long remain = size;
  if (offset)
  {
    int to_read = fd->bank->page_size - offset;
    if (remain < to_read)
      to_read = remain;
    unsigned short page_id = lvsFS_GetPage(fd->bank, fd->file_desc, page);
    fd->error = lvsFS_Rewrite(fd->bank, page_id, offset, data, to_read);
    if (fd->error != LVS_OK)
      return size - remain;
    else
    {
      remain -= to_read;
      data += to_read;
    };
  };
  while (remain)
  {
    page += 1;
    int to_read = fd->bank->page_size;
    if (remain < to_read)
      to_read = remain;
    unsigned short page_id = lvsFS_GetPage(fd->bank, fd->file_desc, page);
    if (to_read == fd->bank->page_size)
      fd->error = lvsFS_Write(fd->bank, page_id, 0, data, to_read);
    else
      fd->error = lvsFS_Rewrite(fd->bank, page_id, 0, data, to_read);
    if (fd->error != LVS_OK)
      return size - remain;
    else
    {
      remain -= to_read;
      data += to_read;
    };    
  };
  return size;
};

int lvsFS_GetFileSize(LVSFS_FD* fd)
{
  LVS_FS_FILE_HEADER fh;
  __lvs_fs_read_file_header(fd, &fh);
  if (fd->error != LVS_OK)
    return 0;
  return fh.size;
};

LVS_ERROR_T lvsFS_Format(LVS_BANK_T* bank)
{
  LVS_ERROR_T result = lvsFS_FormatBank(bank);
  if (result != LVS_OK)
    return result;
  result = __lvs_fs_create_global_dir(bank);
  return result;
};

LVSFS_FD lvsFS_OpenGlobalDir(LVS_BANK_T* bank)
{
  LVSFS_FD result;
  result.bank = bank;
  result.error = LVS_OK;
  result.file_desc = 0;
  return result;
};

static LVSFS_FD __lvs_fs_find(LVSFS_FD* dir, char* name)
{
  int size = lvsFS_GetFileSize(dir);
  unsigned short item;
  LVSFS_FD result = {LVS_OK, dir->bank, 0xFFFF};
  for (int i = 0; i < size / 2; i++)
  {
    int read = lvsFS_ReadFile(dir, i * 2, (unsigned char*) &item, sizeof(unsigned short));
    if ((dir->error != LVS_OK) || (read != sizeof(unsigned short)))
    {
      result.error = LVS_IO_ERROR;
      return result;
    };
    LVS_FS_FILE_HEADER header;
    result.error = lvsFS_Read(dir->bank, item, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));
    if (result.error != LVS_OK)
      return result;
    int found = 1;
    for (int idx = 0; name[idx]; idx++)
    {
      if (name[idx] != header.name[idx])
      {
        found = 0;
        break;
      }
    };
    if (found)
    {
      result.file_desc = item;
      return result;
    };
  };
  result.error = LVS_NOT_FOUND;
  return result;
};

static LVSFS_FD __lvs_fs_create_file(LVS_BANK_T* bank, char* name)
{
    LVSFS_FD result;
    result.bank = bank;
    result.error = lvsFS_Allocate(bank, 1, &result.file_desc);
    if (result.error != LVS_OK)
      return result;
    LVS_FS_FILE_HEADER header;
    lvs_memset(&header.name[0], 0, LVS_FS_FILE_NAME_SIZE);
    for (int i = 0; name[i] != 0; i++)
      header.name[i] = name[i];
    header.attributes = 0;
    header.size = 0;
    result.error = lvsFS_Write(bank, result.file_desc, 0, (unsigned char*) &header, sizeof(header));
    if (result.error != LVS_OK)
    {
      lvsFS_Erase(bank, result.file_desc);
      result.file_desc = 0xFFFF;
    };
    return result;
};

LVS_ERROR_T __lvs_fs_create_global_dir(LVS_BANK_T* bank)
{
  LVSFS_FD result = __lvs_fs_create_file(bank, "");
  if (result.error != LVS_OK)
    return result.error;
  if (result.file_desc != 0)
    return LVS_WRONG_ARGUMENT;
  return LVS_OK;
};

LVSFS_FD lvsFS_CreateFile(LVSFS_FD* dir, char* name)
{
  LVSFS_FD result = __lvs_fs_find(dir, name);
  if (result.error != LVS_NOT_FOUND)
  {
    result.error = LVS_ALREADY_EXISTS;
    return result;
  };
  result = __lvs_fs_create_file(dir->bank, name);
  if (result.error != LVS_OK)
    return result;
  int size = lvsFS_GetFileSize(dir);
  size = lvsFS_WriteFile(dir, size, (unsigned char*)&result.file_desc, sizeof(unsigned short));
  if (size != sizeof(unsigned short))
    result.error = LVS_IO_ERROR;
  return result;  
};

LVSFS_FD lvsFS_OpenFile(LVSFS_FD* dir, char* name)
{
  LVSFS_FD result = __lvs_fs_find(dir, name);
  if (result.error != LVS_OK)
    return result;
  return result;
};

void __lvs_fs_test()
{
  LVS_ERROR_T result;
  unsigned short first_page;
  int len1, len2;
  char read[32];
  LVSFS_FD fd;
  result = lvsFS_InitBank(&LVS_FS_BANK(fs_mem));
  result = lvsFS_Format(&LVS_FS_BANK(fs_mem));
  LVSFS_FD dir = lvsFS_OpenGlobalDir(&LVS_FS_BANK(fs_mem));
  fd = lvsFS_CreateFile(&dir, "Test");
  fd = lvsFS_OpenFile(&dir, "Test");
  len1 = lvsFS_WriteFile(&fd, 10000, "Hellow, World!", 15);
  len2 = lvsFS_ReadFile(&fd, 10000, read, 15);
  len1 = lvsFS_GetFileSize(&fd);
};