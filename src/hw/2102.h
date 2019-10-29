#ifndef PRESS_F_2102_H
#define PRESS_F_2102_H

#include "../types.h"

#define F2102_SIZE       0x80
#define F2102_FUNCTION_A 0
#define F2102_FUNCTION_B 1

typedef struct f2102_t
{
   u8  data[F2102_SIZE];
   u16 selected_address;
   u8  chip_select;
} f2102_t;

void f2012_function_a(u8 *port_data);
void f2012_function_b(u8 *port_data);

void f2102_init(f2102_t *device);
void f2102_free(f2102_t *device);

#endif
