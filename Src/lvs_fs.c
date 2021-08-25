#include "lvs_fs.h"
#include "lvs_drv_flash.h"
#include "lvs_mem.h"
#include "lvs_config.h"

LVS_ERROR_T __lvs_fs_create_global_dir(LVS_BANK_T* bank);

#define ASSERT_FD(fd) {if ((fd).file_desc > (fd).bank->bank_size / (fd).bank->page_size - 2) LVS_FATAL_ERROR();}

typedef struct __LVS_FS_FILE_HEADER
{
  char name[LVS_FS_FILE_NAME_SIZE];
  unsigned short attributes;
  long size;
} LVS_FS_FILE_HEADER;

static void __lvs_fs_read_file_header(LVSFS_FD* fd, LVS_FS_FILE_HEADER* header)
{
  ASSERT_FD(*fd);
  fd->error = lvsFS_Read(fd->bank, fd->file_desc, 0, (unsigned char*) header, sizeof(LVS_FS_FILE_HEADER));
};

static void __lvs_fs_write_file_header(LVSFS_FD* fd, LVS_FS_FILE_HEADER* header)
{
  ASSERT_FD(*fd);
  fd->error = lvsFS_Rewrite(fd->bank, fd->file_desc, 0, (unsigned char*) header, sizeof(LVS_FS_FILE_HEADER));
};

int lvsFS_ReadFile(LVSFS_FD* fd, unsigned long offset, unsigned char* data, int size)
{
  ASSERT_FD(*fd);
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
  ASSERT_FD(*fd);
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
  ASSERT_FD(*fd);
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
  if (result != LVS_OK)
    return result;
  LVSFS_FD dir = lvsFS_OpenGlobalDir(bank);
  ASSERT_FD(dir);
  lvsFS_SetAttr(&dir, LVS_FS_ATTR_DIR);
  return dir.error;
};

LVSFS_FD lvsFS_OpenGlobalDir(LVS_BANK_T* bank)
{
  LVSFS_FD result;
  result.bank = bank;
  result.error = LVS_OK;
  result.file_desc = 0;
  return result;
};

void lvsFS_SetAttr(LVSFS_FD* fd, unsigned short attrs)
{
  ASSERT_FD(*fd);
  LVS_FS_FILE_HEADER header;
  fd->error = lvsFS_Read(fd->bank, fd->file_desc, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));
  if (fd->error != LVS_OK)
    return;
  header.attributes = attrs;
  fd->error = lvsFS_Rewrite(fd->bank, fd->file_desc, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));
};

unsigned short lvsFS_GetAttr(LVSFS_FD* fd)
{
  ASSERT_FD(*fd);
  LVS_FS_FILE_HEADER header;
  fd->error = lvsFS_Read(fd->bank, fd->file_desc, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));
  if (fd->error != LVS_OK)
    return 0xFFFF;
  return header.attributes;
};

static LVSFS_FD __lvs_fs_find(LVSFS_FD* dir, char* name)
{
  ASSERT_FD(*dir);
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
    for (int idx = 0; name[idx] != 0; idx++)
    {
      if ((name[idx] != header.name[idx]) || (idx == LVS_FS_FILE_NAME_SIZE))
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
  ASSERT_FD(*dir);
  LVSFS_FD result = __lvs_fs_find(dir, name);
  return result;
};

LVS_ERROR_T lvsFS_CloseFile(LVSFS_FD* dir)
{
  ASSERT_FD(*dir);
  return LVS_OK;
};

void __lvs_fs_remove_fd_from_dir(LVSFS_FD* dir, unsigned short fd)
{
  ASSERT_FD(*dir);
  int size = lvsFS_GetFileSize(dir);
  if (dir->error != LVS_OK)
    return;
  unsigned short item;
  int write_ptr = 0;
  for (int i = 0; i < size / 2; i++)
  {      
    int read = lvsFS_ReadFile(dir, i * 2, (unsigned char*)&item, sizeof(unsigned short));
    if (read != sizeof(unsigned short))
      dir->error = LVS_IO_ERROR;
    if (dir->error != LVS_OK)
      return;
    if (i != write_ptr)
    {
      lvsFS_WriteFile(dir, write_ptr * 2, (unsigned char*)&item, sizeof(unsigned short));
      if (dir->error != LVS_OK)
        return;
    } 
    if (item != fd)
      write_ptr += 1;
  };
  lvsFS_TruncateFile(dir, write_ptr * 2);
};

void lvsFS_TruncateFile(LVSFS_FD* fd, int size)
{
  ASSERT_FD(*fd);
  int current_size = lvsFS_GetFileSize(fd);
  if (current_size <= size)
    return;
  unsigned short size_in_pages = (size + sizeof(LVS_FS_FILE_HEADER) + fd->bank->page_size - 1) / fd->bank->page_size;
  fd->error = lvsFS_Truncate(fd->bank, fd->file_desc, size_in_pages);
  if (fd->error != LVS_OK)
    return;
  LVS_FS_FILE_HEADER header;
  fd->error = lvsFS_Read(fd->bank, fd->file_desc, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));
  if (fd->error != LVS_OK)
    return;
  header.size = size;
  fd->error = lvsFS_Rewrite(fd->bank, fd->file_desc, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));  
};

