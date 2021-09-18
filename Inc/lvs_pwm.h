#ifndef __LVS_PWM_H__
#define __LVS_PWM_H__

#include "lvs_time.h"
#include "lvs_cb.h"

#define LVS_DEFINE_PWM(name, length, period, cb)  \
  LVS_DEFINE_EVENT_TYPE(lvs_pwm_time_##name);     \
  extern unsigned char __lvs_pwm_##name[length];  \
  unsigned char __lvs_pwm_on_##name = 1;          \
  static LVS_ERROR_T __lvs_on_pwm_timer_##name(LVS_EVENT_T evt) \
  {                                               \
    static int counter = 0;                       \
    if (__lvs_pwm_on_##name )                     \
      lvsCall(cb, (void*) __lvs_pwm_##name[counter]); \
    counter = (counter + 1) % length;             \
    return LVS_OK;                                \
  }                                               \
  LVS_DEFINE_SUBSCRIBER(__lvs_on_pwm_timer_##name); \
  LVS_DEFINE_TIMER(__lvs_pwm_timer_##name);       \
  unsigned char __lvs_pwm_period_##name = period; \
  unsigned char __lvs_pwm_##name[length]

#define LVS_USE_PWM(name, length)  \
  extern unsigned char __lvs_pwm_period_##name; \
  extern unsigned char __lvs_pwm_on_##name;     \
  extern unsigned char __lvs_pwm_##name[length];

#define LVS_INIT_PWM(name)                      \
  {                                             \
    LVS_CREATE_EVENT_TYPE(lvs_pwm_time_##name); \
    LVS_ADD_SUBSCRIBER(lvs_pwm_time_##name, __lvs_on_pwm_timer_##name);                     \
    LVS_INIT_TIMER(__lvs_pwm_timer_##name, lvs_pwm_time_##name, __lvs_pwm_period_##name, 1);\
    LVS_START_TIMER(__lvs_pwm_timer_##name);                                                \
  }

#define LVS_PWM_ON(name) {__lvs_pwm_on_##name = 1;}
#define LVS_PWM_OFF(name) {__lvs_pwm_on_##name = 0;}

#endif