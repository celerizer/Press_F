#ifndef PRESS_F_CHANNELF_H
#define PRESS_F_CHANNELF_H

#include "3850.h"
#include "f8_device.h"

#define IO_PORTS      16

#define ROM_BIOS_A    0x0000
#define ROM_BIOS_B    0x0400
#define ROM_CARTRIDGE 0x0800

/* BIOS size is exact, cart size is a maximum. (inaccurate?)
   The highest cart size in the No-Intro set is 0x1800 bytes. */
#define ROM_BIOS_SIZE 0x0400
#define ROM_CART_SIZE 0x1800

/* 2 bits * 128 width * 64 height */
#define VRAM_SIZE     0x0800

/* Arbitrary limit for max number of devices hooked up to a system */
#define F8_MAX_DEVICES 16

typedef struct channelf_t
{
   void    (**functions)();
   c3850_t c3850;

   u16 dc0;
   u16 dc1;
   u16 pc0;
   u16 pc1;

   u8 io  [IO_PORTS];
   u8 rom [ROM_CART_SIZE + ROM_BIOS_SIZE * 2];
   u8 vram[VRAM_SIZE];
   i8 dbus;

   u32 cycles;
   u32 total_cycles;

   f8_device_t f8devices[F8_MAX_DEVICES];
   u16         f8device_count;
} channelf_t;

#endif
