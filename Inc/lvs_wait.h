/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) LigerWays, 2013-2021       */
/********************************************************************************/
/* Wait primitives and flags                                                    */
/********************************************************************************/
#ifndef __LVS_WAIT_H__
#define __LVS_WAIT_H__

#include "lvs_event.h"
#include "lvs_config.h"
#include "lvs_bitmem.h"

// Execute following portion of code only if time in ms passed since its last execution. Prevents
// extra frequent execution of some code blocks
#define LVS_IF_PASSED(ms)                                            \
  {                                                                  \
    static unsigned long __lvs_start_time = 0;                       \
    {lvs_PerformScheduler();};                                       \
    if (!__lvs_start_time) __lvs_start_time = lvs_GetTickCount();    \
    if (lvs_GetTickCount() - __lvs_start_time > LVS_TICKS_PER_SECOND * ms / 1000) \
    {

// Execute given code only if flag is set (and resets the flag)
#define LVS_IF_FLAG(num)                                             \
  {                                                                  \
    static unsigned long __lvs_start_time = 0;                       \
    {lvs_PerformScheduler();};                                       \
    if (lvs_GetFlag(num))                                            \
    {                                                                \
      {lvs_ResetFlag(num);}

// Execute given code if the condition is true 
#define LVS_IF(condition)                                            \
  {                                                                  \
    static unsigned long __lvs_start_time = 0;                       \
    {lvs_PerformScheduler();};                                       \
    if (condition)                                                   \
    {

// Else section for any of LVS_IF_ operations
#define LVS_ELSE()                                                   \
    } else {                                                         \

// End LVS_IF_ operation
#define LVS_ENDIF()                                                  \
      {lvs_PerformScheduler();};                                     \
      __lvs_start_time = lvs_GetTickCount();                         \
    }                                                                \
  }

// Task or event handler waits for a given condition
#define LVS_WAIT_FOR(condition)               \
  {                                           \
    while (!(condition))                      \
    {                                         \
      if (LVS_OK != lvs_PerformScheduler())   \
        LVS_IDLE();                           \
    }                                         \
  }

// Initialize flags. Called by LVS_INIT() and shouldn't be performed manually
void lvs_InitFlags(void);
// Get flag from flags array
LVS_BIT_T lvs_GetFlag(int num);
// Set flag in flags array
void lvs_SetFlag(int num);
// Reset flag in flags array
void lvs_ResetFlag(int num);
// Delay on given milliseconds
void lvs_Delay(int msec);

// These macros must be used instead of functions that maybe changed in future
#define LVS_GET_FLAG(num) lvs_GetFlag(num);
#define LVS_SET_FLAG(num) lvs_SetFlag(num);
#define LVS_RESET_FLAG(num) lvs_ResetFlag(num);
#define LVS_WAIT_FLAG(num) LVS_WAIT_FOR(LVS_GET_FLAG(num))
#define LVS_DELAY(msec) {lvs_Delay(msec);}

#endif