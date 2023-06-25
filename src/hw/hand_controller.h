#ifndef PRESS_F_HAND_CONTROLLER_H
#define PRESS_F_HAND_CONTROLLER_H

#include "../types.h"
#include "f8_device.h"

typedef struct hand_controller_t
{
  /**
   * The "player index" of the hand controller. This is used to map port
   * hookups for casual emulator play.
   * On a Channel F:
   * - The hand controller in port 4 would be 0; player 1.
   * - The hand controller in port 1 would be 1; player 2.
   */
  u8 index;
} hand_controller_t;

F8D_OP_IN(hand_controller_input);

void hand_controller_init(f8_device_t *device);

#endif
