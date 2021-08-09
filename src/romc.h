#ifndef PRESS_F_ROMC_H
#define PRESS_F_ROMC_H

#include "hw/channelf.h"

#define ROMC_OP(a) void a(channelf_t* system)

#define CYCLE_SHORT 4
#define CYCLE_LONG  6

ROMC_OP(romc00);
ROMC_OP(romc01);
ROMC_OP(romc02);
ROMC_OP(romc03);
ROMC_OP(romc04);
ROMC_OP(romc05);
ROMC_OP(romc06);
ROMC_OP(romc07);
ROMC_OP(romc08);
ROMC_OP(romc09);
ROMC_OP(romc0a);
ROMC_OP(romc0b);
ROMC_OP(romc0c);
ROMC_OP(romc0d);
ROMC_OP(romc0e);
ROMC_OP(romc0f);
ROMC_OP(romc10);
ROMC_OP(romc11);
ROMC_OP(romc12);
ROMC_OP(romc13);
ROMC_OP(romc14);
ROMC_OP(romc15);
ROMC_OP(romc16);
ROMC_OP(romc17);
ROMC_OP(romc18);
ROMC_OP(romc19);
ROMC_OP(romc1a);
ROMC_OP(romc1b);
ROMC_OP(romc1c);
ROMC_OP(romc1d);
ROMC_OP(romc1e);
ROMC_OP(romc1f);

#endif
