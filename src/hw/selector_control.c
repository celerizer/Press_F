#include "../input.h"
#include "selector_control.h"

static const char *name = "Selector Control buttons";
static const unsigned type = F8_DEVICE_SELECTOR_CONTROL;

F8D_OP_IN(selector_control_input)
{
  F8_UNUSED(device);
  io_data->u = get_input(0);
}

void selector_control_init(f8_device_t *device)
{
  if (device)
  {
    device->name = name;
    device->type = type;
    device->flags = F8_NO_ROMC;
  }
}
