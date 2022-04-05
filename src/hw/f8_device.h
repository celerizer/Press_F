#ifndef PRESS_F_F8_DEVICE_H
#define PRESS_F_F8_DEVICE_H

#include "../types.h"

/*
   Some devices may not actually have registers that are included in the
   spec. These flags will prevent accessing them if needed.
*/
#define F8_NO_PC0 (1 << 0)
#define F8_NO_PC1 (1 << 1)
#define F8_NO_DC0 (1 << 2)
#define F8_NO_DC1 (1 << 3)

/*
   Set this if the data region includes any cartridge, BIOS, or otherwise
   copyrighted data so it won't be included in savestates.
*/
#define F8_HAS_COPYRIGHTED_DATA (1 << 4)

/*
   Set this if a memory region is writable. For example, this would be TRUE on
   a RAM chip but FALSE on a cartridge.
*/
#define F8_DATA_WRITABLE (1 << 5)

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
   Bitfield specifying some properties of the device and how to access it.
   See the F8 flags defined above.
*/
   u32 flags;

/*
   Device-specific behavior
*/
   void *device;
   void (*init) (void *device);
   void (*free) (void *device);
   void (*reset)(void *device);

   void (*functions[8])(f8_device_t *device, u8 *port_data);
} f8_device_t;

#endif
