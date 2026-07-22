/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) Konstantin Khait, 2013-2021*/
/********************************************************************************/
/* Declarative management of state machines                                     */
/********************************************************************************/

#ifndef __LVS_SMACH_H__
#define __LVS_SMACH_H__

// Open states section
#define LVS_DEFINE_STATES(name)       \
  typedef enum ____lvs_smach_##name { \

// Open actions section
#define LVS_DEFINE_ACTIONS(name)      \
    __LVS_SMACH_UNDEFINED_##name = 0xFFFF \
  } __lvs_smach_##name;               \
  __lvs_smach_##name lvs_smach_##name =  __LVS_SMACH_UNDEFINED_##name;\
  typedef enum ____lvs_smach_act_##name {   

// Open reactions section
#define LVS_DEFINE_REACTIONS(name)    \
    __LVS_SMACH_UNDEFINED_ACTION##name = 0xFFFF \
  } __lvs_smach_actions_##name;       \
  void lvsSM_OnAction_##name(__lvs_smach_actions_##name action) {  \
    const __lvs_smach_##name state = lvs_smach_##name;                    \
    __lvs_smach_##name* __state_ptr = &lvs_smach_##name;                  \
    if (false) {

// Introduce state
#define LVS_STATE(name) __LVS_SMACH_##name,
// Introduce action
#define LVS_ACTION(name) __LVS_SMACH_ACTION_##name,

// Introduce reaction
#define LVS_ON_ACTION(fstate, faction)  } \
    else if ((state == __LVS_SMACH_##fstate) && (__LVS_SMACH_ACTION_##faction == action)) {

// Reaction on any action for given state
#define LVS_ON_ANY_ACTION(fstate)  } \
    else if (state == __LVS_SMACH_##fstate) {

// Reaction for the initial / undefined state
#define LVS_ON_UNDEFINED_STATE()  } \
    else if (state == 0xFFFF) {

// Complete state machine
#define LVS_END_DEFINITION(name) }}

// Action to state machine
#define LVS_DO_ACTION(machine, action) lvsSM_OnAction_##machine(__LVS_SMACH_ACTION_##action)

// Switch state
#define LVS_SWITCH_STATE(state) { *__state_ptr = __LVS_SMACH_##state; }

#endif