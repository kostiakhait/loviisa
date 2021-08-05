/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) LigerWays, 2013-2021       */
/********************************************************************************/
/* Generic memory operations, may be replaced with CRT library functions        */
/********************************************************************************/
#ifndef __LVS_MEM_H__
#define __LVS_MEM_H__

// Fill memory with the given template value
void lvs_memset(void* ptr, unsigned char value, int size);
// Compare memory blocks
int lvs_memcmp(void* p1, void* p2, int size);
// Copy memory block
void lvs_memcpy(void* p1, void* p2, int size);

#endif