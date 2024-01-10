#include "beeper.h"
#include "../sound.h"

static const char *name = "Beeper";
static const int type = F8_DEVICE_BEEPER;

F8D_OP_OUT(beeper_out)
{
  F8_UNUSED(device);
  sound_push_back(value.u >> 6, 50, 100);
  *io_data = value;
}

void beeper_init(f8_device_t *device)
{
  if (device)
  {
    device->name = name;
    device->type = type;
    device->flags = F8_NO_ROMC;
  }
}
