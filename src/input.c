#ifndef PRESS_F_INPUT_C
#define PRESS_F_INPUT_C

#include "input.h"

static u8 p1_controller;
static u8 p2_controller;

u8 get_input(u8 port)
{
   return port == 1 ? p2_controller : p1_controller;
}

void set_input_button(u8 port, u8 button, u8 enabled)
{
   if (port == 1)
      p2_controller = enabled ? p2_controller & ~button : p2_controller | button;
   else
   	  p1_controller = enabled ? p1_controller & ~button : p1_controller | button;
}

#endif
