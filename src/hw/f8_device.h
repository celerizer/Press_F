#ifndef PRESS_F_F8_DEVICE_H
#define PRESS_F_F8_DEVICE_H

#include "../types.h"

typedef struct f8_device_t
{
   char name[256];

/*
   Hardware
*/
   u16 pc0;
   u16 pc1;
   u16 dc0;
   u16 dc1;

/*
   Example from a 2102 RAM chip

   start  = 0x2000
   end    = 0x23FF
   length = 0x0400
   mask   = 0x03FF
*/
   u8  *data;
   u16  start;
   u16  end;
   u16  length;
   u16  mask;

/*
   Set this if the region includes any cartridge, BIOS, or otherwise
   copyrighted data so it won't be included in savestates.
*/
   u8 include_in_savestates;

/*
   Set this if a memory region is writable. For example, this would
   be TRUE on a RAM chip but FALSE on a cartridge.
*/
   u8 writable;
   
/*
   Device-specific behavior
*/
   void *device;
   void (*init) (void *device);
   void (*free) (void *device);
   void (*reset)(void *device);
} f8_device_t;

#endif
