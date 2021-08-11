#ifndef PRESS_F_ROMC_C
#define PRESS_F_ROMC_C

#include "config.h"
#include "romc.h"
#include "types.h"

/* Macros to keep code less redundant */

#define INIT_DEVICES \
   f8_device_t *device; \
   u8 i;

#define FOREACH_DEVICE \
   for (i = 0; i < system->f8device_count; i++) \
   { \
      device = &system->f8devices[i];

/* Helper functions */

/*
   Returns whether a given virtual address is within the region a given
   F8 device is mapped in.
*/
static u8 f8device_contains(f8_device_t* device, u16 address)
{
   return address >= device->start && address <= device->end;
}

static u8* f8device_vptr(f8_device_t* device, u16 address)
{
   address -= device->start;
   address &= device->mask;
   return &device->data[address];
}

static void f8device_write(f8_device_t *device, u16 address, u8 data)
{
   address -= device->start;
   address &= device->mask;
   device->data[address] = data;
}

/*
   ROMC 0 0 0 0 0 / 00 / S, L
   ---
   Instruction Fetch. The device whose address space includes the contents of
   the PC0 register must place on the data bus the op code addressed by PC0;
   then all devices increment the contents of PC0.

   NOTES: If a device's PC0 goes out of sync with the rest, this could cause
      multiple devices to write to the data bus. What's the accurate
      behavior here?
*/
ROMC_OP(romc00)
{
   INIT_DEVICES

#if PF_ROMC_REDUNDANCY == FALSE
   FOREACH_DEVICE
      if (f8device_contains(device, device->pc0))
      {
         system->dbus = *f8device_vptr(device, device->pc0);
         break;
      }
   }
   FOREACH_DEVICE
      device->pc0++;
   }
#else
   FOREACH_DEVICE
      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_vptr(device, device->pc0);
      device->pc0++;
   }
#endif

   system->cycles += CYCLE_SHORT + CYCLE_LONG;
}

/*
   ROMC 0 0 0 0 1 / 01 / L
   ---
   The device whose address space includes the contents of the PC0 register
   must place on the data bus the contents of the memory location addressed
   by PC0; then all devices add the 8-bit value on the data bus, as a
   signed binary number, to PC0.

   NOTES: We could break once found to do this quicker, but we want to emulate
      proper race conditions eventually. However, I don't know how out-of-sync
      devices actually behave.
*/
ROMC_OP(romc01)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_vptr(device, device->pc0);
   }

   FOREACH_DEVICE
      device->pc0 += system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 0 0 1 0 / 02 / L
   ---
   The device whose DC0 addresses a memory word within the address space of
   that device must place on the data bus the contents of the memory location
   addressed by DC0; then all devices increment DC0.
*/
ROMC_OP(romc02)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->dc0))
         system->dbus = *f8device_vptr(device, device->dc0);
      device->dc0++;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 0 0 1 1 / 03 / L, S
   ---
   Similar to 00, except that it is used for Immediate Operand fetches (using
   PC0) instead of instruction fetches.

   NOTES: The documentation calls it similar and LONG/SHORT is flipped, is
      there any functional difference?
*/
ROMC_OP(romc03)
{
   romc00(system);
}

/*
   ROMC 0 0 1 0 0 / 04 / S
   ---
   Copy the contents of PC1 into PC0.
*/
ROMC_OP(romc04)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->pc0 = device->pc1;
   }

   system->cycles += CYCLE_SHORT;
}

/*
   ROMC 0 0 1 0 1 / 05 / L
   ---
   Store the data bus contents into the memory location pointed to by DC0;
   increment DC0.
*/
ROMC_OP(romc05)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->dc0))
         f8device_write(device, device->dc0, system->dbus);
      device->dc0++;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 0 1 1 0 / 06 / L
   ---
   Place the high order byte of DC0 on the data bus.

   NOTE: Because timing here won't be accurate anyway, and all devices should
      write the same value, we only worry about the first F8 device (probably
      the 3850).
