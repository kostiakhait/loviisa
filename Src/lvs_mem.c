/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) LigerWays, 2013-2021       */
/********************************************************************************/
/* Generic memory operations, may be replaced with CRT library functions        */
/********************************************************************************/
#include "lvs_mem.h"

void lvs_memset(void* ptr, unsigned char value, int size)
{
  unsigned char* byteptr = (unsigned char*) ptr;
  for (int i = 0; i < size; i++)
    byteptr[i] = value;
};

int lvs_memcmp(void* p1, void* p2, int size)
{
  unsigned char* bp1 = (unsigned char*) p1;
  unsigned char* bp2 = (unsigned char*) p2;
  for (int i = 0; i < size; i++)
  {
    if (bp1[i] < bp2[i])
      return -1;
    if (bp1[i] > bp2[i])
      return +1;
  }
  return 0;
};

void lvs_memcpy(void* p1, void* p2, int size)
{
  unsigned char* bp1 = (unsigned char*) p1;
  unsigned char* bp2 = (unsigned char*) p2;
  for (int i = 0; i < size; i++)
    bp1[i] = bp2[i];
};