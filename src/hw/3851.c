#include <stdlib.h>

#include "3851.h"

static const char *name = "Fairchild 3851 (Program Storage Unit)";
static const int type = F8_DEVICE_3851;

void f8_generic_serialize(const f8_device_t *device, void *buffer, unsigned *size)
{
  if (device && device->data && device->length)
  {
  //  memcpy(buffer + *size, device->data, device->length);
    *size += device->length;
  }
}

void f8_generic_unserialize(f8_device_t *device, const void *buffer, unsigned *size)
{
  if (device && device->data && device->length)
  {
  //  memcpy(device->data, buffer + *size, device->length);
    *size += device->length;
  }
}

void f8_generic_init(f8_device_t *device, unsigned size)
{
  if (device)
  {
    device->data = malloc(size);
    device->length = size;
    device->serialize = f8_generic_serialize;
    device->unserialize = f8_generic_unserialize;
  }
}

void f3851_init(f8_device_t *device)
{
  if (device)
  {
    f8_generic_init(device, 1024);
    device->name = name;
    device->type = type;
  }
}
