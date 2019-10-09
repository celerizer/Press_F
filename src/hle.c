#ifndef PRESS_F_HLE_C
#define PRESS_F_HLE_C

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "hle.h"

#define HLE(a) void a(channelf_t *system)

/* 0000 */
HLE(reset)
{
   system->c3850.accumulator = 0;
}

/* 0001 - 0019 */
HLE(init)
{
   /* 0001 - 0009 */
   /* Reset all scratchpad registers to 00 */
   memset(system->c3850.scratchpad, 0, SCRATCH_SIZE);

   /* 000A - 000E */
   /* Initialize the stack pointer */
   system->c3850.accumulator = 40;
   system->c3850.scratchpad[59] = 40;

   /* 000F - 0019 */
   /* Jump to either the built-in game or the inserted cartridge */
   if (system->rom[0x0800] != 0x55)
      system->pc0 = 0x001A - 1;
   else
      system->pc0 = 0x0802 - 1;
}

/* 001A - 001F */
HLE(no_cart_init)
{
   /* Run the screen clear routine */
   system->c3850.scratchpad[0x03] = 0xD6;
   system->pc0 = 0x00D0 - 1;
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

void* hle_get_func_from_addr(u16 address)
{
   switch (address)
   {
   case 0x0000:
      return reset;
   case 0x0001:
      return init;
   case 0x001A:
      return no_cart_init;
   }

   return NULL;
}

#endif
