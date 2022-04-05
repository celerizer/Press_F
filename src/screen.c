#ifndef PRESS_F_SCREEN_C
#define PRESS_F_SCREEN_C

#include <string.h>

#include "screen.h"

static u8 screen_dirty_any;
static u8 screen_dirty[64];

/* The palette index is inversed to save a calculation in GET_PALETTE */
static const u16 PIXEL_COLOR_RGB565[4][4] =
{
   /* Green background */
   {0x066B, 0xF98A, 0x49FE, 0x97F4},
   /* Blue background */
   {0x066B, 0xF98A, 0x49FE, 0xE71C},
   /* Grey background */
   {0x066B, 0xF98A, 0x49FE, 0xCE9F},
   /* Black and white */
   {0xFFFF, 0xFFFF, 0xFFFF, 0x0000}
};

/* Returns the two-bit pixel data for a given pixel index */
#define GET_PIXEL(a, b) ((a[(b) / 4] >> (3 - ((b) & 3)) * 2) & 3)

/* Returns the 2-bit palette index for a scanline (defined by bit 1 of columns 125 and 126) */
#define GET_PALETTE(a, b) ((GET_PIXEL(a, ((b) & 0xFF80) + 125) & 2) >> 1) + (GET_PIXEL(a, ((b) & 0xFF80) + 126) & 2)

u8 draw_frame_rgb565_full(u8 *vram, u16 *buffer)
{
   if (!screen_dirty_any)
      return FALSE;
   else
   {
      u8 palette, pixel_data, x_pos, y_pos;
      u16 buffer_pos = 0;

      for (y_pos = 0; y_pos < VRAM_HEIGHT; y_pos++)
      {
         /* Don't waste time painting lines that haven't changed this frame */
         if (!screen_dirty[y_pos])
         {
            buffer_pos += VRAM_WIDTH;
            continue;
         }

         palette = GET_PALETTE(vram, buffer_pos);

         for (x_pos = 0; x_pos < VRAM_WIDTH; x_pos++, buffer_pos++)
         {
            pixel_data = GET_PIXEL(vram, buffer_pos);
            buffer[buffer_pos] = PIXEL_COLOR_RGB565[palette][pixel_data];
         }

         screen_dirty[y_pos] = FALSE;
      }

      screen_dirty_any = FALSE;
   }

   return TRUE;
}

u8 draw_frame_rgb565(u8 *vram, u16 *buffer)
{
   if (!screen_dirty_any)
      return FALSE;
   else
   {
      u8 palette, pixel_data, x_pos, y_pos;
      u16 buffer_pos = 0;

      for (y_pos = 4; y_pos < 4 + SCREEN_HEIGHT; y_pos++)
      {
         /* Don't waste time painting lines that haven't changed this frame */
         if (!screen_dirty[y_pos])
         {
            buffer_pos += SCREEN_WIDTH;
            continue;
         }

         palette = GET_PALETTE(vram, y_pos * VRAM_WIDTH);

         for (x_pos = 4; x_pos < 4 + SCREEN_WIDTH; x_pos++, buffer_pos++)
         {
            pixel_data = GET_PIXEL(vram, y_pos * VRAM_WIDTH + x_pos);
            buffer[buffer_pos] = PIXEL_COLOR_RGB565[palette][pixel_data];
         }

         screen_dirty[y_pos] = FALSE;
      }

      screen_dirty_any = FALSE;
   }

   return TRUE;
}

#ifdef ULTRA64
u8 draw_frame_ultra(u8 *vram, u8 *buffer_a, u8 *buffer_b)
{
   if (!screen_dirty_any)
      return FALSE;
   else
   {
      u8 palette, pixel_data, x_pos, y_pos;
      u16 buffer_pos = 0;

      for (y_pos = 0; y_pos < VRAM_HEIGHT; y_pos++)
      {
         /* Don't waste time painting lines that haven't changed this frame */
         if (!screen_dirty[y_pos])
         {
            buffer_pos += VRAM_WIDTH / 4;
            continue;
         }

         palette = GET_PALETTE(vram, y_pos * VRAM_WIDTH) * 4;

         for (x_pos = 0; x_pos < VRAM_WIDTH / 2; x_pos += 2, buffer_pos++)
         {
            pixel_data = GET_PIXEL(vram, y_pos * VRAM_WIDTH + x_pos);
            buffer_a[buffer_pos] = ((palette + pixel_data) << 4) & 0xF0;
            
            pixel_data = GET_PIXEL(vram, y_pos * VRAM_WIDTH + x_pos + 1);
            buffer_a[buffer_pos] += palette + pixel_data;

            pixel_data = GET_PIXEL(vram, y_pos * VRAM_WIDTH + x_pos + 64);
            buffer_b[buffer_pos] = ((palette + pixel_data) << 4) & 0xF0;
            
            pixel_data = GET_PIXEL(vram, y_pos * VRAM_WIDTH + x_pos + 65);
            buffer_b[buffer_pos] += palette + pixel_data;
         }

         screen_dirty[y_pos] = FALSE;
      }

      screen_dirty_any = FALSE;
   }

   return TRUE;
}
#endif

/* Force the emulator to redraw every pixel next frame, 
   helpful for state loads */
void force_draw_frame()
{
   memset(screen_dirty, TRUE, sizeof(screen_dirty));
   screen_dirty_any = TRUE;
}

/* Writes a 2-bit pixel into the correct position in VRAM 
   We assume "value" is also 2-bit. */
void vram_write(u8 *vram, u8 x, u8 y, u8 value)
{
   u16 byte = (x + y * 128)/4;
   u8 final;
   u8 mask;

   screen_dirty_any = TRUE;
   screen_dirty[y]  = TRUE;
   switch (x & 3)
   {
   case 0:
      mask = 0x3F;
      break;
   case 1:
      mask = 0xCF;
      break;
   case 2:
      mask = 0xF3;
      break;
   case 3:
      mask = 0xFC;
      break;
   /* Shouldn't ever happen, but here to silence a warning. */
   default:
      return;
   }
   final = (u8)(value << ((3 - (x & 3)) * 2));

   vram[byte] &= mask;
   vram[byte] |= final;
}

#endif
