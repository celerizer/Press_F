#include <stdlib.h>

#include "../wave.h"

#include "beeper.h"

static const char *name = "Beeper";
static const int type = F8_DEVICE_BEEPER;

static const i16 SOUND_FREQUENCIES[4] =
{
  0,
  1000,
  500,
  120
};

static void sound_push_back(f8_beeper_t *beeper, unsigned frequency)
{
  unsigned current_tick = PF_SOUND_SAMPLES * ((double)beeper->current_cycles /
                                        (double)beeper->total_cycles);

  if (current_tick != beeper->last_tick)
  {
    unsigned i;

    for (i = beeper->last_tick; i < current_tick; i++)
      beeper->frequencies[i] = beeper->frequency_last;
  }
  beeper->last_tick = current_tick;
  beeper->frequency_last = SOUND_FREQUENCIES[frequency];
}

F8D_OP_OUT(beeper_out)
{
  sound_push_back(device->device, value.u >> 6);
  *io_data = value;
}

void beeper_set_timing(f8_device_t *device, int current, int total)
{
  ((f8_beeper_t*)device->device)->current_cycles = current;
  ((f8_beeper_t*)device->device)->total_cycles = total;
}

void beeper_finish_frame(f8_device_t *device)
{
#if PF_AUDIO_ENABLE
  f8_beeper_t* m_beeper = ((f8_beeper_t*)device->device);
  unsigned i;

  /* Fill any unwritten samples with the last known tone */
  if (m_beeper->last_tick != PF_SOUND_SAMPLES - 1)
  {
    for (i = m_beeper->last_tick; i < PF_SOUND_SAMPLES; i++)
      m_beeper->frequencies[i] = m_beeper->frequency_last;
  }

  for (i = 0; i < PF_SOUND_SAMPLES; i++, m_beeper->time++)
  {
    if (m_beeper->frequencies[i] == 0)
    {
      /* Sound was turned off, reset the amplitude so our next sound pops */
      m_beeper->time = 0;
      m_beeper->amplitude = PF_MAX_AMPLITUDE;
      m_beeper->samples[2 * i] = 0;
      m_beeper->samples[2 * i + 1] = 0;
    }
    else
    {
      /* Use sine wave to tell if our square wave is on or off */
      double sine = pf_wave((2 * PF_PI * m_beeper->frequencies[i] *
                             m_beeper->time * PF_SOUND_PERIOD), FALSE);
      int mult = sine > 0 ? 1 : 0;

      m_beeper->samples[2 * i] = m_beeper->amplitude * mult;
      m_beeper->samples[2 * i + 1] = m_beeper->amplitude * mult;
      m_beeper->amplitude *= PF_SOUND_DECAY;
    }
  }
  m_beeper->last_tick = 0;
#endif
}

void beeper_init(f8_device_t *device)
{
  if (device)
  {
    device->device = (f8_beeper_t*)calloc(1, sizeof(f8_beeper_t));
    device->name = name;
    device->type = type;
    device->flags = F8_NO_ROMC;
    device->set_timing = beeper_set_timing;
    device->finish_frame = beeper_finish_frame;
  }
}
