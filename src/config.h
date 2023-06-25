#ifndef PRESS_F_CONFIG_H
#define PRESS_F_CONFIG_H

#include "software.h"
#include "types.h"

/*
================================================================================
   Compile-time options
================================================================================
*/

/**
 * Constantly checks the validity of pointers in main emulation loop.
 * Could be a performance hit and the program should be stable to the point
 * where this isn't needed.
 **/
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

/**
 * Defines whether or not to use static asserts at compile time.
 * Only necessary for developers.
 **/
#ifndef PF_STATIC_ASSERTS
#define PF_STATIC_ASSERTS FALSE
#endif
#if PF_STATIC_ASSERTS
#define PF_STATIC_ASSERT(a, b) static_assert(a, b)
#else
#define PF_STATIC_ASSERT(a, b)
#endif

/**
 * Sampling frequency for sound.
 * Turn this down if audio causes slowdowns.
 **/
#ifndef PF_FREQUENCY
#define PF_FREQUENCY 44100
#endif
PF_STATIC_ASSERT(!(PF_FREQUENCY % 60),
                 "Audio frequency not evenly divisible by 60")

/**
 * Determine whether HLE BIOS implementations should be used.
 * @todo This should be a runtime option as well.
 **/
#ifndef PF_HAVE_HLE_BIOS
#define PF_HAVE_HLE_BIOS TRUE
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

/**
 * Controls whether or not to emulate ROMC functions.
 * Not emulating ROMC allows for using simpler instruction sets, but removes
 * some functionality.
 */
#ifndef PF_ROMC
#define PF_ROMC TRUE
#endif

/**
 * Controls whether or not to break redundant loops in ROMC functions.
 * While allowing this should be more accurate, the only way it should
 * change behavior is on miswired or otherwise damaged hardware.
 */
#ifndef PF_ROMC_REDUNDANCY
#define PF_ROMC_REDUNDANCY FALSE
#endif

/**
 * Controls whether or not memory regions that contain copyrighted material
 * (with flag F8_HAS_COPYRIGHTED_DATA) are included during serialization.
 */
#ifndef PF_SERIALIZE_COPYRIGHTED_CONTENT
#define PF_SERIALIZE_COPYRIGHTED_CONTENT FALSE
#endif

/* Runtime options */
typedef struct pf_settings_t
{
  /**
   * Determines which system preset to load if software identification fails.
   * See "software.h"
   */
  u8 default_system;

  /**
   * Bool: Always hookup a 2114 chip at $2800 under Channel F series presets.
   * This is very common for Channel F homebrew.
   */
  u8 cf_always_scach;

  /**
   * Bool: Rasterizes extra VRAM data to the framebuffer.
   */
  u8 cf_full_vram;

  /**
   * Bool: Allows any software to be played under Channel F series presets.
   * Done by NOPing $0015 and $0016 in the 3851 BIOS.
   */
  u8 cf_skip_cartridge_verification;
} pf_settings_t;

/*const pf_settings_t settings_default =
{
  F8_SYSTEM_CHANNEL_F,
  FALSE,
  FALSE,
  FALSE
};*/

#endif
