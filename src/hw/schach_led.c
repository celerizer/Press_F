#include "schach_led.h"

static const char *name = "Schach LED";
static const int type = F8_DEVICE_SCHACH_LED;

void schach_led_init(f8_device_t *device)
{
  if (device)
  {
    /* Not sure if this is the correct size, could just be 1 */
    f8_generic_init(device, 256);
    device->name = name;
    device->type = type;

    device->init = schach_led_init;
  }
}
