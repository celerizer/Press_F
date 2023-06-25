#ifndef PRESS_F_3850_H
#define PRESS_F_3850_H

#include "f8_device.h"

#define SCRATCH_SIZE 64

#define STATUS_SIGN       (1 << 0)
#define STATUS_CARRY      (1 << 1)
#define STATUS_ZERO       (1 << 2)
#define STATUS_OVERFLOW   (1 << 3)
#define STATUS_INTERRUPTS (1 << 4)

typedef struct f3850_t
{
  /* 8 bit */
  f8_byte accumulator;

  /* 6 bit */
  u8 isar;

  /* 8 bit */
  f8_byte scratchpad[SCRATCH_SIZE];

  /* 5 bit */
  u8 status_register;
} f3850_t;

void f3850_init(f8_device_t *device);

#endif
