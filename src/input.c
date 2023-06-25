#ifndef PRESS_F_INPUT_C
#define PRESS_F_INPUT_C

#include "input.h"

static u8 input[16] = { 0xFF };
static u8 keyboard_ready = FALSE;
static u8 keyboard = 0;

u8 get_input(u8 port)
{
  return input[port];
}

u8 get_keyboard_char()
{
  keyboard_ready = FALSE;
  return keyboard;
}

void set_input_button(u8 port, u8 button, u8 enabled)
{
  input[port] = enabled ? input[port] & ~button : input[port] | button;
}

#endif
