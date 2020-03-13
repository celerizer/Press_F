#ifndef PRESS_F_WAVE_H
#define PRESS_F_WAVE_H

#include "types.h"

#define PF_PI 3.141592653589793f
/* Largest Taylor series factorial a float can hold */
#define PF_TERMS 34

float pf_wave(float x, u8 cosine);

#endif
