#include "lvs_drvcore.h"
#include "lvs_event.h"

LVS_PIPE_T* __lvs_pipes;

LVS_ERROR_T lvs_WriteToPipe(LVS_PIPE_T* pipe, unsigned char* data, int size)
{
  LVS_ERROR_T result;
  if (pipe->source_lock)
    return LVS_BUSY;
  pipe->source_lock = 1;
  if (lvs_RingbufGetFreeSpace(pipe->source) >= size)
    result = lvs_RingbufWrite(pipe->source, data, size);
  else
    result = LVS_OVERFLOW;
  pipe->source_lock = 0;
  return result;
};

LVS_ERROR_T lvs_ReadFromPipe(LVS_PIPE_T* pipe, unsigned char* data, int* size)
{
  LVS_ERROR_T result;
  if (pipe->target_lock)
    return LVS_RESOURCE_LOCKED;
  pipe->target_lock = 1;
  int _size = lvs_RingbufGetSize(pipe->target);
  if (*size > _size)
    *size = _size;
  if (*size)
    result = lvs_RingbufRead(pipe->target, data, *size);
  else
    result = LVS_UNDERFLOW;
  pipe->target_lock = 0;
  return result;
};

static LVS_ERROR_T __lvs_SyncPipe(LVS_PIPE_T* pipe)
{
  LVS_ERROR_T result = LVS_OK;
  if (pipe->source_lock || pipe->target_lock)
    return LVS_RESOURCE_LOCKED;
  pipe->source_lock = pipe->target_lock = 1;
  int size = lvs_RingbufGetSize(pipe->source);
  unsigned char c;
  for (int i = 0; i < size; i++)
  {
    result = lvs_RingbufReadByte(pipe->source, &c);
    if (LVS_OK == result)
    {
      result = lvs_RingbufWriteByte(pipe->target, c);
      if (LVS_OK != result)
        break;
    }
  };
  if ((size > 0) && pipe->on_data)
  {
    LVS_EVENT_T event;
    event.id = pipe->on_data;
    event.payload = pipe;
    lvs_SendEvent(event);
  }
  pipe->source_lock = pipe->target_lock = 0;
  return result;
};

void lvs_SyncDrvBufs()
{
  LVS_PIPE_T* channel = __lvs_pipes;
  while (channel)
  {
    __lvs_SyncPipe(channel);
    channel = channel->next;
  };
};