#ifndef PRESS_F_INPUT_H
#define PRESS_F_INPUT_H

#include "types.h"

/* Channel F console input */
#define INPUT_TIME  (1 << 0)
#define INPUT_MODE  (1 << 1)
#define INPUT_HOLD  (1 << 2)
#define INPUT_START (1 << 3)

/* Channel F hand-controller input */
#define INPUT_RIGHT      (1 << 0)
#define INPUT_LEFT       (1 << 1)
#define INPUT_BACK       (1 << 2)
#define INPUT_FORWARD    (1 << 3)
#define INPUT_ROTATE_CCW (1 << 4)
#define INPUT_ROTATE_CW  (1 << 5)
#define INPUT_PULL       (1 << 6)
#define INPUT_PUSH       (1 << 7)

/* FAIR-BUG parallel input receiver */
/* KD-BUG User's Guide (1976) - Appendix G */
enum f8_tty_char
{
  F8_TTY_LINE_FEED = 0x0A,
  F8_TTY_CARRIAGE_RTN = 0x0D,

  F8_TTY_SPACE = 0x20,
  F8_TTY_EXCLAMATION_POINT,
  F8_TTY_QUOTE,
  F8_TTY_POUND,
  F8_TTY_DOLLAR,
  F8_TTY_PERCENT,
  F8_TTY_AMPERSAND,
  F8_TTY_APOSTROPHE,
  F8_TTY_PARANTHESIS_L,
  F8_TTY_PARANTHESIS_R,
  F8_TTY_ASTERISK,
  F8_TTY_PLUS,
  F8_TTY_COMMA,
  F8_TTY_HYPHEN,
  F8_TTY_PERIOD,
  F8_TTY_SLASH,

  F8_TTY_0 = 0x30,
  F8_TTY_1,
  F8_TTY_2,
  F8_TTY_3,
  F8_TTY_4,
  F8_TTY_5,
  F8_TTY_6,
  F8_TTY_7,
  F8_TTY_8,
  F8_TTY_9,
  F8_TTY_SEMICOLON,
  F8_TTY_COLON,
  F8_TTY_GREATER_THAN,
  F8_TTY_EQUALS,
  F8_TTY_LESSER_THAN,
  F8_TTY_QUESTION_MARK,

  F8_TTY_AT_SIGN = 0x40,
  F8_TTY_A,
  F8_TTY_B,
  F8_TTY_C,
  F8_TTY_D,
  F8_TTY_E,
  F8_TTY_F,
  F8_TTY_G,
  F8_TTY_H,
  F8_TTY_I,
  F8_TTY_J,
  F8_TTY_K,
  F8_TTY_L,
  F8_TTY_M,
  F8_TTY_N,
  F8_TTY_O,
  F8_TTY_P,
  F8_TTY_Q,
  F8_TTY_R,
  F8_TTY_S,
  F8_TTY_T,
  F8_TTY_U,
  F8_TTY_V,
  F8_TTY_W,
  F8_TTY_X,
  F8_TTY_Y,
  F8_TTY_Z,
  F8_TTY_BRACKET_L,
  F8_TTY_BACKSLASH,
  F8_TTY_BRACKET_R,
  F8_TTY_UP,
  F8_TTY_LEFT,

  F8_TTY_NULL_A = 0x7F,
  F8_TTY_BELL = 0x87,
  F8_TTY_READER_ON = 0x91,
  F8_TTY_PUNCH_ON = 0x92,
  F8_TTY_READER_OFF = 0x93,
  F8_TTY_PUNCH_OFF = 0x94,
  F8_TTY_NULL_B = 0xFF
};

/* 
   This code keeps input in the way ports 1 and 4 want it so it's 
   ready when asked for via the CPU IN/INS opcodes. We do this so we 
   don't interfere with video output, which these ports also handle.

   When an input is pressed its bit is OFF, otherwise it is ON.
*/

u8 get_input(u8 port);
u8 get_keyboard_char();
void set_input_button(u8 port, u8 button, u8 enabled);

#endif
