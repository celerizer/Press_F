#ifndef PRESS_F_ULTRA_MAIN_H
#define PRESS_F_ULTRA_MAIN_H

#include "../../emu.h"

/* 
   Affix "volatile" to the argument which is shared by the main routine and 
   call-back so that it cannot be cleared by the optimization.
*/
extern volatile int stage;

/* N64 controller input data */
extern NUContData contdata[4];

/* Emulator data */
extern channelf_t ultra_channelf;

#endif
