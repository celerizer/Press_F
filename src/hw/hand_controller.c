#include "../input.h"
#include "hand_controller.h"

static const char *name = "Hand-Controller";
static const int type = F8_DEVICE_HAND_CONTROLLER;

F8D_OP_IN(hand_controller_input)
{
  io_data->u = get_input(4);
}

void hand_controller_init(f8_device_t *device)
{
  if (device)
  {
    device->name = name;
    device->type = type;
    device->flags = F8_NO_ROMC;
  }
}
