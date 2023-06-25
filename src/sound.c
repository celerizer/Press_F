#include <string.h>

#include "sound.h"
#include "wave.h"

static const i16 SOUND_FREQUENCIES[4] =
{
   0,
   1000,
   500,
   120
};

static i16   frequency_last;
static u16   last_tick;
static float amplitude;
static u32   time;
static i16   frequencies[PF_SAMPLES];

i16 samples[PF_SAMPLES * 2];

void sound_empty()
{
   memset(samples,     0, sizeof(samples));
   memset(frequencies, 0, sizeof(frequencies));
}

void sound_write()
{
#ifdef PF_AUDIO_ENABLE
   u16 i;

   /* Fill any unwritten samples with the last known tone */
   if (last_tick != PF_SAMPLES - 1)
   {
      for (i = last_tick; i < PF_SAMPLES; i++)
         frequencies[i] = frequency_last;
   }

   for (i = 0; i < PF_SAMPLES; i++, time++)
   {
      if (frequencies[i] == 0)
      {
         /* Sound was turned off, reset the amplitude so our next sound pops */
         time = 0;
         amplitude = PF_MAX_AMPLITUDE;
         samples[2 * i]     = 0;
         samples[2 * i + 1] = 0;
      }
      else
      {
         /* Use sine wave to tell if our square wave is on or off */
         float sine = pf_wave((2 * PF_PI * frequencies[i] * time * PF_PERIOD), FALSE);
         i8 mult = sine > 0 ? 1 : 0;

         samples[2 * i]     = amplitude * mult;
         samples[2 * i + 1] = amplitude * mult;
         amplitude *= PF_DECAY;
      }
   }
   last_tick = 0;
#endif
}

void sound_push_back(u8 frequency, u32 current_cycles, u32 total_cycles)
{
#if PF_AUDIO_ENABLE
   unsigned current_tick = PF_SAMPLES * ((float)current_cycles / (float)total_cycles);

   if (current_tick != last_tick)
   {
      unsigned i;

      for (i = last_tick; i < current_tick; i++)
         frequencies[i] = frequency_last;
   }
   last_tick = current_tick;
   frequency_last = SOUND_FREQUENCIES[frequency];
#endif
}
