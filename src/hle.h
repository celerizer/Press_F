#ifndef PRESS_F_HLE_H
#define PRESS_F_HLE_H

#include "hw/system.h"

/*
   Return the HLE function pointer for a given BIOS address, or NULL if one
   does not exist.
*/
void* hle_get_func_from_addr(u16 address);

#endif
