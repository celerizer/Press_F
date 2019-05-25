#ifndef PRESSF_3850_H
#define PRESSF_3850_H

#include "../types.h"

#define SCRATCH_SIZE 0x40

#define STATUS_SIGN       (1 << 0)
#define STATUS_CARRY      (1 << 1)
#define STATUS_ZERO       (1 << 2)
#define STATUS_OVERFLOW   (1 << 3)
#define STATUS_INTERRUPTS (1 << 4)

typedef struct c3850_t
{
   /* 8 bit */
   u8 accumulator;

   /* 6 bit */
   u8 isar;

   /* 8 bit */
   u8 scratchpad[SCRATCH_SIZE];

   /* 5 bit */
   u8 status_register;
} c3850_t;

#endif