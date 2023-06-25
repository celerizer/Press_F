#ifndef PRESS_F_FAIRBUG_PARALLEL_H
#define PRESS_F_FAIRBUG_PARALLEL_H

#include "f8_device.h"

typedef struct fairbug_parallel_t
{
  u8 *io;
} fairbug_parallel_t;

F8D_OP_IN(fairbug_parallel_io);

void fairbug_parallel_init(f8_device_t *device);

#endif
