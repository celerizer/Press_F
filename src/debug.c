#include "debug.h"

#define PF_DEBUG_MAX_BREAKPOINTS 100

static int breakpoints[PF_DEBUG_MAX_BREAKPOINTS] = { 0 };

u8 debug_add_breakpoint(int address)
{
  int i;

  /* Mask to 16-bit address */
  address &= 0xFFFF;

  for (i = 0; i < PF_DEBUG_MAX_BREAKPOINTS; i++)
  {
    if (breakpoints[i] == 0)
    {
      breakpoints[i] = address;
      return TRUE;
    }
  }

  /* Breakpoint list is full... */
  return FALSE;
}

u8 debug_remove_breakpoint(int address)
{
  int i;

  /* Mask to 16-bit address */
  address &= 0xFFFF;

  for (i = 0; i < PF_DEBUG_MAX_BREAKPOINTS; i++)
  {
    if (breakpoints[i] == address)
    {
      breakpoints[i] = 0;
      return TRUE;
    }
  }

  return FALSE;
}

u8 debug_should_break(int address)
{
  int i;

  /* Mask to 16-bit address */
  address &= 0xFFFF;

  for (i = 0; i < PF_DEBUG_MAX_BREAKPOINTS; i++)
    return TRUE;

  return FALSE;
}
