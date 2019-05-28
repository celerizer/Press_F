#ifndef PRESSF_SCREEN_C
#define PRESSF_SCREEN_C

#include "screen.h"

const u16 PIXEL_COLOR_RGB565[4] = 
{
   0x066B,
   0xF98A,
   0x49FE,
   0xCE9F
};

void draw_frame_argb8888(u8 *vram, u32 *buffer)
{
}

u16 get_pixel_rgb565(u8 byte, u8 index)
{
   return PIXEL_COLOR_RGB565[(byte >> (3 - index) * 2) & 3];
}

void draw_frame_rgb565(u8 *vram, u16 *buffer)
{
   u16 i, j;

   for (i = 0, j = 0; i < 0x800; i++, j += 4)
   {
      buffer[j]     = get_pixel_rgb565(vram[i], 0);
      buffer[j + 1] = get_pixel_rgb565(vram[i], 1);
      buffer[j + 2] = get_pixel_rgb565(vram[i], 2);
      buffer[j + 3] = get_pixel_rgb565(vram[i], 3);
   }
}

#endif
