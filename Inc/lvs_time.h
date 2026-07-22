/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) Konstantin Khait, 2013-2021*/
/********************************************************************************/
/* Timer management functions                                                   */
/********************************************************************************/
#ifndef __LVS_TIME_H__
#define __LVS_TIME_H__

#include "lvs_event.h"
#include "lvs_cb.h"

// Timer handler structure
typedef struct __LVS_TIMER
{
  unsigned long tick_count;                                       // Global ticks at the last triggering moment
  unsigned long period_msec;                                      // Timer period
  unsigned long epoch;                                            // Number how much times timer was triggered
  LVS_EVENT_ID_T event;                                           // Event triggered by the timer (timer structure will be an argument)
  unsigned char enabled;                                          // 1 if timer is on, i.e. launched
  unsigned char periodic;                                         // 1 if timer is periodic, 0 if single shot
  struct __LVS_TIMER* next;                                       // Next timer structure
} LVS_TIMER_T;

extern LVS_TIMER_T* __lvs_timers;                                 // List of timer structures

void lvs_TimerTick(void);                                         // Function to be triggered by RTC on each HW timer event
unsigned long lvs_GetTickCount(void);                             // Get number of RTC ticks

// Define logical timer (once in C file in a global context)
#define LVS_DEFINE_TIMER(timer)                                   \
  LVS_TIMER_T __lvs_timer_##timer;

// Declare logical timer (anywhere before use)
#define LVS_USE_TIMER(timer)                                      \
  extern LVS_TIMER_T __lvs_timer_##timer;

// Initialize timer with event, period and periodic or single shot info
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

// Run the given timer
#define LVS_START_TIMER(timer)                                    \
  {                                                               \
    __lvs_timer_##timer.tick_count = lvs_GetTickCount();          \
    __lvs_timer_##timer.enabled = 1;                              \
  }

// Stop the given timer
#define LVS_STOP_TIMER(timer)                                     \
  {                                                               \
    __lvs_timer_##timer.enabled = 0;                              \
  }

#define LVS_START_TIMEOUT(timer)                                  \
  unsigned long __lvs_local_timer_##timer = lvs_GetTickCount();

#define LVS_RESET_TIMEOUT(timer)                                  \
  __lvs_local_timer_##timer = lvs_GetTickCount();

#define LVS_IF_TIMEOUT(timer, ms)                                 \
 {                                                                \
   static unsigned long __lvs_start_time = 0;                     \
   {lvs_PerformScheduler();};                                     \
   if (lvs_GetTickCount() -  __lvs_local_timer_##timer > LVS_TICKS_PER_SECOND * ms / 1000)  \
   {

// Get paramters from existing timer structure
#define LVS_GET_TIMER_PERIOD_MS(timer) (__lvs_timer_##timer.period_msec)
#define LVS_GET_TIMER_EPOCH(timer) (__lvs_timer_##timer.epoch)
#define LVS_IS_TIMER_ENABLED(timer) (__lvs_timer_##timer.enabled)
#define LVS_IS_TIMER_PERIODIC(timer) (__lvs_timer_##timer.periodic)

typedef struct __LVS_DELAYED_CALL
{
  LVS_CB_CALL_T cb;
  unsigned long start_time;
  unsigned long delay;
  unsigned char active;
  struct __LVS_DELAYED_CALL* next;
} LVS_DELAYED_CALL_T;

extern LVS_DELAYED_CALL_T* __lvs_delayed_calls;

#define LVS_DEFINE_DELAYED_CALL(name) \
  LVS_DELAYED_CALL_T __lvs_delayed_call_##name;

#define LVS_USE_DELAYED_CALL(name) \
  LVS_DELAYED_CALL_T __lvs_delayed_call_##name;

#define LVS_INIT_DELAYED_CALL(name) \
  {                                                             \
    __lvs_delayed_call_##name.active = 0;                       \
    __lvs_delayed_call_##name.next = __lvs_delayed_calls;       \
    __lvs_delayed_calls = &__lvs_delayed_call_##name;           \
  }

#define LVS_DELAYED_CALL(name, proc, argument, delay_ms) \
  {                                                             \
    __lvs_delayed_call_##name.start_time = lvs_GetTickCount();  \
    __lvs_delayed_call_##name.cb.callback = &proc;              \
    __lvs_delayed_call_##name.cb.arg = argument;                \
    __lvs_delayed_call_##name.delay = delay_ms;                 \
    __lvs_delayed_call_##name.active = 1;                       \
  }

#endif