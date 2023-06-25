#ifndef PRESS_F_FONT_H
#define PRESS_F_FONT_H

#include "hw/system.h"

enum
{
  FONT_FAIRCHILD = 0,
  FONT_CUTE,
  FONT_SKINNY,

  FONT_SIZE
};

void font_load(f8_system_t *system, const u8 font);
void font_reset(f8_system_t *system);

#endif
