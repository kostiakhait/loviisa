/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) LigerWays, 2013-2021       */
/********************************************************************************/
/* Loviisa Configuration File. Redefine configuration parameters if needed      */
/********************************************************************************/

#ifndef __LVS_CONFIG_H__
#define __LVS_CONFIG_H__

#define LVS_EVENT_QUEUE_SIZE 32         // Maximum size of the event queue in events

// Define LVS_IDLE as a function that you want to be executed all the time when
// events scheduler has no event to process
#ifndef LVS_IDLE
#define LVS_IDLE() ;
#endif

// Number of flags used
#ifndef LVS_FLAGS_USED
#define LVS_FLAGS_USED 1
#endif

// Fatal error handler
#ifndef LVS_FATAL_ERROR
#define LVS_FATAL_ERROR() {while (1);}
#endif

// Ticks per second
#ifndef LVS_TICKS_PER_SECOND
#define LVS_TICKS_PER_SECOND 512
#endif

#endif