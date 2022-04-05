#ifndef PRESS_F_DEBUG_H
#define PRESS_F_DEBUG_H

#include "types.h"

/**
 * Adds a breakpoint for a given address.
 * Returns TRUE on success, FALSE otherwise.
 */
u8 debug_add_breakpoint(int address);

/**
 * Removes a breakpoint for a given address.
 * Returns TRUE on success, FALSE otherwise.
 */
u8 debug_remove_breakpoint(int address);

/**
 * Returns whether or not a breakpoint has been triggered.
 */
u8 debug_should_break(int address);

#endif
