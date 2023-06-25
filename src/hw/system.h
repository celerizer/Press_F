#ifndef PRESS_F_SYSTEM_H
#define PRESS_F_SYSTEM_H

#include "3850.h"
#include "f8_device.h"
#include "../config.h"

#define F8_MAX_IO_PORTS 128

#define ROM_BIOS_A    0x0000
#define ROM_BIOS_B    0x0400
#define ROM_CARTRIDGE 0x0800

/* Arbitrary limit for max number of devices hooked up to a system */
#define F8_MAX_DEVICES 16

/**
 * A holder that serves as a "connection" between the emulated system and an
 * IO device.
 **/
typedef struct io_t
{
  struct f8_device_t *device_out;
  struct f8_device_t *device_in;
  F8D_OP_OUT_T *func_out;
  F8D_OP_IN_T *func_in;
  f8_byte data;
} io_t;

typedef struct f8_system_t
{
  u16 dc0;
  u16 dc1;
  u16 pc0;
  u16 pc1;

  f8_byte dbus;

  int cycles;
  int total_cycles;

  f8_device_t f8devices[F8_MAX_DEVICES];
  unsigned f8device_count;
  f3850_t *main_cpu;

  io_t io_ports[F8_MAX_IO_PORTS];

  pf_settings_t settings;
} f8_system_t;

/**
 * Gets a pointer to the main CPU of an F8 system.
 * This returns the first 3850 unit in the devices array.
 */
f3850_t* f8_main_cpu(f8_system_t *system);

/**
 * Gets a pointer to the device wrapper of the main CPU of an F8 system.
 * This returns the first 3850 unit in the devices array.
 */
f8_device_t* f8_main_cpu_device(f8_system_t *system);

/**
 * Reads data from the F8 system into a buffer.
 * @param system A pointer to an F8 system.
 * @param dest A pointer to the destination buffer to read into.
 * @param address The virtual address in the F8 system to read from.
 * @param size The number of bytes to read.
 * @return The number of bytes read, equal to size if there is no error.
 */
unsigned f8_read(f8_system_t *system, void *dest, unsigned address,
                 unsigned size);

/**
 * Writes data from a buffer into the F8 system.
 * @param system A pointer to an F8 system.
 * @param address The virtual address in the F8 system to write to.
 * @param src A pointer to the data buffer to write from.
 * @param size The number of bytes to write.
 * @return The number of bytes written, equal to size if there is no error.
 */
unsigned f8_write(f8_system_t *system, unsigned address, const void *src,
                  unsigned size);

u8 f8_device_add(f8_system_t *system, f8_device_t *device);

u8 f8_device_remove(f8_system_t *system, f8_device_t *device);

u8 f8_device_remove_index(f8_system_t *system, unsigned index);

u8 f8_system_init(f8_system_t *system, const system_preset_t *preset);

#endif
