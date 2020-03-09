/*
   Copyright (c) 2020 Celerizer
   ---
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.
   ---
   There's a lot of repeated code here to make sure these small functions get
   properly "inlined" with C89.
*/

#ifndef PRESS_F_ROMC_C
#define PRESS_F_ROMC_C

#define ROMC_OP(a) void a(channelf_t* system)

#define CYCLE_SHORT 4
#define CYCLE_LONG  6

u8 f8device_contains(f8device_t* device, u16 address)
{
   return address >= device->start && address <= device->end ? TRUE : FALSE;
}

/*
   Returns a pointer to the F8 device's data at a given offset.
   We need to assume f8device_contains would be true here.
*/
i8* f8device_virtual_ptr(f8device_t* device, u16 offset)
{
#ifdef PRESS_F_SAFETY
   if (!device || !device->data)
      return NULL;
#endif
   offset &= device->mask;

   return device->data? &device->data[offset];
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
   f8device_t *device;
   i8 *return_value = NULL;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_virtual_ptr(device->pc0)
      device->pc0++;
   }
   if (return_value)
      system->dbus = *return_value;

   system->cycles -= CYCLE_SHORT + CYCLE_LONG;
}

/*
   ROMC 0 0 0 0 1 / 01 / L
   ---
   The device whose address space includes the contents of the PC0 register
   must place on the data bus the contents of the memory location addressed
   by PC0; then all devices add the 8-bit value on the data bus, as a 
   signed binary number, to PC0.

   NOTE: We could break once found to do this quicker, but we want to emulate
      proper race conditions eventually.
*/
ROMC_OP(romc01)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_virtual_ptr(device->pc0);
   }
   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->pc0 += system->dbus;
   }

   system->cycles -= CYCLE_LONG;
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
   f8device_t *device;
   i8 *return_value = NULL;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->dc0))
         return_value = f8device_virtual_ptr(device->dc0)
      device->dc0++;
   }
   if (return_value)
      system->dbus = *return_value;

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 0 0 0 1 1 / 03 / L, S
   ---
   Similar to 00, except that it is used for Immediate Operand fetches (using
   PC0) instead of instrcution fetches.

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
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->pc0 = device->pc1;
   }

   system->cycles -= CYCLE_SHORT;
}

/*
   ROMC 0 0 1 0 1 / 05 / L
   ---
   Store the data bus contents into the memory location pointed to by DC0;
   increment DC0.
*/
ROMC_OP(romc05)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->dc0))
         *f8device_virtual_ptr(device, device->dc0) = system->dbus;
      device->dc0++;
   }

   system->cycles -= CYCLE_LONG;
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
   system->dbus = system->devices[0].dc0 & 0xFF00;

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 0 0 1 1 1 / 07 / L
   ---
   Place the high order byte of PC1 on the data bus.

   NOTE: See above.
*/
ROMC_OP(romc07)
{
   system->dbus = system->devices[0].pc1 & 0xFF00;

   system->cycles -= CYCLE_LONG;
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
   f8device_t *device;
   u8 i;

   system->dbus = 0;
   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      device->pc1 = device->pc0;
      device->pc0 = system->dbus * 0x100 + system->dbus;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 0 1 0 0 1 / 09 / L
   ---
   The device whose address space includes the contents of the DC0 register 
   must place the low order byte of DC0 onto the data bus.
*/
ROMC_OP(romc09)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->dc0))
         system->dbus = device->dc0 & 0xFF;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 0 1 0 1 0 / 0A / L
   ---
   All devices add the 8-bit value on the data bus, treated as a signed binary
   number, to the data counter.
*/
ROMC_OP(romc0a)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->dc0 += system->dbus;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 0 1 0 1 1 / 0B / L
   ---
   The device whose address space includes the value in PC1 must place the low
   order byte of PC1 on the data bus.
*/
ROMC_OP(romc0b)
{
   f8device_t *device;
   i8 *return_value = NULL;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->pc1))
         system->dbus = device->pc1 & 0xFF;
   }

   system->cycles -= CYCLE_LONG;
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
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_virtual_ptr(device->pc0);
   }
   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->pc0 = (device->pc0 & 0xFF00) + system->dbus;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 0 1 1 0 1 / 0D / S
   ---
   All devices store in PC1 the current contents of PC0, incremented by 1;
   PC0 is unaltered.
*/
ROMC_OP(romc0d)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->pc1 = device->pc0 + 1;
   }

   system->cycles -= CYCLE_SHORT;
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
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_virtual_ptr(device->pc0);
   }
   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->dc0 = (device->dc0 & 0xFF00) + system->dbus;
   }

   system->cycles -= CYCLE_LONG;
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
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      device->pc1 = device->pc0;
      device->pc0 = (device->pc0 & 0xFF00) + system->dbus;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 0 0 0 0 / 10 / L
   ---
   Inhibit any modification to the interrupt priority logic.

   NOTE: TODO? What is this
*/
ROMC_OP(romc10)
{
   system->cycles -= CYCLE_LONG;
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
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      if (f8device_contains(device, device->pc0))
         system->dbus = *f8device_virtual_ptr(device->pc0);
   }
   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->dc0 = (device->dc0 & 0x00FF) + system->dbus * 0x100;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 0 0 1 0 / 12 / L
   ---
   All devices copy the contents of PC0 into PC1. All devices then move the 
   contents of the data bus into the low order byte of PC0.
*/
ROMC_OP(romc12)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];

      device->pc1 = device->pc0;
      device->pc0 = (device->pc0 & 0xFF00) + system->dbus;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 0 0 1 1 / 13 / L
   ---
   TODO: This
*/
ROMC_OP(romc13)
{
   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 0 1 0 0 / 14 / L
   ---
   All devices move the contents of the dbus into the high order byte of PC0.
*/
ROMC_OP(romc14)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->pc0 = (device->pc0 & 0x00FF) + system->dbus * 0x100;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 0 1 0 1 / 15 / L
   ---
   All devices move the contents of the dbus into the high order byte of PC1.
*/
ROMC_OP(romc15)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->pc1 = (device->pc1 & 0x00FF) + system->dbus * 0x100;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 0 1 1 0 / 16 / L
   ---
   All devices move the contents of the dbus into the high order byte of DC0.
*/
ROMC_OP(romc16)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->dc0 = (device->dc0 & 0x00FF) + system->dbus * 0x100;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 0 1 1 1 / 17 / L
   ---
   All devices move the contents of the dbus into the low order byte of PC0.
*/
ROMC_OP(romc17)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->pc0 = (device->pc0 & 0xFF00) + system->dbus;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 1 0 0 0 / 18 / L
   ---
   All devices move the contents of the dbus into the low order byte of PC1.
*/
ROMC_OP(romc18)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->pc1 = (device->pc1 & 0xFF00) + system->dbus;
   }

   system->cycles -= CYCLE_LONG;
}

/*
   ROMC 1 1 0 0 1 / 19 / L
   ---
   All devices move the contents of the dbus into the low order byte of DC0.
*/
ROMC_OP(romc19)
{
   f8device_t *device;
   u8 i;

   for (i = 0; i < system->f8device_count; i++)
   {
      device = &system->f8devices[i];
      device->dc0 = (device->dc0 & 0xFF00) + system->dbus;
   }

   system->cycles -= CYCLE_LONG;
}

#endif