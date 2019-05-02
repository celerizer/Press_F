#ifndef PRESSF_FILE_C
#define PRESSF_FILE_C

#include <string.h>

#include "file.h"
#include "types.h"
#include "hw/channelf.h"

u8 load_combined_bios(channelf_t *system, const u8 *data, u16 size)
{
   if (!system || !data || size != ROM_BIOS_SIZE * 2)
      return FALSE;
   memcpy(&system->rom[ROM_BIOS_A], &data[ROM_BIOS_A], ROM_BIOS_SIZE);
   memcpy(&system->rom[ROM_BIOS_B], &data[ROM_BIOS_B], ROM_BIOS_SIZE);

   return TRUE;
}

u8 load_single_bios(channelf_t *system, const u8 *data, u16 size, u8 bios_b)
{
   if (!system || !data || size != ROM_BIOS_SIZE)
      return FALSE;
   memcpy(bios_b ? &system->rom[ROM_BIOS_B] : &system->rom[ROM_BIOS_A], data, size);

   return TRUE;
}

u8 load_cartridge(channelf_t *system, const u8 *data, u16 size)
{
   if (!system || !data || !size || size > ROM_CART_SIZE)
      return FALSE;
   memcpy(&system->rom[ROM_CARTRIDGE], data, size);

   return TRUE;
}

#endif