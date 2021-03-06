#ifndef PRESS_F_2102_H
#define PRESS_F_2102_H

#include "f8_device.h"
#include "../types.h"

#define F2102_SIZE       0x80
#define F2102_FUNCTION_A 0
#define F2102_FUNCTION_B 1

typedef struct f2102_t
{
   u16 selected_address;
   u8  chip_select;
} f2102_t;

void f2012_function_a(f8_device_t *device, u8 *port_data);
void f2012_function_b(f8_device_t *device, u8 *port_data);

void f2102_init(f8_device_t *device);

#endif
