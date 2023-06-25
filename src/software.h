#ifndef PRESS_F_SOFTWARE_H
#define PRESS_F_SOFTWARE_H

#include "hw/f8_device.h"

#define SOFTWARE_CRC32_MAX 8
#define SOFTWARE_HOOKUP_MAX 8
#define SYSTEM_HOOKUP_MAX 32

enum
{
  F8_SYSTEM_UNKNOWN = 0,

  F8_SYSTEM_CHANNEL_F,

  F8_SYSTEM_SIZE
};

typedef struct software_hookup_t
{
  /* An identifier specifying which of the implemented devices this is */
  f8_device_id_t type;

  /* An identifier differentiating hooked up devices from each other, so
   * multiple hookups can be made for one allocated device. */
  unsigned id;

  /* The port this hookup takes place in */
  unsigned port;

  /* The virtual memory address this device's data begins at. Only used for
   * ROMC devices. */
  u16 start;

  /* A pointer to the function called when the 3850 performs an IN or INS
   * instruction. If NULL, a default implementation will be used where data is
   * simply written to the port. */
  F8D_OP_IN_T *func_in;

  /* A pointer to the function called when the 3850 performs an OUT or OUTS
   * instruction. If NULL, a default implementation will be used where data is
   * simply recieved from the port. */
  F8D_OP_OUT_T *func_out;
} software_hookup_t;

typedef struct software_t
{
  const char *title;
  unsigned system;
  unsigned crc32[SOFTWARE_CRC32_MAX];
  software_hookup_t hookups[SOFTWARE_HOOKUP_MAX];
} software_t;

typedef struct system_preset_t
{
  const char *title;
  unsigned system;
  software_hookup_t hookups[SYSTEM_HOOKUP_MAX];
} system_preset_t;

extern const software_t pf_software[];
extern const system_preset_t pf_systems[];

const software_t* software_identify(const void *data, u32 size);

#endif
