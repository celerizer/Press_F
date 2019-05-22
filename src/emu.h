#ifndef PRESSF_EMU_H
#define PRESSF_EMU_H

#include "hw/channelf.h"
#include "read.h"

u8 pressf_init();
u8 pressf_run(channelf_t *system);
void pressf_step(channelf_t *system);

#endif