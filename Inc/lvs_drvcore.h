#ifndef __LVS_DRVCORE_H__
#define __LVS_DRVCORE_H__

#include "lvs_mem.h"
#include "lvs_error.h"
#include "lvs_ringbuf.h"
#include "lvs_event.h"

typedef struct __LVS_PIPE
{
  LVS_RINGBUF_T* source;
  LVS_RINGBUF_T* target;
  unsigned int source_lock : 1;
  unsigned int target_lock : 1;
  LVS_EVENT_ID_T on_data;
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
      0, 0, 0L, 0L                                                \
    };

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


typedef struct __LVS_OUTPUT_PAYLOAD
{
  unsigned char* data;
  int size;
} LVS_OUTPUT_PAYLOAD;

#define LVS_DEFINE_OUTPUT(name, pipe_size)                        \
  LVS_DEFINE_PIPE(pipe_##name, pipe_size)                          \
  LVS_DEFINE_EVENT_TYPE(event_##name)                              \
  LVS_ERROR_T __lvs_output_##name(LVS_EVENT_T event)              \
  {                                                               \
    LVS_OUTPUT_PAYLOAD* payload = (LVS_OUTPUT_PAYLOAD*) event.payload;                  \
    LVS_ERROR_T result = LVS_WRITE_PIPE(pipe_##name, payload->data, payload->size);      \
    if (LVS_OK != result)                                         \
      lvs_SendEvent(event);                                       \
    return result;                                                \
  }                                                               \
  LVS_DEFINE_SUBSCRIBER(__lvs_output_##name)

#define LVS_INIT_OUTPUT(name)                                     \
  {                                                               \
    LVS_CREATE_PIPE(pipe_##name)                                   \
    LVS_CREATE_EVENT_TYPE(event_##name)                            \
    LVS_ADD_SUBSCRIBER(event_##name, __lvs_output_##name);         \
  }

#define LVS_USE_OUTPUT(name)                                      \
  LVS_USE_PIPE(pipe_##name)                                       \
  LVS_USE_EVENT_TYPE(event_##name)

#define LVS_SEND_TO_OUTPUT(name, dataptr, _size)                  \
  {                                                               \
    static LVS_OUTPUT_PAYLOAD payload;                            \
    payload.data = dataptr;                                       \
    payload.size = _size;                                         \
    LVS_SEND(event_##name, &payload);                              \
  }

#define LVS_GET_OUTPUT_PIPE(name) (__lvs_pipe_pipe_##name)
#define LVS_GET_OUTPUT_EVENT_TYPE(name) LVS_GET_EVENT_TYPE(event_##name)
#define LVS_GET_INPUT_PIPE(name) (__lvs_pipe_pipe_##name)
#define LVS_GET_INPUT_EVENT_TYPE(name) LVS_GET_EVENT_TYPE(event_##name)

#define LVS_DEFINE_INPUT(name, pipe_size)                         \
  LVS_DEFINE_PIPE(pipe_##name, pipe_size)                         \
  LVS_DEFINE_EVENT_TYPE(event_##name)

#define LVS_USE_INPUT(name)                                       \
  LVS_USE_PIPE(pipe_##name)                                       \
  LVS_USE_EVENT_TYPE(event_##name)

#define LVS_INIT_INPUT(name)                                      \
  {                                                               \
    LVS_CREATE_PIPE(pipe_##name)                                  \
    LVS_CREATE_EVENT_TYPE(event_##name)                           \
    LVS_GET_INPUT_PIPE(name).on_data = LVS_GET_EVENT_TYPE(event_##name); \
  }

#endif