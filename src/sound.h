#ifndef PRESS_F_SOUND_H
#define PRESS_F_SOUND_H

#include "types.h"

extern i16 samples[1470];
extern u32 samples_this_frame;

void sound_empty();
void sound_push_back(u8 frequency, u32 current_cycles, u32 total_cycles);
void sound_write();

#endif
