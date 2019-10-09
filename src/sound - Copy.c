#ifndef PRESS_F_SOUND_C
#define PRESS_F_SOUND_C

#include <math.h>
#include <string.h>
#include <stdio.h>

#include "sound.h"

const i16 SOUND_FREQUENCIES[4] = 
{
   0,
   1000,
   500,
   120
};

static i16 frequency_last;
static u8  frequency_pushes;
static u32 frequency_this_tick;
static u16 last_tick;
static u16 steps_with_same;

i16 samples[1470];
u32 samples_this_frame;

void sound_empty()
{
   memset(samples, 0, sizeof(samples));
   frequency_pushes = 0;
   frequency_this_tick = 0;
   last_tick = 0;
   steps_with_same = 0;
}

void sound_write()
{
   samples_this_frame = 735;
   u16 i, j;
   i16 step;

   /*for (j = last_tick; j < 735; j++)
   {
      step = 5000 * sin(2 * 3.14159265 * frequency_last * last_tick * 0.00002267573);
      samples[2 * j] = step;
      samples[2 * j + 1] = step;
   }*/
}

void sound_push_back(u8 frequency, u32 current_cycles, u32 total_cycles)
{
   u16 current_tick = 736 * ((float)current_cycles / (float)total_cycles);

   frequency_this_tick += SOUND_FREQUENCIES[frequency];
   frequency_pushes++;

   if (current_tick != last_tick)
   {
      u16 i, j;

      frequency_this_tick = ((float)frequency_this_tick / frequency_pushes);
      for (i = 0, j = last_tick; j < current_tick; i++, j++)
      {
         samples[2 * j] = frequency_this_tick;
         samples[2 * j + 1] = frequency_this_tick;
      }
      frequency_this_tick = 0;
      frequency_pushes = 0;
   }
   /*if (current_tick != last_tick)
   {
      u16 i, j;
      i16 step;

      for (i = 0, j = last_tick; j < current_tick; i++, j++)
      {
         step = 5000 * sin(2 * 3.14159265 * SOUND_FREQUENCIES[frequency] * i * 0.00002267573);
         samples[2 * j] = step;
         samples[2 * j + 1] = step;
      }
   }*/

   last_tick = current_tick;
   frequency_last = SOUND_FREQUENCIES[frequency];
}

#endif