LVS_ERROR_T lvsFS_RenameFile(LVSFS_FD* dir, char* old_name, char* new_name)
{
  ASSERT_FD(*dir);
  LVSFS_FD fd = lvsFS_OpenFile(dir, old_name);
  if (fd.error != LVS_OK)
    return fd.error;
  LVSFS_FD _fd = lvsFS_OpenFile(dir, new_name);
  if (_fd.error != LVS_NOT_FOUND)
    return LVS_ALREADY_EXISTS;
  LVS_FS_FILE_HEADER header;
  fd.error = lvsFS_Read(fd.bank, fd.file_desc, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));
  if (fd.error != LVS_OK)
    return fd.error;
  lvs_memcpy(&header.name[0], new_name, LVS_FS_FILE_NAME_SIZE);
  return lvsFS_Rewrite(fd.bank, fd.file_desc, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));  
};

char* lvsFS_GetFileName(LVSFS_FD* fd)
{
  ASSERT_FD(*fd);
  static LVS_FS_FILE_HEADER header;
  fd->error = lvsFS_Read(fd->bank, fd->file_desc, 0, (unsigned char*)&header, sizeof(LVS_FS_FILE_HEADER));
  if (fd->error != LVS_OK)
    return 0L;
  return &header.name[0];
};

LVS_ERROR_T lvsFS_RemoveFile(LVSFS_FD* dir, char* name)
{
  ASSERT_FD(*dir);
  LVSFS_FD fd = lvsFS_OpenFile(dir, name);
  if (dir->error != LVS_OK)
    return LVS_NOT_FOUND;
  __lvs_fs_remove_fd_from_dir(dir, fd.file_desc);
  if (dir->error != LVS_OK)
    return LVS_IO_ERROR;
  return lvsFS_Erase(dir->bank, fd.file_desc);
};

LVSFS_FD lvsFS_MakeDirectory(LVSFS_FD* dir, char* name)
{
  ASSERT_FD(*dir);
  LVSFS_FD fd = lvsFS_CreateFile(dir, name);
  if (fd.error != LVS_OK)
    return fd;
  ASSERT_FD(fd);
  lvsFS_SetAttr(&fd, LVS_FS_ATTR_DIR);
  return fd;
};

int lvsFS_GetNumberOfItems(LVSFS_FD* dir)
{
  ASSERT_FD(*dir);
  unsigned short attrs = lvsFS_GetAttr(dir);
  if (dir->error != LVS_OK)
    return -1;
  if ((attrs & LVS_FS_ATTR_DIR) == 0)
    return -1;
  int size = lvsFS_GetFileSize(dir);
  if (dir->error != LVS_OK)
    return -1;
  return size / 2;
};

LVSFS_FD lvsFS_GetItem(LVSFS_FD* dir, int index)
{
  ASSERT_FD(*dir);
  int read;
  unsigned short item;
  LVSFS_FD result = {LVS_OK, dir->bank, 0xFFFF};
  read = lvsFS_ReadFile(dir, index * 2, (unsigned char*)&item, sizeof(unsigned short));
  if (read != sizeof(unsigned short))
  {
    result.error = dir->error;
    return result;
  };
  result.file_desc = item;
  return result;
};

