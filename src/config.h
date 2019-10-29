#ifndef PRESS_F_CONFIG_H
#define PRESS_F_CONFIG_H

#include "types.h"

/* Compile-time options */

/* Constantly checks the validity of pointers in main emulation loop */
/* Causes slowdowns and the program should be stable to the point where this isn't needed */
#define PRESS_F_SAFETY FALSE

/* Runtime options */
typedef struct pf_settings_t
{
   /* Changes the rendered framebuffer size */
   u8 screen_size;

   /* Allows any ROM to be played by NOPing 0x15 and 0x16 in the BIOS */
   u8 skip_cartridge_verification;
} pf_settings_t;

#endif
