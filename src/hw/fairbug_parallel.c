#include "fairbug_parallel.h"
#include "../input.h"

static const char *name = "FAIR-BUG Parallel TTY";
static const int type = F8_DEVICE_FAIRBUG_PARALLEL;

F8D_OP_IN(fairbug_parallel_io)
{
  io_data->u = get_keyboard_char();
}

void fairbug_parallel_init(f8_device_t *device)
{
  if (device)
  {
    device->name = name;
    device->type = type;
  }
}
