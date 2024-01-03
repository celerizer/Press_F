#include <string.h>

#include "3851.h"

static const char *name = "Fairchild 3851 (Program Storage Unit)";
static const int type = F8_DEVICE_3851;

void f3851_init(f8_device_t *device)
{
  if (device)
  {
    f8_generic_init(device, 1024);
    device->name = name;
    device->type = type;
  }
}
