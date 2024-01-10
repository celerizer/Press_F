#include "2114.h"

static const char *name = "Fairchild 2114 (1024 x 4 Static RAM)";
static const int type = F8_DEVICE_2114;

void f2114_init(f8_device_t *device)
{
  if (device)
  {
    f8_generic_init(device, 1024 * 4 / 8);
    device->name = name;
    device->type = type;
    device->flags = F8_DATA_WRITABLE;
  }
}
