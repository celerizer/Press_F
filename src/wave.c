#ifndef PRESS_F_WAVE_C
#define PRESS_F_WAVE_C

#include "types.h"
#include "wave.h"

float pf_factorial(u8 x)
{
   float result = x;

   for (x--; x > 1; x--)
      result *= x;

   return result;
}

float pf_power(float x, u8 power)
{
   float result = x;

   for (; power > 1; power--)
      result *= x;

   return result;
}

float pf_wave(float x, u8 cosine)
{
   float next, result;
   u8 positive = TRUE;
   u8 i;

   while (x > 2 * PF_PI)
      x -= 2 * PF_PI;
   result = cosine ? 1 : x;
   for (i = cosine ? 2 : 3; i <= PF_TERMS; i += 2)
   {
      positive ^= TRUE;
      next = pf_power(x, i) / pf_factorial(i);
      result = positive ? result + next : result - next;
   }

   return result;
}

#endif
