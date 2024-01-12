#ifndef PRESS_F_2102_H
#define PRESS_F_2102_H

#include "f8_device.h"

#define F2102_SIZE (1024 / 8)

typedef struct f2102_t
{
  f8_byte data[F2102_SIZE];
  u16 address;
} f2102_t;

F8D_OP_OUT(f2102_out_address);
F8D_OP_OUT(f2102_out_write);

void f2102_init(f8_device_t *device);

#endif
