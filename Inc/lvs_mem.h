/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) Konstantin Khait, 2013-2021*/
/********************************************************************************/
/* Generic memory operations, may be replaced with CRT library functions        */
/********************************************************************************/
#ifndef __LVS_MEM_H__
#define __LVS_MEM_H__

// Fill memory with the given template value
void lvs_memset(void* ptr, unsigned char value, int size);
// Compare memory blocks
int lvs_memcmp(void* p1, void* p2, int size);
// Copy memory block
void lvs_memcpy(void* p1, void* p2, int size);

#define LVS_START_DATA(name)                    \
  typedef struct __LVS_DATA_##name {            \
    unsigned char __used : 1;                   \

#define LVS_END_DATA(name)                      \
  } LVS_DATA_##name;

#define LVS_DEFINE_DATA_POOL(name, type, quantity)  \
  LVS_DATA_##type __lvs_data_pool_##name[quantity]; \
  const int __LVS_DATA_POOL_SIZE_##name = quantity; \
  void __lvs_init_data_pool_##name(void)            \
  {                                                 \
    int i;                                          \
    for (i = 0; i < quantity; i++)                  \
      memset(& __lvs_data_pool_##name[0], 0, sizeof(LVS_DATA_##type)); \
  };                                                \
  LVS_DATA_##type* __lvs_allocate_data_pool##name(void) \
  {                                             \
    int i;                                      \
    for (i = 0; i < quantity; i++)              \
      if (!__lvs_data_pool_##name[i].__used)    \
      {                                         \
        __lvs_data_pool_##name[i].__used = 1;   \
        return &__lvs_data_pool_##name[i];      \
      };                                        \
    return 0L;                                  \
  };                                            

#define LVS_CREATE_DATA_POOL(name)              \
  {__lvs_init_data_pool_##name();}

#define LVS_USE_DATA_POOL(name)                 \
  void __lvs_init_data_pool_##name();           \
  LVS_DATA_##type* __lvs_allocate_data_pool##name(void);

#define LVS_ALLOCATE(name) (__lvs_allocate_data_pool##name())

#define LVS_DEALLOCATE(data) {data->__used = 0;}

#define LVS_DATA(name) LVS_DATA_##name

#endif