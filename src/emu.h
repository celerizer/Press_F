#ifndef PRESS_F_EMU_H
#define PRESS_F_EMU_H

#include "hw/system.h"

typedef struct opcode_t
{
  unsigned length;
  const char *format;
  const char *description;
} opcode_t;

extern opcode_t opcodes[256];

u8   pressf_init    (f8_system_t *system);
u8   pressf_load_rom(f8_system_t *system);
void pressf_reset   (f8_system_t *system);
u8   pressf_run     (f8_system_t *system);
void pressf_step    (f8_system_t *system);

#endif
