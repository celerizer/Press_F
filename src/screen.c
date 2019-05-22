#ifndef PRESSF_SCREEN_C
#define PRESSF_SCREEN_C

#include "screen.h"

#define BKG_BLACK 0x0000
#define BKG_GREY  0xDEDB
#define BKG_BLUE  0x841F
#define BKG_GREEN 0x67EC

void draw_frame_argb8888(u8 *vram, u32 *buffer)
{
}

u16 get_pixel_rgb565(u8 byte, u8 index)
{
   switch ((byte >> index * 2) & 3)
   {
   case 0:
      return BKG_BLACK;
   case 1:
      return BKG_GREY;
   case 2:
      return BKG_BLUE;
   case 3:
      return BKG_GREEN;
   }

   return BKG_BLACK;
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
