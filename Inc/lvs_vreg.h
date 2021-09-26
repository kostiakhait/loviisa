#ifndef __LVS_VREG_H__
#define __LVS_VREG_H__

#include "lvs_cb.h"

typedef enum __LVS_VREG_TYPE
{
  LVS_VREG_BYTE,
  LVS_VREG_WORD,
  LVS_VREG_DWORD,
  LVS_VREG_SYMBOL,
  LVS_VREG_SHORTINT,
  LVS_VREG_INTEGER,
  LVS_VREG_NUMBER,
  LVS_VREG_STRING,
  LVS_VREG_UNKNOWN
} LVS_VREG_TYPE_T;

typedef union __LVS_VREG
{
  unsigned char   byte;
  unsigned short  word;
  unsigned long   dword;
  signed char     symbol;
  signed short    shortint;
  signed long     integer;
  double          number;
  char*           string;
} LVS_VREG_T;

typedef struct __LVS_VREG_HANDLER
{
  LVS_VREG_T payload;
  LVS_VREG_TYPE_T type;
  char* name;
  LVS_EVENT_ID_T on_change_event;
  struct __LVS_VREG_HANDLER* next;
  LVS_CALLBACK_T on_set;
} LVS_VREG_HANDLER_T;

extern LVS_VREG_HANDLER_T* __lvs_vregs;

#define LVS_DEFINE_VREG(name, type, event)  \
  LVS_VREG_HANDLER_T __lvs_vreg_##name;     \
  const LVS_VREG_TYPE_T __lvs_vreg_type_##name = type;  \
  const LVS_EVENT_ID_T* __lvs_vreg_evt_##name = &LVS_GET_EVENT_TYPE(event);   \
  char* __lvs_vreg_name_##name = #name;

#define LVS_USE_VREG(name)  \
  extern LVS_VREG_HANDLER_T __lvs_vreg_##name;     \
  extern const LVS_VREG_TYPE_T __lvs_vreg_type_##name;  \
  extern const LVS_EVENT_ID_T __lvs_vreg_evt_##name;    \
  extern char* __lvs_vreg_name_##name;

#define LVS_INIT_VREG(nname)                 \
  {                                         \
    __lvs_vreg_##nname.type = __lvs_vreg_type_##nname; \
    __lvs_vreg_##nname.name = __lvs_vreg_name_##nname; \
    __lvs_vreg_##nname.on_change_event = *__lvs_vreg_evt_##nname; \
    __lvs_vreg_##nname.on_set = 0L;               \
    __lvs_vreg_##nname.next = __lvs_vregs;        \
    __lvs_vregs = &__lvs_vreg_##nname;            \
  }

LVS_ERROR_T lvs_GetVReg(const char* name, LVS_VREG_T* value, LVS_VREG_TYPE_T* type);
LVS_ERROR_T lvs_SetVReg(const char* name, LVS_VREG_T value, LVS_VREG_TYPE_T type);
LVS_ERROR_T lvs_UpdateVReg(const char* name, LVS_VREG_T value, LVS_VREG_TYPE_T type);
LVS_ERROR_T lvs_SetVRegCB(const char* name, LVS_CALLBACK_T cb);

LVS_ERROR_T lvs_GetVRegByte(const char* name, unsigned char* byte);
LVS_ERROR_T lvs_GetVRegWord(const char* name, unsigned short* word);
LVS_ERROR_T lvs_GetVRegDWord(const char* name, unsigned long* dword);
LVS_ERROR_T lvs_GetVRegSymbol(const char* name, signed char* symbol);
LVS_ERROR_T lvs_GetVRegShortint(const char* name, signed short* shortint);
LVS_ERROR_T lvs_GetVRegInteger(const char* name, signed long* integer);
LVS_ERROR_T lvs_GetVRegNumber(const char* name, double* number);
LVS_ERROR_T lvs_GetVRegString(const char* name, char** string);

LVS_ERROR_T lvs_SetVRegByte(const char* name, LVS_VREG_T* value, unsigned char byte);
LVS_ERROR_T lvs_SetVRegWord(const char* name, LVS_VREG_T* value, unsigned short word);
LVS_ERROR_T lvs_SetVRegDWord(const char* name, LVS_VREG_T* value, unsigned long dword);
LVS_ERROR_T lvs_SetVRegSymbol(const char* name, LVS_VREG_T* value, signed char symbol);
LVS_ERROR_T lvs_SetVRegShortint(const char* name, LVS_VREG_T* value, signed short shortint);
LVS_ERROR_T lvs_SetVRegInteger(const char* name, LVS_VREG_T* value, signed long integer);
LVS_ERROR_T lvs_SetVRegNumber(const char* name, LVS_VREG_T* value, double number);
LVS_ERROR_T lvs_SetVRegString(const char* name, LVS_VREG_T* value, char* string);

#endif