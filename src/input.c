#ifndef PRESS_F_INPUT_C
#define PRESS_F_INPUT_C

#include "input.h"

u8 input[16];

u8 get_input(u8 port)
{
   return input[port];
}

void set_input_button(u8 port, u8 button, u8 enabled)
{
   input[port] = enabled ? input[port] & ~button : input[port] | button;
}

#endif
