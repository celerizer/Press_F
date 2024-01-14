#include "types.h"
#include "wave.h"

double pf_factorial(u8 x)
{
  double result = x;

  for (x--; x > 1; x--)
    result *= x;

  return result;
}

double pf_power(double x, u8 power)
{
  double result = x;

  for (; power > 1; power--)
    result *= x;

  return result;
}

double pf_wave(double x, u8 cosine)
{
  double next, result;
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
