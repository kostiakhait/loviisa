/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) LigerWays, 2013-2021       */
/********************************************************************************/
/* Ring buffer implementation. Used for events queue and can be used by the     */
/* application code                                                             */
/********************************************************************************/
#include "lvs_ringbuf.h"
#include "lvs_mem.h"

void lvs_RingbufInit(LVS_RINGBUF_T* rb, unsigned char* buf, unsigned int size)
{
  rb->data = buf;
  lvs_memset(buf, 0xFF, size);
  rb->readpos = 0;
  rb->writepos = 0;
  rb->size = 0;
  rb->total_size = size;
};

int lvs_RingbufGetFreeSpace(LVS_RINGBUF_T* rb)
{
  return rb->total_size - rb->size;
};

int lvs_RingbufGetSize(LVS_RINGBUF_T* rb)
{
  return rb->size;
};

int lvs_RingbufGetTotalSize(LVS_RINGBUF_T* rb)
{
  return rb->total_size;
};

LVS_ERROR_T lvs_RingbufWriteByte(LVS_RINGBUF_T* rb, unsigned char data)
{
  if (rb->total_size - rb->size >= 1)
  {
    rb->data[rb->writepos] = data;
    rb->writepos += 1;
    if (rb->writepos >= rb->total_size)
      rb->writepos = 0;
    rb->size += 1;
  }
  else
    return LVS_OVERFLOW;
  return LVS_OK;
};

LVS_ERROR_T lvs_RingbufWrite(LVS_RINGBUF_T* rb, unsigned char* data, int size)
{
  if (lvs_RingbufGetFreeSpace(rb) < size)
    return LVS_OVERFLOW;
  else
    for (int i = 0; i < size; i++)
      lvs_RingbufWriteByte(rb, data[i]);
  return LVS_OK;
};

LVS_ERROR_T lvs_RingbufReadByte(LVS_RINGBUF_T* rb, unsigned char* data)
{
  if (rb->size > 0)
  {
    *data = rb->data[rb->readpos];
    rb->readpos += 1;
    if (rb->readpos >= rb->total_size)
      rb->readpos = 0;
    rb->size -= 1;
  }
  else
    return LVS_UNDERFLOW;
  return LVS_OK;
};

LVS_ERROR_T lvs_RingbufRead(LVS_RINGBUF_T* rb, unsigned char* data, int size)
{
  if (rb->size < size)
    return LVS_UNDERFLOW;
  else
    for (int i = 0; i < size; i++)
      lvs_RingbufReadByte(rb, &data[i]);
  return LVS_OK;
};

LVS_ERROR_T lvs_RingbufPeek(LVS_RINGBUF_T* rb, unsigned char* data)
{
  if (rb->size > 0)
  {
    *data = rb->data[rb->readpos];
  }
  else
    return LVS_UNDERFLOW;
  return LVS_OK;
};
