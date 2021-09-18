#ifndef __LVS_CB_H__
#define __LVS_CB_H__

#include "lvs_event.h"

typedef void (*LVS_CALLBACK_T)(void* arg);

typedef struct __LVS_CB_CALL
{
  LVS_CALLBACK_T callback;
  void* arg;
} LVS_CB_CALL_T;

LVS_ERROR_T lvsCall(LVS_CALLBACK_T cb, void* arg);

void __lvsCBInit(void);

#endif