*/
ROMC_OP(romc06)
{
   system->dbus = (system->f8devices[0].dc0 & 0xFF00) >> 8;

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 0 1 1 1 / 07 / L
   ---
   Place the high order byte of PC1 on the data bus.

   NOTE: See above.
*/
ROMC_OP(romc07)
{
   system->dbus = (system->f8devices[0].pc1 & 0xFF00) >> 8;

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 1 0 0 0 / 08 / L
   ---
   All devices copy the contents of PC0 into PC1. The CPU outputs zero on the
   data bus in this ROMC state. Load the data bus into both halves of PC0,
   thus clearing the register.

   NOTE: PC0 could be set directly to zero here, but we do it the slow way to
      emulate possible data bus corruption (unless this gets compiled away).
*/
ROMC_OP(romc08)
{
   INIT_DEVICES

   system->dbus = 0;

   FOREACH_DEVICE
      device->pc1 = device->pc0;
      device->pc0 = ((u16)(system->dbus << 8) | system->dbus);
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 1 0 0 1 / 09 / L
   ---
   The device whose address space includes the contents of the DC0 register
   must place the low order byte of DC0 onto the data bus.
*/
ROMC_OP(romc09)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->dc0))
         system->dbus = device->dc0 & 0xFF;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 1 0 1 0 / 0A / L
   ---
   All devices add the 8-bit value on the data bus, treated as a signed binary
   number, to the data counter.
*/
ROMC_OP(romc0a)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->dc0 += system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 1 0 1 1 / 0B / L
   ---
   The device whose address space includes the value in PC1 must place the low
   order byte of PC1 on the data bus.
*/
ROMC_OP(romc0b)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->pc1))
         system->dbus = device->pc1 & 0xFF;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 1 1 0 0 / 0C / L
   ---
   The device whose address space includes the contents of the PC0 register
   must place the contents of the memory word addressed by PC0 onto the
   data bus; then all devices move the value that has just been placed on
   the data bus into the low order byte of PC0.

   NOTE: We could break once found to do this quicker, but we want to emulate
      proper race conditions eventually.
*/
ROMC_OP(romc0c)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_vptr(device, device->pc0);
   }
   FOREACH_DEVICE
      device->pc0 = (device->pc0 & 0xFF00) + system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 1 1 0 1 / 0D / S
   ---
   All devices store in PC1 the current contents of PC0, incremented by 1;
   PC0 is unaltered.
*/
ROMC_OP(romc0d)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->pc1 = device->pc0 + 1;
   }

   system->cycles += CYCLE_SHORT;
}

/*
   ROMC 0 1 1 1 0 / 0E / L
   ---
   The device whose memory space includes the contents of PC0 must place the
   contents of the word addressed by PC0 onto the data bus. The value on the
   data bus is then moved to the low order byte of DC0 by all devices.
*/
ROMC_OP(romc0e)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_vptr(device, device->pc0);
   }
   FOREACH_DEVICE
      device = &system->f8devices[i];
      device->dc0 = (device->dc0 & 0xFF00) | system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 0 1 1 1 1 / 0F / L
   ---
   The interrupting device with highest priority must place the low order
   byte of the interrupt vector on the data bus. All devices must copy the
   contents of PC0 into PC1. All devices must move the contents of the data bus
   into the low order byte of PC0.

   NOTE: Interrupt stuff is TODO!
*/
ROMC_OP(romc0f)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->pc1 = device->pc0;
      device->pc0 = (device->pc0 & 0xFF00) + system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 0 0 0 0 / 10 / L
   ---
   Inhibit any modification to the interrupt priority logic.

   NOTE: TODO? What is this
*/
ROMC_OP(romc10)
{
   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 0 0 0 1 / 11 / L
   ---
   The device whose memory space includes the contents of PC0 must place the
   contents of the addressed memory word onto the data bus. All devices must
   then move the contents of the data bus to the upper byte of DC0.
*/
ROMC_OP(romc11)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_vptr(device, device->pc0);
 #if PF_ROMC_REDUNDANCY == TRUE
      break;
 #endif
   }
   FOREACH_DEVICE
      device->dc0 &= 0x00FF;
      device->dc0 |= system->dbus << 8;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 0 0 1 0 / 12 / L
   ---
   All devices copy the contents of PC0 into PC1. All devices then move the
   contents of the data bus into the low order byte of PC0.
