/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) Konstantin Khait, 2013-2021*/
/********************************************************************************/
/* Timer management functions                                                   */
/********************************************************************************/
#include "lvs_time.h"

LVS_TIMER_T* __lvs_timers = 0L;
static unsigned long __tick_count = 0;
LVS_DELAYED_CALL_T* __lvs_delayed_calls = 0L;

void lvs_TimerTick(void)
{
  __tick_count += 1;
};

unsigned long lvs_GetTickCount(void)
{
  return __tick_count;
};

void __lvs_OnTimer(void)
{
  LVS_TIMER_T* timer = __lvs_timers;
  while (timer)
  {
    if (timer->enabled)
    {
      unsigned long ticks_passed = __tick_count - timer->tick_count;
      unsigned long period_in_ticks = (timer->period_msec * LVS_TICKS_PER_SECOND) / 1000;
      if (ticks_passed >= period_in_ticks)
      {
        timer->tick_count = __tick_count;
        timer->epoch += 1;
        LVS_EVENT_T event;
        event.id = timer->event;
        event.payload = timer;
        lvs_SendEvent(event);
        if (!timer->periodic)
          timer->enabled = 0;
      };
    };
    timer = timer->next;
  };
};

void __lvs_OnDelayedCalls(void)
{
  LVS_DELAYED_CALL_T* dcall = __lvs_delayed_calls;
  while (dcall)
  {
    if (dcall->active)
    {
      unsigned long ticks_passed = __tick_count - dcall->start_time;
      unsigned long period_in_ticks = (dcall->delay * LVS_TICKS_PER_SECOND) / 1000;
      if (ticks_passed >= period_in_ticks)
      {
        dcall->active = 0;
        lvsCall(dcall->cb.callback, dcall->cb.arg);
      };
    };
    dcall = dcall->next;
  };
};

void lvs_OnTimer(void)
{
  __lvs_OnTimer();
  __lvs_OnDelayedCalls();
};