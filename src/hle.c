#ifndef PRESS_F_HLE_C
#define PRESS_F_HLE_C

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hle.h"
#include "screen.h"

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
   memset(system->vram, 0x0, sizeof(system->vram));
   system->pc0 = system->pc1 - 1;
}

/* 0107 - 011D */
/* TODO: ISARize both of these */
HLE(push_k)
{
   u8 stack = system->c3850.scratchpad[59];

   memcpy(&system->c3850.scratchpad[stack], &system->c3850.scratchpad[12], 2);
   system->c3850.scratchpad[59] += 2;

   system->pc0 = system->pc1 - 1;
}

/* 011E - 0134 */
HLE(pop_k)
{
   u8 stack = system->c3850.scratchpad[59];

   memcpy(&system->c3850.scratchpad[12], &system->c3850.scratchpad[stack], 2);
   system->c3850.scratchpad[59] -= 2;

   system->pc0 = system->pc1 - 1;
}

/* 0679 - 0766 */
/*
 * R0 11000000 = color
 * R0 00111111 = character id
 * R1          = x position
 * R2          = y position
*/
HLE(drawchar_custom)
{
   u8 *character = &system->rom[(system->dc0 + system->c3850.scratchpad[0]) & 0x3F];
   u8 color      = (system->c3850.scratchpad[0] & 0xC0) >> 6;
   u8 x          = system->c3850.scratchpad[1];
   u8 y          = system->c3850.scratchpad[2];
   u8 i;

   for (i = 0; i < 5 * 8; i++)
   {
      u8 on = ((*character >> ((i % 5) + 3)) & 1);

      vram_write(
         system->vram,
         x + (i % 5),
         y + i / 8,
         on ? color : 0
      );
      if (i % 5 == 0)
         character++;
   }
   system->pc0 = system->pc1 - 1;
}

HLE(drawchar)
{
   system->dc0 = 0x0767;
   drawchar_custom(system);
}

void* hle_get_func_from_addr(u16 address)
{
   switch (address)
   {
   /*case 0x0000:
      return reset;
   case 0x0001:
      return init;
   case 0x001A:
      return no_cart_init;
   case 0x00D0:
      return clear_screen;
   case 0x0107:
      return push_k;
   case 0x011E:
      return pop_k;
   case 0x0679:
      return drawchar;
   case 0x067C:
      return drawchar_custom;*/
   }

   return NULL;
}

#endif
