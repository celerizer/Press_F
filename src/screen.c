#ifndef PRESSF_SCREEN_C
#define PRESSF_SCREEN_C

#include "screen.h"

const u16 PIXEL_COLOR_RGB565[4][4] = 
{
   {0x066B, 0xF98A, 0x49FE, 0xCE9F},
   {0x066B, 0xF98A, 0x49FE, 0xE71C},
   {0x066B, 0xF98A, 0x49FE, 0x97F4},
   {0xFFFF, 0xFFFF, 0xFFFF, 0x0000}
};

void draw_frame_argb8888(u8 *vram, u32 *buffer)
{
}

/* Returns the two-bit pixel data for a given pixel index */
u8 get_pixel(u8 *vram, u16 index)
{
   return (vram[index / 4] >> (3 - (index & 3)) * 2) & 3;
}

u16 get_pixel_rgb565(u8 byte, u8 index, u8 palette)
{
   u8 pixel_data = (byte >> (3 - index) * 2) & 3;

   return PIXEL_COLOR_RGB565[palette][pixel_data];
}

u8 get_palette(u8 *vram, u16 index)
{
   u8 row = index / 128;

   return get_pixel(vram, (row * 128) + 126);
}

void draw_frame_rgb565(u8 *vram, u16 *buffer)
{
   u8 palette;
   u16 i, j;

   for (i = 0, j = 0; i < 0x800; i++, j += 4)
   {
      /* Every aligned 4 pixels should use the same palette */
      palette = get_palette(vram, j);
      buffer[j]     = get_pixel_rgb565(vram[i], 0, palette);
      buffer[j + 1] = get_pixel_rgb565(vram[i], 1, palette);
      buffer[j + 2] = get_pixel_rgb565(vram[i], 2, palette);
      buffer[j + 3] = get_pixel_rgb565(vram[i], 3, palette);
   }
}

#endif
