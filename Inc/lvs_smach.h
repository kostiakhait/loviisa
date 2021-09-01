#ifndef __LVS_SMACH_H__
#define __LVS_SMACH_H__

#define LVS_DEFINE_STATES(name)       \
  typedef enum ____lvs_smach_##name { \

#define LVS_DEFINE_ACTIONS(name)      \
    __LVS_SMACH_UNDEFINED_##name = 0xFFFF \
  } __lvs_smach_##name;               \
  extern __lvs_smach_##name lvs_smach_##name;\
  typedef enum ____lvs_smach_act_##name {

#define LVS_DEFINE_REACTIONS(name)    \
  } __lvs_smach_actions_##name;       \
  inline void lvsSM_OnAction_##name(__lvs_smach_actions_##name action) {  \
    const __lvs_smach_##name state = lvs_smach_##name;                    \
    __lvs_smach_##name* __state_ptr = &lvs_smach_##name;                  \
    if (false) {

#define LVS_STATE(name) __LVS_SMACH_##name,
#define LVS_ACTION(name) __LVS_SMACH_ACTION_##name,

#define LVS_ON_ACTION(fstate, faction)  } \
    else if ((state == __LVS_SMACH_##fstate) && (__LVS_SMACH_ACTION_##faction == action)) {

#define LVS_ON_ANY_ACTION(fstate)  } \
    else if (state == __LVS_SMACH_##fstate) {

#define LVS_END_DEFINITION(name) }}

#define LVS_DO_ACTION(machine, action) lvsSM_OnAction_##machine(__LVS_SMACH_ACTION_##action)

#define LVS_SWITCH_STATE(state) { *__state_ptr = __LVS_SMACH_##state; }

#define LVS_USE_STATE_MACHINE(name) void lvsSM_OnAction_##name(__lvs_smach_actions_##name action);

#define LVS_DECLARE_STATE_MACHINE(name) \
  __lvs_smach_##name lvs_smach_##name =  __LVS_SMACH_UNDEFINED_##name;

#define LVS_UNDEFINED_STATE(name) 0xFFFF


#endif