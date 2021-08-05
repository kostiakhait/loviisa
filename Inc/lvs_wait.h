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