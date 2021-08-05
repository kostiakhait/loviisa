#include "lvs_wait.h"
#include "lvs_mem.h"
#include "lvs_time.h"

LVS_ALLOCATE_BITMEM(lvs_flags, LVS_FLAGS_USED);

void lvs_InitFlags(void)
{
  lvs_memset(lvs_flags, 0, LVS_BITMEM_SIZE(lvs_flags));
};

LVS_BIT_T lvs_GetFlag(int num)
{
  if (num >= LVS_FLAGS_USED)
    LVS_FATAL_ERROR()
  else
    return lvs_BitmemGet1(lvs_flags, num);
}

void lvs_SetFlag(int num)
{
  if (num >= LVS_FLAGS_USED)
    LVS_FATAL_ERROR()
  else
    lvs_BitmemPut1(lvs_flags, num, 1);
};

void lvs_ResetFlag(int num)
{
  if (num >= LVS_FLAGS_USED)
    LVS_FATAL_ERROR()
  else
    lvs_BitmemPut1(lvs_flags, num, 0);
};

void lvs_Delay(int msec)
{
  unsigned long start_time = lvs_GetTickCount();
  unsigned long ticks_passed;
  unsigned long period_in_ticks;

  do
  {
      ticks_passed = lvs_GetTickCount() - start_time;
      period_in_ticks = (msec * LVS_TICKS_PER_SECOND) / 1000;
      lvs_PerformScheduler();
  } while (ticks_passed < period_in_ticks);
};
