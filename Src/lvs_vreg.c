#include "lvs_vreg.h"

LVS_VREG_HANDLER_T* __lvs_vregs = 0L;

static int __lvs_strcmp(const char* s1, const char* s2)
{
  for (int i = 0; ;i++)
  {
    if (s1[i] == s2[i])
    {
      if (s1[i] == 0)
        return 0;
    }
    else
    {
      if (s1[i] == 0)
        return -1;
      if (s2[i] == 0)
        return 1;
      return s1[i] < s2[i] ? -1 : 1;
    };
  };
};

static LVS_VREG_HANDLER_T* __lvs_find_vreg(const char* name)
{
  LVS_VREG_HANDLER_T* current = __lvs_vregs;
  while (current)
  {
    if (!__lvs_strcmp(name, current->name))
      return current;
    current = current->next;
  }
  return 0L;
};

LVS_ERROR_T lvs_GetVReg(const char* name, LVS_VREG_T* value, LVS_VREG_TYPE_T* type)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  *value = handler->payload;
  *type = handler->type;
  return LVS_OK;
};

LVS_ERROR_T lvs_UpdateVReg(const char* name, LVS_VREG_T value, LVS_VREG_TYPE_T type)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != type)
    return LVS_WRONG_TYPE;
  handler->payload = value;
  LVS_EVENT_T event;
  event.id = handler->on_change_event;
  event.payload = handler;
  lvs_SendEvent(event);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVReg(const char* name, LVS_VREG_T value, LVS_VREG_TYPE_T type)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != type)
    return LVS_WRONG_TYPE;
  handler->payload = value;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVRegCB(const char* name, LVS_CALLBACK_T cb)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  handler->on_set = cb;
  return LVS_OK;
};

LVS_ERROR_T lvs_GetVRegByte(const char* name, unsigned char* byte)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_BYTE)
    return LVS_WRONG_TYPE;
  *byte = handler->payload.byte;
  return LVS_OK;
};

LVS_ERROR_T lvs_GetVRegWord(const char* name, unsigned short* word)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_WORD)
    return LVS_WRONG_TYPE;
  *word = handler->payload.word;
  return LVS_OK;
};

LVS_ERROR_T lvs_GetVRegDWord(const char* name, unsigned long* dword)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_DWORD)
    return LVS_WRONG_TYPE;
  *dword = handler->payload.dword;
  return LVS_OK;
};

LVS_ERROR_T lvs_GetVRegSymbol(const char* name, signed char* symbol)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_SYMBOL)
    return LVS_WRONG_TYPE;
  *symbol = handler->payload.symbol;
  return LVS_OK;
};

LVS_ERROR_T lvs_GetVRegShortint(const char* name, signed short* shortint)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_SHORTINT)
    return LVS_WRONG_TYPE;
  *shortint = handler->payload.shortint;
  return LVS_OK;
};

LVS_ERROR_T lvs_GetVRegInteger(const char* name, signed long* integer)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_INTEGER)
    return LVS_WRONG_TYPE;
  *integer = handler->payload.integer;
  return LVS_OK;
};

LVS_ERROR_T lvs_GetVRegNumber(const char* name, double* number)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_NUMBER)
    return LVS_WRONG_TYPE;
  *number = handler->payload.number;
  return LVS_OK;
};

LVS_ERROR_T lvs_GetVRegString(const char* name, char** string)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_STRING)
    return LVS_WRONG_TYPE;
  *string = handler->payload.string;
  return LVS_OK;
};


LVS_ERROR_T lvs_SetVRegByte(const char* name, LVS_VREG_T* value, unsigned char byte)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_BYTE)
    return LVS_WRONG_TYPE;
  handler->payload.byte = byte;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVRegWord(const char* name, LVS_VREG_T* value, unsigned short word)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_WORD)
    return LVS_WRONG_TYPE;
  handler->payload.word = word;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVRegDWord(const char* name, LVS_VREG_T* value, unsigned long dword)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_DWORD)
    return LVS_WRONG_TYPE;
  handler->payload.dword = dword;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVRegSymbol(const char* name, LVS_VREG_T* value, signed char symbol)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_SYMBOL)
    return LVS_WRONG_TYPE;
  handler->payload.symbol = symbol;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVRegShortint(const char* name, LVS_VREG_T* value, signed short shortint)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_SHORTINT)
    return LVS_WRONG_TYPE;
  handler->payload.shortint = shortint;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVRegInteger(const char* name, LVS_VREG_T* value, signed long integer)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_INTEGER)
    return LVS_WRONG_TYPE;
  handler->payload.integer = integer;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVRegNumber(const char* name, LVS_VREG_T* value, double number)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_NUMBER)
    return LVS_WRONG_TYPE;
  handler->payload.number = number;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

LVS_ERROR_T lvs_SetVRegString(const char* name, LVS_VREG_T* value, char* string)
{
  LVS_VREG_HANDLER_T* handler = __lvs_find_vreg(name);
  if (!handler)
    return LVS_NOT_FOUND;
  if (handler->type != LVS_VREG_STRING)
    return LVS_WRONG_TYPE;
  handler->payload.string = string;
  if (handler->on_set)
    return lvsCall(handler->on_set, handler);
  return LVS_OK;
};

