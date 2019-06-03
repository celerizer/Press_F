#ifndef PRESS_F_INPUT_H
#define PRESS_F_INPUT_H

#include "types.h"

/* Main console input */
#define INPUT_TIME       (1 << 0)
#define INPUT_MODE       (1 << 1)
#define INPUT_HOLD       (1 << 2)
#define INPUT_START      (1 << 3)

/* Hand-controller input */
#define INPUT_RIGHT      (1 << 0)
#define INPUT_LEFT       (1 << 1)
#define INPUT_BACK       (1 << 2)
#define INPUT_FORWARD    (1 << 3)
#define INPUT_ROTATE_CCW (1 << 4)
#define INPUT_ROTATE_CW  (1 << 5)
#define INPUT_PULL       (1 << 6)
#define INPUT_PUSH       (1 << 7)

/* 
   This code keeps input in the way ports 1 and 4 want it so it's 
   ready when asked for via the CPU IN/INS opcodes. We do this so we 
   don't interfere with video output, which these ports also handle.

   When an input is pressed its bit is OFF, otherwise it is ON.
*/

u8 get_input(u8 port);
void set_input_button(u8 port, u8 button, u8 enabled);

#endif
