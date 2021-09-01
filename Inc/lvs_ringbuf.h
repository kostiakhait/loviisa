/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) LigerWays, 2013-2021       */
/********************************************************************************/
/* Ring buffer implementation. Used for events queue and can be used by the     */
/* application code                                                             */
/********************************************************************************/
#ifndef __LVS_RINGBUF_H__
#define __LVS_RINGBUF_H__

#include "lvs_error.h"

// Ring buffer handler structure. Shall be never accessed manually except special cases
typedef struct __LVS_RINGBUF
{
  unsigned char* data;                                  // Data buffer (memory pointer)
  unsigned int size;                                    // Currently used size
  unsigned int total_size;                              // Total size of the memory allocated for the buffer
  unsigned int writepos;                                // Position for writing
  unsigned int readpos;                                 // Position for reading
} LVS_RINGBUF_T;

// Define ring buffer structures. Shall be used once outside of all functions
#define LVS_USE_RINGBUF(name)                            \
  extern LVS_RINGBUF_T __lvs_ringbuf_##name;

// Define ring buffer structures. Shall be used once outside of all functions
#define LVS_DEFINE_RINGBUF(name, size)                  \
  LVS_RINGBUF_T __lvs_ringbuf_##name;                   \
  unsigned char __lvs_ringbuf_data_##name[size];        \
  const unsigned int __lvs_ringbuf_size_##name = size;
  
// Initialize the ring buffer. Shall be used once in the initialization function
#define LVS_RINGBUF_INIT(name)                          \
  lvs_RingbufInit(&__lvs_ringbuf_##name, &__lvs_ringbuf_data_##name[0], __lvs_ringbuf_size_##name);

// Get ring buffer handler structure by name
#define LVS_RINGBUF(name) (__lvs_ringbuf_##name)

// Initialize ring buffer (normally called with LVS_RINGBUF_INIT)
void lvs_RingbufInit(LVS_RINGBUF_T* rb, unsigned char* buf, unsigned int size);
// Get amount of the free space remaining in the buffer
int lvs_RingbufGetFreeSpace(LVS_RINGBUF_T* rb);
// Get current size of space used in the buffer
int lvs_RingbufGetSize(LVS_RINGBUF_T* rb);
// Get total size allocated for the buffer
int lvs_RingbufGetTotalSize(LVS_RINGBUF_T* rb);
// Write a single byte to the end of buffer
LVS_ERROR_T lvs_RingbufWriteByte(LVS_RINGBUF_T* rb, unsigned char data);
// Write a byte sequence to the end of the buffer
LVS_ERROR_T lvs_RingbufWrite(LVS_RINGBUF_T* rb, unsigned char* data, int size);
// Read single byte from the head of the buffer
LVS_ERROR_T lvs_RingbufReadByte(LVS_RINGBUF_T* rb, unsigned char* data);
// Read a sequence from the head of the buffer
LVS_ERROR_T lvs_RingbufRead(LVS_RINGBUF_T* rb, unsigned char* data, int size);
// Retrieve single byte from the buffer without changing read position
LVS_ERROR_T lvs_RingbufPeek(LVS_RINGBUF_T* rb, unsigned char* data);

#endif