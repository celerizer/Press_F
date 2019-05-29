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

/* Returns the two-bit pixel data for a given pixel index */
u8 get_pixel(u8 *vram, u16 index)
{
   return (vram[index / 4] >> (3 - (index & 3)) * 2) & 3;
}

/* Returns the 2-bit palette index for a scanline (defined by column 126) */
u8 get_palette(u8 *vram, u16 index)
{
   return get_pixel(vram, (index & 0xFF80) + 126);
}

void draw_frame_rgb565(u8 *vram, u16 *buffer)
{
   u8 palette, pixel_data;
   u16 i, j;

   for (i = 0; i < 128 * 64;)
   {
      /* Every aligned 4 pixels should use the same palette */
      palette = get_palette(vram, i);

      for (j = 0; j < 4; i++, j++)
      {
         pixel_data = get_pixel(vram, i);
         buffer[i] = PIXEL_COLOR_RGB565[palette][pixel_data];
      }
   }
}

#endif
