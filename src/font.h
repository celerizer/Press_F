#ifndef PRESS_F_FONT_H
#define PRESS_F_FONT_H

#include "hw/channelf.h"

#define FONT_FAIRCHILD 0
#define FONT_CUTE      1
#define FONT_SKINNY    2

void font_load(channelf_t *system, const u8 font);
void font_reset(channelf_t *system);

#endif
