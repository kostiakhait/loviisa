/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) LigerWays, 2013-2021       */
/********************************************************************************/
/* Events management core                                                       */
/********************************************************************************/
#include "lvs_event.h"
#include "lvs_config.h"
#include "lvs_cb.h"

void lvs_OnTimer(void);
void lvs_SyncDrvBufs(void);

LVS_DEFINE_EVENT_QUEUE(LVS_EVENT_QUEUE_SIZE);

LVS_DEFINE_EVENT_TYPE(goto);
LVS_DEFINE_EVENT_TYPE(null);

LVS_EVENT_HANDLERS __lvs_event_handler;

static int __stop_scheduler = 0;
static int __lock = 0;
static int __scheduler_locked = 0;

LVS_ERROR_T __lvs_goto(LVS_EVENT_T event)
{
  LVS_GOTO_EVENT_PAYLOAD* goto_evt_payload = (LVS_GOTO_EVENT_PAYLOAD*) event.payload;
  if (goto_evt_payload->handler == 0L)
    return LVS_NO_HANDLER;
  return (*goto_evt_payload->handler)(goto_evt_payload->arg);
};

LVS_DEFINE_SUBSCRIBER(__lvs_goto);

void lvs_InitEventScheduler(void)
{
  LVS_RINGBUF_INIT(lvs_event_queue);
  LVS_CREATE_EVENT_TYPE(goto);
  LVS_CREATE_EVENT_TYPE(null)
  LVS_ADD_SUBSCRIBER(goto, __lvs_goto);
};

LVS_ERROR_T lvs_SendEvent(LVS_EVENT_T event)
{
  while (__lock);
  __lock = 1;
  int size = lvs_RingbufGetFreeSpace(&LVS_RINGBUF(lvs_event_queue));
  if (size < sizeof(event))
    LVS_FATAL_ERROR();
  LVS_ERROR_T result = lvs_RingbufWrite(&LVS_RINGBUF(lvs_event_queue), (unsigned char*)&event, sizeof(event));
  __lock = 0;
  return result;
};

LVS_ERROR_T lvs_ReadEvent(LVS_EVENT_T* event)
{
  LVS_ERROR_T result;
  while (__lock);
  __lock = 1;
  int size = lvs_RingbufGetSize(&LVS_RINGBUF(lvs_event_queue));
  if (size < sizeof(event))
    result = LVS_UNDERFLOW;
  else
    result = lvs_RingbufRead(&LVS_RINGBUF(lvs_event_queue), (unsigned char*)event, sizeof(LVS_EVENT_T));
  __lock = 0;
  return result;
};

LVS_ERROR_T lvs_ScheduleEvent(LVS_EVENT_T event)
{
  LVS_EVENT_ID_REC* current = __lvs_event_handler.handlers;
  while (current != 0L)
  {
    if (current->id == event.id)
    {
      LVS_EVENT_SUBSCRIPTION* subscribers = current->subscribers;
      while (subscribers != 0L)
      {
        if (subscribers->handler)
        {
          LVS_ERROR_T err = (*subscribers->handler)(event);
          if (err == LVS_INTERRUPT)
            return err;
        };
        subscribers = subscribers->next;
      };
      break;
    };
    current = current->next;
  };
  return LVS_OK;
};

void lvs_RunScheduler(void)
{
  do
  {
    LVS_EVENT_T event;
    LVS_ERROR_T err;
    do {
      if (__scheduler_locked)
        break;
      err = lvs_ReadEvent(&event);
      if (err == LVS_OK)
        lvs_ScheduleEvent(event);
      else
        break;
    } while (1);
    lvs_OnTimer();
    lvs_SyncDrvBufs();
    LVS_IDLE();
  } while (!__stop_scheduler);
  __stop_scheduler = 0;
};

LVS_ERROR_T lvs_PerformScheduler(void)
{
  LVS_EVENT_T event;
  LVS_ERROR_T err;
  if (__scheduler_locked)
    return LVS_RESOURCE_LOCKED;
  err = lvs_ReadEvent(&event);
  if (err == LVS_OK)
    lvs_ScheduleEvent(event);
  lvs_OnTimer();
  lvs_SyncDrvBufs();
  return err;
};

void lvs_StopScheduler(void)
{
  __stop_scheduler = 1;
};

void lvs_Init(void)
{
  lvs_InitEventScheduler(); 
  lvs_InitFlags();
  __lvsCBInit();
};

void lvs_LockScheduler(void)
{
  __scheduler_locked = 1;
};

void lvs_UnlockScheduler(void)
{
  __scheduler_locked = 0;
};
