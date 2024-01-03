#include <stdlib.h>

#include "f8_device.h"

void f8_generic_serialize(const f8_device_t *device, void *buffer, unsigned *size)
{
  /*
  if (device && device->data && device->length)
  {
    memcpy(buffer + *size, device->data, device->length);
    *size += device->length;
  }
  */
}

void f8_generic_unserialize(f8_device_t *device, const void *buffer, unsigned *size)
{
  /*
  if (device && device->data && device->length)
  {
    memcpy(device->data, buffer + *size, device->length);
    *size += device->length;
  }
  */
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
