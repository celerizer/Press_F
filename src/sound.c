#ifndef PRESS_F_SOUND_C
#define PRESS_F_SOUND_C

#include <math.h>
#include <string.h>
#include <stdio.h>

#define PF_AMPLITUDE 3000.0
#define PF_FREQUENCY 44100
#define PF_FPS       60
#define PF_SAMPLES   PF_FREQUENCY / PF_FPS
#define PF_PERIOD    1.0 / PF_FREQUENCY
#define PF_PI        3.14159265358979323846264338327950288

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

static i16 frequencies[735];
i16 samples[1470];
u32 samples_this_frame;

static u32 test;

void sound_empty()
{
   //memset(samples, 0, sizeof(samples));
   //memset(frequencies, 0, sizeof(frequencies));
   frequency_pushes = 0;
   frequency_this_tick = 0;
   last_tick = 0;
}

void sound_write()
{
   u16 i;

   samples_this_frame = PF_SAMPLES;

   if (last_tick != PF_SAMPLES - 1)
   {
      for (i = last_tick; i < PF_SAMPLES; i++)
         frequencies[i] = frequency_last;
   }

   for (i = 0; i < samples_this_frame; i++, test++)
   {
      /* Basic
      samples[2 * i]     = frequencies[i];
      samples[2 * i + 1] = frequencies[i]; */

      /* Sine */
      samples[2 * i]     = PF_AMPLITUDE * cos(2 * PF_PI * frequencies[i] * test * PF_PERIOD);
      samples[2 * i + 1] = PF_AMPLITUDE * cos(2 * PF_PI * frequencies[i] * test * PF_PERIOD);

      /* Sawtooth
      samples[2 * i]     = ((-2.0 * PF_AMPLITUDE)/PRESS_F_PI) * frequencies[i] * atan(1.0 / tan((test * PRESS_F_PI) / PF_PERIOD));
      samples[2 * i + 1] = ((-2.0 * PF_AMPLITUDE)/PRESS_F_PI) * frequencies[i] * atan(1.0 / tan((test * PRESS_F_PI) / PF_PERIOD)); */
   }
}

void sound_push_back(u8 frequency, u32 current_cycles, u32 total_cycles)
{
   u16 current_tick = PF_SAMPLES * ((float)current_cycles / (float)total_cycles);

   if (last_tick == 0)
   {
      u16 i;

      for (i = 0; i <= current_tick; i++)
         frequencies[i] = frequency_last;
   }
   else if (current_tick != last_tick)
   {
      u16 i;

      frequency_this_tick = ((float)frequency_this_tick / frequency_pushes);
      for (i = last_tick; i <= current_tick; i++)
         frequencies[i] = frequency_this_tick;
      frequency_this_tick = 0;
      frequency_pushes = 0;
   }
   frequency_this_tick += SOUND_FREQUENCIES[frequency];
   frequency_pushes++;
   last_tick = current_tick;
   frequency_last = SOUND_FREQUENCIES[frequency];
}

#endif
