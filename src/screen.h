#ifndef PRESS_F_SCREEN_H
#define PRESS_F_SCREEN_H

#include "types.h"

void draw_frame_argb8888(u8 *vram, u32 *buffer);
void draw_frame_rgb565(u8 *vram, u16 *buffer);

#endif
