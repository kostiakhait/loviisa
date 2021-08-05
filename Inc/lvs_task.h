#ifndef __LVS_TASK_H__
#define __LVS_TASK_H__

#include "lvs_event.h"

LVS_USE_EVENT_TYPE(goto);

#define LVS_TASK_CONTEXT_START(task)                          \
struct ___lvs_task_context_##task                             \
  {                                                           \
    void* params;                                             \
    unsigned long long __counter;

#define LVS_TASK_CONTEXT_END(task)                            \
  } __lvs_task_context_##task;
  
#define LVS_TASK_INIT(task)                                   \
  LVS_ERROR_T __lvs_task_start_##task(void* arg);             \
  LVS_ERROR_T __lvs_task_init_##task(void* arg)               \
  {                                                           \
    struct ___lvs_task_context_##task* lvs_context = &__lvs_task_context_##task;            \
    lvs_context->params = arg;                                \
    lvs_context->__counter = 0LL;

#define LVS_TASK_START(task)                                  \
  {                                                           \
    static LVS_GOTO_EVENT_PAYLOAD __task_init_ev_##task;      \
    __task_init_ev_##task.handler = &__lvs_task_start_##task; \
    __task_init_ev_##task.arg = lvs_context->params;          \
    LVS_SEND(goto, &__task_init_ev_##task);                   \
  }};                                                         \
  LVS_ERROR_T __lvs_task_start_##task(void* arg)              \
  {                                                           \
    struct ___lvs_task_context_##task* lvs_context = &__lvs_task_context_##task;

#define LVS_TASK_END(task)                                    \
  {                                                           \
    static LVS_GOTO_EVENT_PAYLOAD __task_end_ev_##task;       \
    __task_end_ev_##task.handler = &__lvs_task_start_##task;  \
    __task_end_ev_##task.arg = lvs_context->params;           \
    LVS_SEND(goto, &__task_end_ev_##task);                    \
    lvs_context->__counter += 1LL;                            \
  }}

#define LVS_TASK_RUN(task, params)                            \
  {                                                           \
    static LVS_GOTO_EVENT_PAYLOAD __task_run_ev_##task;       \
    __task_run_ev_##task.handler = &__lvs_task_init_##task;   \
    __task_run_ev_##task.arg = params;                        \
    LVS_SEND(goto, &__task_run_ev_##task);                    \
  }

#define LVS_TASK_EXIT(task) {return LVS_OK;}

#define LVS_CTX(var) (lvs_context->var)

#define LVS_TASK_YIELD(task) {lvs_PerformScheduler();}

#define LVS_TASK_COUNTER(task) (lvs_context->__counter)

#endif