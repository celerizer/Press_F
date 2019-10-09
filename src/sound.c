#ifndef PRESS_F_SOUND_C
#define PRESS_F_SOUND_C

#include <string.h>

#include "sound.h"
#include "wave.h"

const i16 SOUND_FREQUENCIES[4] = 
{
   0,
   1000,
   500,
   120
};

static i16   frequency_last;
static u8    frequency_pushes;
static u32   frequency_this_tick;
static u16   last_tick;
static float amplitude;
static u32   time;
static i16 frequencies[PF_SAMPLES];

i16 samples[PF_SAMPLES * 2];
u32 samples_this_frame;

void sound_empty()
{
   memset(samples,     0, sizeof(samples));
   memset(frequencies, 0, sizeof(frequencies));
}

void sound_write()
{
   u16 i;

   if (last_tick != PF_SAMPLES - 1)
   {
      for (i = last_tick; i < PF_SAMPLES; i++)
         frequencies[i] = frequency_last;
   }
   for (i = 0; i < PF_SAMPLES; i++, time++)
   {
      amplitude *= PF_DECAY;
      samples[2 * i]     = amplitude * pf_wave((2 * PF_PI * frequencies[i] * time * PF_PERIOD), FALSE);
      samples[2 * i + 1] = amplitude * pf_wave((2 * PF_PI * frequencies[i] * time * PF_PERIOD), FALSE);
      if (frequencies[i] == 0)
      {
         time = 0;
         amplitude = PF_MAX_AMPLITUDE;
      }
   }
   frequency_pushes = 0;
   frequency_this_tick = 0;
   last_tick = 0;
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

      frequency_this_tick = ((float)frequency_this_tick / (float)frequency_pushes);
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
