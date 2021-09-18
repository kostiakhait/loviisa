#include "lvs_cb.h"
#include "lvs_mem.h"
#include "lvs_config.h"

#ifndef LVS_NUMBER_OF_CB_CALLS
#define LVS_NUMBER_OF_CB_CALLS 4
#endif

LVS_DEFINE_EVENT_TYPE(lvs_cb_call);

LVS_START_DATA(lvs_cb_data)
  LVS_CB_CALL_T callback;
LVS_END_DATA(lvs_cb_data)

LVS_DEFINE_DATA_POOL(__lvs_cb_data_pool, lvs_cb_data, LVS_NUMBER_OF_CB_CALLS);

static LVS_ERROR_T __lvs_on_cb(LVS_EVENT_T event)
{
  LVS_DATA(lvs_cb_data)* cb_call = (LVS_DATA(lvs_cb_data)*) event.payload;
  if (cb_call->callback.callback)
    cb_call->callback.callback(cb_call->callback.arg);
  LVS_DEALLOCATE(cb_call);
  return LVS_OK;
};

LVS_DEFINE_SUBSCRIBER(__lvs_on_cb);

void __lvsCBInit(void)
{
  LVS_CREATE_EVENT_TYPE(lvs_cb_call);
  LVS_ADD_SUBSCRIBER(lvs_cb_call, __lvs_on_cb);
  LVS_CREATE_DATA_POOL(__lvs_cb_data_pool);
};

LVS_ERROR_T lvsCall(LVS_CALLBACK_T cb, void* arg)
{
  LVS_DATA(lvs_cb_data)* cb_call = LVS_ALLOCATE(__lvs_cb_data_pool);
  if (!cb_call)
    return LVS_OVERFLOW;
  cb_call->callback.callback = cb;
  cb_call->callback.arg = arg;
  LVS_EVENT_T event = {LVS_GET_EVENT_TYPE(lvs_cb_call), cb_call};
  return lvs_SendEvent(event);
};