*/
ROMC_OP(romc12)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->pc1 = device->pc0;
      device->pc0 &= 0xFF00;
      device->pc0 |= system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 0 0 1 1 / 13 / L
   ---
   TODO: This
*/
ROMC_OP(romc13)
{
   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 0 1 0 0 / 14 / L
   ---
   All devices move the contents of the dbus into the high order byte of PC0.
*/
ROMC_OP(romc14)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->pc0 &= 0x00FF;
      device->pc0 |= system->dbus << 8;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 0 1 0 1 / 15 / L
   ---
   All devices move the contents of the dbus into the high order byte of PC1.
*/
ROMC_OP(romc15)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->pc1 &= 0x00FF;
      device->pc1 |= system->dbus << 8;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 0 1 1 0 / 16 / L
   ---
   All devices move the contents of the dbus into the high order byte of DC0.
*/
ROMC_OP(romc16)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->dc0 = (device->dc0 & 0x00FF) + system->dbus * 0x100;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 0 1 1 1 / 17 / L
   ---
   All devices move the contents of the dbus into the low order byte of PC0.
*/
ROMC_OP(romc17)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->pc0 = (device->pc0 & 0xFF00) + system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 1 0 0 0 / 18 / L
   ---
   All devices move the contents of the dbus into the low order byte of PC1.
*/
ROMC_OP(romc18)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->pc1 &= 0xFF00;
      device->pc1 |= system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 1 0 0 1 / 19 / L
   ---
   All devices move the contents of the dbus into the low order byte of DC0.
*/
ROMC_OP(romc19)
{
   INIT_DEVICES

   FOREACH_DEVICE
      device->dc0 = (device->dc0 & 0xFF00) + system->dbus;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 1 0 1 0 / 1A / L
   ---
   During the prior cycle, an I/O port timer or interrupt controller register
   was addressed; the device conatining the addressed port must move the
   current contents of the data bus into the addressed port.
*/
ROMC_OP(romc1a)
{
   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 1 0 1 1 / 1B / L
   ---
   During the prior cycle, the data bus specified the address of an I/O port.
   The device containing the addressed I/O port must place the contents of
   the I/O port on the data bus.
   (Note that the contents of timer and interrupt control registers cannot be
   read back onto the data bus.)
*/
ROMC_OP(romc1b)
{
   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 1 1 0 0 / 1C / L or S
   ---
   None.
*/
ROMC_OP(romc1c)
{
   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 1 1 0 1 / 1D / S
   ---
   Devices with DC0 and DC1 registers must switch registers. Devices without a
   DC1 register perform no operation.
*/
ROMC_OP(romc1d)
{
   INIT_DEVICES
   u16 temp;

   FOREACH_DEVICE
      temp = device->dc0;
      device->dc0 = device->dc1;
      device->dc0 = temp;
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 1 1 1 0 / 1E / L
   ---
   The device whose address space includes the contents of PC0 must place the
   low order byte of PC0 onto the data bus.
*/
ROMC_OP(romc1e)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->pc0))
      {
         system->dbus = device->pc0 & 0xFF;
#if PF_ROMC_REDUNDANCY == FALSE
         break;
#endif
      }
   }

   system->cycles += CYCLE_LONG;
}

/*
   ROMC 1 1 1 1 0 / 1F / L
   ---
   The device whose address space includes the contents of PC0 must place the
   high order byte of PC0 onto the data bus.
*/
ROMC_OP(romc1f)
{
   INIT_DEVICES

   FOREACH_DEVICE
      if (f8device_contains(device, device->pc0))
      {
         system->dbus = (device->pc0 & 0xFF00) >> 8;
#if PF_ROMC_REDUNDANCY == FALSE
         break;
#endif
      }
   }

   system->cycles += CYCLE_LONG;
}

#endif
