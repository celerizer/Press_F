#ifndef PRESS_F_HLE_C
#define PRESS_F_HLE_C

#include "hle.h"
#include "hw/channelf.h"

#define HLE(a) void a(channelf *system)

/* 0000 */
HLE(reset)
{
   system->c3850.accumulator = 0;
   system->c3850.pc0++;
}

/* 0001 */
HLE(init)
{
   u8 i;

   for (i = 0; i < SCRATCH_SIZE; i++)
      system->c3850.scratchpad[i] = 0;
}

/* 0020 - 0036 */
HLE(no_cart_init)
{
}

/* 00D0 - 00D5 */
HLE(clear_screen)
{
}

/* 0107 - 011D */
HLE(push_k)
{
}

/* 011E - 0134 */
HLE(pop_k)
{
}

#endif