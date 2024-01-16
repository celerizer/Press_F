#include "schach_led.h"

static const char *name = "Schach LED";
static const int type = F8_DEVICE_SCHACH_LED;

/* Appears to write 0xC0 to first byte to turn light on, 0x00 to turn it off */

void schach_led_init(f8_device_t *device)
{
  if (device)
  {
    /* Not sure if this is the correct size, could just be 1 */
    f8_generic_init(device, 256);
    device->name = name;
    device->type = type;
    device->flags = F8_DATA_WRITABLE;

    device->init = schach_led_init;
  }
}
