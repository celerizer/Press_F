#ifndef PRESS_F_KDBUG_H
#define PRESS_F_KDBUG_H

#include "f8_device.h"

/* Port 0 pinouts (control codes) */
/* --- */

#define F8_KDBUG_DISPLAY_DIGIT_A (1 << 0)
#define F8_KDBUG_DISPLAY_DIGIT_B (1 << 1)
#define F8_KDBUG_DISPLAY_DIGIT_C (1 << 2)
#define F8_KDBUG_DISPLAY_DIGIT_D (1 << 3)
#define F8_KDBUG_DISPLAY_DIGIT_E (1 << 4)
#define F8_KDBUG_DISPLAY_DIGIT_F (1 << 5)
#define F8_KDBUG_DISPLAY_DIGITS B00111111

/* If this bit is on, the unit is in store mode and the decimal LED is lit. */
#define F8_KDBUG_DECIMAL (1 << 6)

/* If this bit is on, the KD-BUG display should be blanked? */
#define F8_KDBUG_BLANKING (1 << 7)
/* --- */

/* Port 1 pinouts (data) */
/* --- */

#define F8_KDBUG_DISPLAY_OUT B00001111
#define F8_KDBUG_KEYBOARD_IN B11110000
/* --- */

/* TODO: How is this read back in with only 4 bits??? */
enum f8_kb
{
  F8_KB_0 = 0,
  F8_KB_1,
  F8_KB_2,
  F8_KB_3,
  F8_KB_4,
  F8_KB_5,
  F8_KB_6,
  F8_KB_7,
  F8_KB_8,
  F8_KB_9,
  F8_KB_A,
  F8_KB_B,
  F8_KB_C,
  F8_KB_D,
  F8_KB_E,
  F8_KB_F,
  F8_KB_DECREMENT_ADDRESS,
  F8_KB_ENTER,
  F8_KB_INCREMENT_ADDRESS,
  F8_KB_MOVE_UP,
  F8_KB_MOVE_DOWN,
  F8_KB_STORE_MODE,
  F8_KB_GO,
  F8_KB_CLEAR,

  /* The number of keys available on the keyboard unit. Not a key ID. */
  F8_KB_SIZE,
  F8_KB_NONE = 0xFF
};

typedef struct kdbug_t
{
  u8 *io_control;
  u8 *io_data;
  u8  display_digits[6];
} kdbug_t;

F8D_OP_OUT(kdbug_control);
F8D_OP_OUT(kdbug_data);

#endif
