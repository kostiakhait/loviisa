#ifndef __LVS_WAIT_H__
#define __LVS_WAIT_H__

#include "lvs_event.h"
#include "lvs_config.h"
#include "lvs_bitmem.h"

#define LVS_WAIT_FOR(condition)               \
  {                                           \
    while (!(condition))                      \
    {                                         \
      if (LVS_OK != lvs_PerformScheduler())   \
        LVS_IDLE();                           \
    }                                         \
  }

void lvs_InitFlags(void);
LVS_BIT_T lvs_GetFlag(int num);
void lvs_SetFlag(int num);
void lvs_ResetFlag(int num);
void lvs_Delay(int msec);

#define LVS_GET_FLAG(num) lvs_GetFlag(num);
#define LVS_SET_FLAG(num) lvs_SetFlag(num);
#define LVS_RESET_FLAG(num) lvs_ResetFlag(num);
#define LVS_WAIT_FLAG(num) LVS_WAIT_FOR(LVS_GET_FLAG(num))
#define LVS_DELAY(msec) {lvs_Delay(msec);}

#endif