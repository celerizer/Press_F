#ifndef PRESS_F_CONFIG_H
#define PRESS_F_CONFIG_H

#include "types.h"

/**
 * Constantly checks the validity of pointers in main emulation loop.
 * Could be a performance hit and the program should be stable to the point
 * where this isn't needed.
 */
#ifndef PRESS_F_SAFETY
#define PRESS_F_SAFETY FALSE
#endif

/**
 * Enable audio processing.
 */
#ifndef PF_AUDIO_ENABLE
#define PF_AUDIO_ENABLE TRUE
#endif

/**
 * Enable the debugger.
 */
#ifndef PF_DEBUGGER
#define PF_DEBUGGER FALSE
#endif

/**
 * Defines whether or not to use static asserts at compile time.
 * Only necessary for developers.
 */
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
 */
#ifndef PF_SOUND_FREQUENCY
#define PF_SOUND_FREQUENCY 44100
#endif
PF_STATIC_ASSERT(!(PF_SOUND_FREQUENCY % 60),
                 "Audio frequency not evenly divisible by 60")

#ifndef PF_SOUND_DECAY
#define PF_SOUND_DECAY (1.0 - 77.0 / PF_SOUND_FREQUENCY)
#endif

#ifndef PF_SOUND_PERIOD
#define PF_SOUND_PERIOD 1.0 / PF_SOUND_FREQUENCY
#endif

#ifndef PF_SOUND_SAMPLES
#define PF_SOUND_SAMPLES PF_SOUND_FREQUENCY / 60
#endif

/**
 * Determine whether HLE BIOS implementations should be used.
 * @todo This should be a runtime option as well.
 */
#ifndef PF_HAVE_HLE_BIOS
#define PF_HAVE_HLE_BIOS TRUE
#endif

/**
 * Minimum / maximum sound volume. Needs to be a signed short.
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

#endif
