#ifndef PRESS_F_WAVE_H
#define PRESS_F_WAVE_H

#include "types.h"

#define PF_PI 3.141592653589793115997963468544185161590576171875

/* Largest Taylor series factorial a float can hold */
#define PF_TERMS 32

double pf_wave(double x, u8 cosine);

#endif
