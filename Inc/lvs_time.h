#ifndef __LVS_TIME_H__
#define __LVS_TIME_H__

#include "lvs_event.h"

typedef struct __LVS_TIMER
{
  unsigned long tick_count;
  unsigned long period_msec;
  unsigned long epoch;
  LVS_EVENT_ID_T event;
  unsigned char enabled;
  unsigned char periodic;
  struct __LVS_TIMER* next;
} LVS_TIMER_T;

extern LVS_TIMER_T* __lvs_timers;

void lvs_TimerTick(void);
unsigned long lvs_GetTickCount(void);

#define LVS_DEFINE_TIMER(timer)                                   \
  LVS_TIMER_T __lvs_timer_##timer;

#define LVS_USE_TIMER(timer)                                      \
  extern LVS_TIMER_T __lvs_timer_##timer;

#define LVS_INIT_TIMER(timer, _event, period, _periodic)          \
  {                                                               \
     __lvs_timer_##timer.tick_count = 0L;                         \
     __lvs_timer_##timer.period_msec = period;                    \
     __lvs_timer_##timer.epoch = 0L;                              \
     __lvs_timer_##timer.event = LVS_GET_EVENT_TYPE(_event);      \
     __lvs_timer_##timer.enabled = 0;                             \
     __lvs_timer_##timer.periodic = _periodic;                    \
     __lvs_timer_##timer.next = __lvs_timers;                     \
     __lvs_timers = &__lvs_timer_##timer;                         \
  }

#define LVS_START_TIMER(timer)                                    \
  {                                                               \
    __lvs_timer_##timer.tick_count = lvs_GetTickCount();          \
    __lvs_timer_##timer.enabled = 1;                              \
  }

#define LVS_STOP_TIMER(timer)                                     \
  {                                                               \
    __lvs_timer_##timer.enabled = 0;                              \
  }

#define LVS_GET_TIMER_PERIOD_MS(timer) (__lvs_timer_##timer.period_msec)
#define LVS_GET_TIMER_EPOCH(timer) (__lvs_timer_##timer.epoch)
#define LVS_IS_TIMER_ENABLED(timer) (__lvs_timer_##timer.enabled)
#define LVS_IS_TIMER_PERIODIC(timer) (__lvs_timer_##timer.periodic)

#endif