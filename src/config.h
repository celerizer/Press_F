#ifndef PRESS_F_CONFIG_H
#define PRESS_F_CONFIG_H

#include "types.h"

/*
================================================================================
   Compile-time options
================================================================================
*/

/*
   Constantly checks the validity of pointers in main emulation loop.
   Could be a performance hit and the program should be stable to the point
   where this isn't needed.
*/
#ifndef PRESS_F_SAFETY
#define PRESS_F_SAFETY FALSE
#endif

/*
   Enable audio processing.
*/
#ifndef PF_AUDIO_ENABLE
#define PF_AUDIO_ENABLE TRUE
#endif

/*
   Enable the debugger.
*/
#ifndef PF_DEBUGGER
#define PF_DEBUGGER TRUE
#endif

/*
   Sampling frequency for sound.
   Turn this down if audio causes slowdowns, but keep it divisible by 60.
*/
#ifndef PF_FREQUENCY
#define PF_FREQUENCY 44100
#endif

/*
   Minimum / maximum sound volume. Needs to be a signed short.
*/
#ifndef PF_MIN_AMPLITUDE
#define PF_MIN_AMPLITUDE 0x0000
#endif
#ifndef PF_MAX_AMPLITUDE
#define PF_MAX_AMPLITUDE 0x7FFF
#endif

/*
   Controls whether or not to break possibly redundant loops in ROMC functions.
   While allowing this should be more accurate, the only way it should
   change behavior is on miswired or otherwise damaged hardware.
*/
#ifndef PF_ROMC_REDUNDANCY
#define PF_ROMC_REDUNDANCY FALSE
#endif

/* Runtime options */
typedef struct pf_settings_t
{
   /* Changes the rendered framebuffer size */
   u8 screen_size;

   /* Allows any ROM to be played by NOPing 0x15 and 0x16 in the BIOS */
   u8 skip_cartridge_verification;
} pf_settings_t;

#endif
