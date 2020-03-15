#ifndef PRESS_F_EMU_H
#define PRESS_F_EMU_H

#include "hw/channelf.h"

typedef struct opcode_t
{
   u8          length;
   const char *format;
   const char *description;
} opcode_t;

extern opcode_t opcodes[256];

u8   pressf_init    (channelf_t *system);
u8   pressf_load_rom(channelf_t *system);
void pressf_reset   (channelf_t *system);
u8   pressf_run     (channelf_t *system);
void pressf_step    (channelf_t *system);

#endif
