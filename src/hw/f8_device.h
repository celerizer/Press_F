#ifndef PRESS_F_F8_DEVICE_H
#define PRESS_F_F8_DEVICE_H

#include "../types.h"

typedef enum
{
  F8_DEVICE_INVALID = 0,

  F8_DEVICE_2102,
  F8_DEVICE_2102L,
  F8_DEVICE_2111,

  /* F8 system main CPU */
  F8_DEVICE_3850,

  /* 1024 byte program storage unit */
  F8_DEVICE_3851,

  /* 2048 byte program storage unit */
  F8_DEVICE_3856,

  F8_DEVICE_KDBUG,
  f8_DEVICE_FAIRBUG_SERIAL,
  F8_DEVICE_FAIRBUG_PARALLEL,

  /**
   * VRAM device used in the Channel F.
   **/
  F8_DEVICE_MK4027,
  F8_DEVICE_BEEPER,

  F8_DEVICE_SELECTOR_CONTROL,
  F8_DEVICE_HAND_CONTROLLER,

  F8_DEVICE_SIZE
} f8_device_id_t;

/*
  Some devices may not actually have registers that are included in the
  spec. These flags will prevent accessing them if needed.
*/
#define F8_NO_PC0 (1 << 0)
#define F8_NO_PC1 (1 << 1)
#define F8_NO_DC0 (1 << 2)
#define F8_NO_DC1 (1 << 3)
#define F8_NO_ROMC (F8_NO_PC0 | F8_NO_PC1 | F8_NO_DC0 | F8_NO_DC1)

/*
  The memory region includes cartridge, BIOS, or otherwise copyrighted data.
  Its contents should not be included during serialization.
*/
#define F8_HAS_COPYRIGHTED_DATA (1 << 4)

/*
  The memory region is writable.
  For example, this would be TRUE on a RAM chip but FALSE on a cartridge.
*/
#define F8_DATA_WRITABLE (1 << 5)

/*
  The memory region is mapped to a battery-backed SRAM chip.
  Memory regions with this flag should have their data flushed to the disk.
*/
#define F8_HAS_BATTERY (1 << 6)

/**
 * The device will be accessed via one or more IO ports.
 **/
#define F8_HAS_PORT_HOOKUPS (1 << 7)

typedef struct f8_device_t
{
  const char *name;

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
  f8_byte *data;
  u16   start;
  u16   end;
  u16   length;
  u16   mask;

  int type;

/*
  Bitfield specifying some properties of the device and how to access it.
  See the F8 flags defined above.
*/
  u32 flags;

/*
  Device-specific behavior
*/
  void *device;
  void (*init)       (struct f8_device_t *device);
  void (*free)       (struct f8_device_t *device);
  void (*reset)      (struct f8_device_t *device);
  void (*serialize)  (struct f8_device_t *device);
  void (*unserialize)(struct f8_device_t *device);
} f8_device_t;

#define F8D_OP_IN(a) void a(f8_device_t *device, f8_byte *io_data)
#define F8D_OP_OUT(a) void a(f8_device_t *device, f8_byte *io_data, f8_byte value)
typedef void F8D_OP_IN_T(f8_device_t*, f8_byte*);
typedef void F8D_OP_OUT_T(f8_device_t*, f8_byte*, f8_byte);

#endif
