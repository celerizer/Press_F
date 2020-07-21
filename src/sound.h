#ifndef PRESS_F_SOUND_H
#define PRESS_F_SOUND_H

#include "types.h"

#define PF_FPS           60
#define PF_FREQUENCY     44100
#define PF_MAX_AMPLITUDE 0x7FFF
#define PF_MIN_AMPLITUDE 0x0000
#define PF_PERIOD        1.0 / PF_FREQUENCY
#define PF_SAMPLES       PF_FREQUENCY / PF_FPS

/* From MAME, equal to exp((-0.693/9e-3) / 44100) */
#define PF_DECAY         (1.0 - 77.0 / PF_FREQUENCY)

void sound_empty();
void sound_push_back(u8 frequency, u32 current_cycles, u32 total_cycles);
void sound_write();

extern i16 samples[PF_SAMPLES * 2];

#endif
