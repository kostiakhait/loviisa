/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) LigerWays, 2013-2021       */
/********************************************************************************/
/* Events management core                                                       */
/********************************************************************************/
#ifndef __LVS_EVENT_H__
#define __LVS_EVENT_H__

#include "lvs_error.h"
#include "lvs_ringbuf.h"
#include "lvs_wait.h"

typedef char* LVS_EVENT_ID_T;       // Unique handler of the event
typedef void* LVS_EVENT_PAYLOAD_T;  // Pointer to event's payload
typedef struct __LVS_EVENT
{
  LVS_EVENT_ID_T      id;           // Event identifier
  LVS_EVENT_PAYLOAD_T payload;      // Event payload
} LVS_EVENT_T;                      // Event type

// Event handler function
typedef LVS_ERROR_T (*LVS_EVENT_HANDLER)(LVS_EVENT_T event);

// Auxiliary type, list of handlers to handle given event type
typedef struct __LVS_EVENT_SUBSCRIPTION
{
  LVS_EVENT_HANDLER handler;
  struct __LVS_EVENT_SUBSCRIPTION* next;
} LVS_EVENT_SUBSCRIPTION;

// Auxiliary type, list of all event types with handlers
typedef struct __LVS_EVENT_ID_REC
{
  LVS_EVENT_ID_T id;                      // Event id
  LVS_EVENT_SUBSCRIPTION* subscribers;    // List of subscribers
  struct __LVS_EVENT_ID_REC* next;        // Next type record
} LVS_EVENT_ID_REC;

// Structure to store the head of the event types list
typedef struct __LVS_EVENT_HANDLERS
{
  LVS_EVENT_ID_REC* handlers;
} LVS_EVENT_HANDLERS;

// Global event types list
extern LVS_EVENT_HANDLERS __lvs_event_handler;

// Define new event type. Shall be used once outside of any function. Not in a header file
#define LVS_DEFINE_EVENT_TYPE(typen)                              \
  const LVS_EVENT_ID_T   __lvs_event_id_##typen = #typen;         \
  LVS_EVENT_ID_REC __lvs_event_type_##typen = {__lvs_event_id_##typen, 0L, 0L};

// Declare event type. Used wherever this event type is needed
#define LVS_USE_EVENT_TYPE(typen)                                 \
  extern const LVS_EVENT_ID_T   __lvs_event_id_##typen;           \
  extern LVS_EVENT_ID_REC __lvs_event_type_##typen;

// Get event type from event type name
#define LVS_GET_EVENT_TYPE(typen)  (__lvs_event_id_##typen)

// Initialize event type. Must be executed in the initialization function
#define LVS_CREATE_EVENT_TYPE(typen)                              \
  {                                                               \
    __lvs_event_type_##typen.id = __lvs_event_id_##typen;         \
    __lvs_event_type_##typen.subscribers = 0L;                    \
    __lvs_event_type_##typen.next = __lvs_event_handler.handlers; \
    __lvs_event_handler.handlers = &__lvs_event_type_##typen;     \
  }

// Define event handler. Multiple handlers can be used for a single event type
#define LVS_DEFINE_SUBSCRIBER(subs)                               \
  LVS_EVENT_SUBSCRIPTION __lvs_event_subs_##subs = {subs, 0L}

// Declare event handler. Can be used everywhere if event handler is needed
#define LVS_USE_SUBSCRIBER(subs)                                  \
  extern LVS_EVENT_SUBSCRIPTION __lvs_event_subs_##subs;

// Add event handler. Must be called once in the initialization function
#define LVS_ADD_SUBSCRIBER(typen, subs)                           \
  {                                                               \
    LVS_EVENT_ID_REC* __current = __lvs_event_handler.handlers;   \
    while (0L != __current)                                       \
    {                                                             \
      if (__current->id == __lvs_event_type_##typen.id)           \
      {                                                           \
        __lvs_event_subs_##subs.next = __current->subscribers;    \
        __current->subscribers = &__lvs_event_subs_##subs;        \
         break;                                                   \
      };                                                          \
      __current = __current->next;                                \
    };                                                            \
  }

// Auxiliary definition of the event queue of the given size
#define LVS_DEFINE_EVENT_QUEUE(size)                              \
  LVS_DEFINE_RINGBUF(lvs_event_queue, sizeof(LVS_EVENT_T) * size)

// Payload structure for the special "goto" event that just
// transfers the control to a given address
typedef struct __LVS_GOTO_EVENT_PAYLOAD
{
  LVS_ERROR_T (*handler)(void*);          // Goto event handler
  void *arg;                              // Argument to be passed to the goto event
} LVS_GOTO_EVENT_PAYLOAD;

// Initialize event scheduler
void lvs_InitEventScheduler(void);
// Send event to the queue
LVS_ERROR_T lvs_SendEvent(LVS_EVENT_T event);
// Read single event from the queue
LVS_ERROR_T lvs_ReadEvent(LVS_EVENT_T* event);
// Schedule given event
LVS_ERROR_T lvs_ScheduleEvent(LVS_EVENT_T event);
// Run scheduler until stopped
void lvs_RunScheduler(void);
// Run scheduler until queue is empty
LVS_ERROR_T lvs_PerformScheduler(void);
void lvs_StopScheduler(void);
void lvs_Init(void);

// Initialize the scheduler
#define LVS_INIT() lvs_Init();
// Run the scheduler
#define LVS_RUN() lvs_RunScheduler()
// Stop the scheduler
#define LVS_STOP() lvs_StopScheduler()
// Send event of the given type with the given argument
#define LVS_SEND(typen, arg)                                      \
  {                                                               \
    LVS_EVENT_T __event;                                          \
    __event.id = __lvs_event_id_##typen;                          \
    __event.payload = arg;                                        \
    lvs_SendEvent(__event);                                       \
  }

#endif