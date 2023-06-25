#include "vram.h"
#include "../screen.h"

#include <stdlib.h>

static const char *name = "VRAM";
static const int type = F8_DEVICE_MK4027;

#define RAM_WRT (1 << 5)

F8D_OP_OUT(mk4027_write)
{
  vram_t *m_vram = (vram_t*)device->device;

  /* Write to VRAM if RAM_WRT is pulsed */
  if (!(io_data->u & RAM_WRT) && (value.u & RAM_WRT))
  {
    u8 x, y, c;

    /* The inverse of the lowest 7 bits is the pixel X */
    x = (m_vram->io_x->u ^ 0xFF) & B01111111;

    /* The inverse of the lowest 6 bits is the pixel Y */
    y = (m_vram->io_y->u ^ 0xFF) & B00111111;

    /* The highest 2 bits is the pixel color */
    c = (m_vram->io_color->u & B11000000) >> 6;

    vram_write(m_vram->data, x, y, c);
  }

  *io_data = value;
}

void vram_init(f8_device_t *device)
{
  if (device)
  {
    device->device = (vram_t*)malloc(sizeof(vram_t));
    device->name = name;
    device->type = type;
    device->flags = F8_NO_ROMC;
  }
}
