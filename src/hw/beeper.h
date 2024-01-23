#ifndef PRESS_F_BEEPER_H
#define PRESS_F_BEEPER_H

#include "../config.h"
#include "f8_device.h"

typedef struct
{
  int current_cycles;
  int total_cycles;
  int frequency_last;
  unsigned last_tick;
  double amplitude;
  unsigned time;
  short frequencies[PF_SOUND_SAMPLES];
  i16 samples[PF_SOUND_SAMPLES * 2];
} f8_beeper_t;

F8D_OP_OUT(beeper_out);

void beeper_init(f8_device_t *device);

#endif
