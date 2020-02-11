#ifndef PRESSF_CHANNELF_H
#define PRESSF_CHANNELF_H

#include "3850.h"
#include "f8_device.h"

#define IO_PORTS      32
#define MAX_DEVICES   16

#define ROM_BIOS_A    0x0000
#define ROM_BIOS_B    0x0400
#define ROM_CARTRIDGE 0x0800

/* BIOS size is exact, cart size is a maximum. (inaccurate?)
   The highest cart size in the No-Intro set is 0x1800 bytes. */
#define ROM_BIOS_SIZE 0x0400
#define ROM_CART_SIZE 0x1800

/* 2 bits * 128 width * 64 height */
#define VRAM_SIZE     0x0800

typedef struct channelf_t
{
   void    (**functions)();
   f3850_t f3850;
   u16 pc0;
   u16 pc1;
   u16 dc0;
   u16 dc1;

   f8_device_t *devices[MAX_DEVICES];
   u8           device_count;

   u8      io  [IO_PORTS];
   u8      rom [ROM_CART_SIZE + ROM_BIOS_SIZE * 2];
   u8      vram[VRAM_SIZE];

   u32     cycles;
   u32     total_cycles;
} channelf_t;

u8 channelf_add_device        (channelf_t *system, f8_device_t *device);
u8 channelf_remove_device     (channelf_t *system, u8 index);
u8 channelf_remove_all_devices(channelf_t *system);

#endif
