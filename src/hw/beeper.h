#ifndef PRESS_F_BEEPER_H
#define PRESS_F_BEEPER_H

#include "f8_device.h"

F8D_OP_OUT(beeper_out);

void beeper_init(f8_device_t *device);

#endif
