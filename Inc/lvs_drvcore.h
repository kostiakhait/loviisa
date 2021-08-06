#ifndef __LVS_DRVCORE_H__
#define __LVS_DRVCORE_H__

#include "lvs_mem.h"
#include "lvs_error.h"
#include "lvs_ringbuf.h"

typedef struct __LVS_PIPE
{
  LVS_RINGBUF_T* source;
  LVS_RINGBUF_T* target;
  unsigned int source_lock : 1;
  unsigned int target_lock : 1;
  struct __LVS_PIPE* next;
} LVS_PIPE_T;

extern LVS_PIPE_T* __lvs_pipes;

#define LVS_DEFINE_PIPE(name, size)                               \
  LVS_DEFINE_RINGBUF(src##name, size)                             \
  LVS_DEFINE_RINGBUF(tgt##name, size)                             \
  LVS_PIPE_T __lvs_pipe_##name =                                  \
    {                                                             \
      &LVS_RINGBUF(src##name),                                    \
      &LVS_RINGBUF(tgt##name),                                    \
      0, 0, 0L                                                    \
    }

#define LVS_USE_PIPE(name) extern LVS_PIPE_T __lvs_pipe_##name;

#define LVS_CREATE_PIPE(name)                                     \
  {                                                               \
    LVS_RINGBUF_INIT(src##name);                                  \
    LVS_RINGBUF_INIT(tgt##name);                                  \
    __lvs_pipe_##name.next = __lvs_pipes;                         \
    __lvs_pipes = &__lvs_pipe_##name;                             \
  }

LVS_ERROR_T lvs_WriteToPipe(LVS_PIPE_T* pipe, unsigned char* data, int size);
LVS_ERROR_T lvs_ReadFromPipe(LVS_PIPE_T* pipe, unsigned char* data, int* size);

#define LVS_WRITE_PIPE(name, dataptr, size)                       \
  lvs_WriteToPipe(&__lvs_pipe_##name, dataptr, size)

#define LVS_READ_PIPE(name, dataptr, sizeptr)                     \
  lvs_ReadFromPipe(&__lvs_pipe_##name, dataptr, sizeptr)

#endif