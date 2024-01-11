#ifndef PRESS_F_SOUND_H
#define PRESS_F_SOUND_H

#include "config.h"
#include "types.h"

#define PF_DECAY   (1.0f - 77.0f / PF_FREQUENCY)
#define PF_PERIOD  1.0f / PF_FREQUENCY
#define PF_SAMPLES PF_FREQUENCY / 60

#endif
