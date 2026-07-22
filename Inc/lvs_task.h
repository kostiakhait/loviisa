/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) Konstantin Khait, 2013-2021*/
/********************************************************************************/
/* Pseudo parallel tasks definitions. Tasks are cooperative and preempt each    */
/* other either at the end of task loop or upon yield call                      */
/********************************************************************************/
#ifndef __LVS_TASK_H__
#define __LVS_TASK_H__

#include "lvs_event.h"

// Tasks are implemented using sending of "goto" event to the common queue
LVS_USE_EVENT_TYPE(goto);

// Start task context (memory allocated for task variables)
// Task shall not use local variables except ones being used for one loop only
#define LVS_TASK_CONTEXT_START(task)                          \
struct ___lvs_task_context_##task                             \
  {                                                           \
    void* params;                                             \
    unsigned long long __counter;

// End task context
#define LVS_TASK_CONTEXT_END(task)                            \
  } __lvs_task_context_##task;

#define LVS_USE_TASK(task)                                    \
  LVS_ERROR_T __lvs_task_start_##task(void* arg);             \
  LVS_ERROR_T __lvs_task_init_##task(void* arg);              \
  extern unsigned char __lvs_task_running_##task;
  
// Start task initialization section
#define LVS_TASK_INIT(task)                                   \
  unsigned char __lvs_task_running_##task = 0;                \
  LVS_ERROR_T __lvs_task_start_##task(void* arg);             \
  LVS_ERROR_T __lvs_task_init_##task(void* arg)               \
  {                                                           \
    struct ___lvs_task_context_##task* lvs_context = &__lvs_task_context_##task;            \
    lvs_context->params = arg;                                \
    lvs_context->__counter = 0LL;                             \
    __lvs_task_running_##task = 1;

// End task initialization and start task loop
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

// End task loop
#define LVS_TASK_END(task)                                    \
  {                                                           \
    static LVS_GOTO_EVENT_PAYLOAD __task_end_ev_##task;       \
    __task_end_ev_##task.handler = &__lvs_task_start_##task;  \
    __task_end_ev_##task.arg = lvs_context->params;           \
    LVS_SEND(goto, &__task_end_ev_##task);                    \
    lvs_context->__counter += 1LL;                            \
  }}

#define LVS_USE_LOOP(task)                                    \
  LVS_ERROR_T __lvs_loop_start_##task(void* arg);             \
  extern unsigned char __lvs_loop_running_##task;

// End task initialization and start task loop
#define LVS_LOOP_START(task)                     \
  LVS_ERROR_T __lvs_loop_start_##task(void* arg)              \
  {                                                           \
    struct __lvs_context {                                    \
       unsigned long long __counter;                          \
       void* params } __context, *lvs_context;                \
    lvs_context = &__context;                                 \
    lvs_context->params = arg;                                \
    lvs_context->__counter = 0LL;                             \

// End task loop
#define LVS_LOOP_END(task)                                    \
  {                                                           \
    {lvs_PerformScheduler();};                                \
    static LVS_GOTO_EVENT_PAYLOAD __task_end_ev_##task;       \
    __task_end_ev_##task.handler = &__lvs_loop_start_##task;  \
    __task_end_ev_##task.arg = lvs_context->params;           \
    LVS_SEND(goto, &__task_end_ev_##task);                    \
    lvs_context->__counter += 1LL;                            \
  }}

#define LVS_LOOP_RUN(task, params)                              \
    {                                                           \
      static LVS_GOTO_EVENT_PAYLOAD __task_run_ev_##task;       \
      __task_run_ev_##task.handler = &__lvs_loop_start_##task;  \
      __task_run_ev_##task.arg = params;                        \
      LVS_SEND(goto, &__task_run_ev_##task);                    \
    }                                                           \

// Launch task
#define LVS_TASK_RUN(task, params)                              \
  {                                                             \
    if (!__lvs_task_running_##task)                             \
    {                                                           \
      static LVS_GOTO_EVENT_PAYLOAD __task_run_ev_##task;       \
      __task_run_ev_##task.handler = &__lvs_task_init_##task;   \
      __task_run_ev_##task.arg = params;                        \
      __lvs_task_running_##task = 1;                            \
      LVS_SEND(goto, &__task_run_ev_##task);                    \
    }                                                           \
  }

// Exit task from itself
#define LVS_TASK_EXIT(task) {__lvs_task_running_##task = 0; return LVS_OK;}
#define LVS_LOOP_EXIT(task) {return LVS_OK;}

#define LVS_IS_TASK_RUNNING(task) (__lvs_task_running_##task)

// Get variable from task's context, only available from inside the task
#define LVS_CTX(var) (lvs_context->var)

// Yield for another event
#define LVS_TASK_YIELD(task) {lvs_PerformScheduler();}

// Get task loop counter
#define LVS_TASK_COUNTER(task) (lvs_context->__counter)
#define LVS_TASK_PARAM(task) (lvs_context->params)

// Main function replacement
#define LVS_MAIN_BEGIN()                                        \
  int main(int argc, char** argv) {                             \
    LVS_INIT()                                                  \

#define LVS_MAIN_END()                                          \
    LVS_RUN();                                                  \
  }

#endif