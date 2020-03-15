#ifndef PRESS_F_FILE_H
#define PRESS_F_FILE_H

#include "hw/channelf.h"
#include "types.h"

u8 load_combined_bios(channelf_t *system, const u8 *data, u16 size);
u8 load_single_bios  (channelf_t *system, const u8 *data, u16 size, u8 bios_b);
u8 load_cartridge    (channelf_t *system, const u8 *data, u16 size);

#endif