#ifndef PRESS_F_VRAM_H
#define PRESS_F_VRAM_H

#include "f8_device.h"

/* 2 bits * 128 width * 64 height */
#define VRAM_SIZE 0x0800

typedef struct vram_t
{
  u8 data[VRAM_SIZE];

  /**
   * IO port that signals to write a pixel to VRAM when pulsed.
   * Port 0 on the Channel F.
   */
  f8_byte *io_write;

  /**
   * IO port whose highest 2 bits determine the color to write.
   * Port 1 on the Channel F.
   */
  f8_byte *io_color;

  /**
   * IO port whose lowest 7 bits determine the X coordinate of the pixel.
   * Port 4 on the Channel F.
   */
  f8_byte *io_x;

  /**
   * IO port whose lowest 6 bits determine the Y coordinate of the pixel.
   * Port 5 on the Channel F.
   */
  f8_byte *io_y;
} vram_t;

F8D_OP_OUT(mk4027_write);

void vram_init(f8_device_t *device);

#endif
