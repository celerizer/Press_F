#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "emu.h"
#include "hle.h"
#include "input.h"
#include "romc.h"
#include "screen.h"
#include "sound.h"

#define A    ((f3850_t*)system->f8devices[0].device)->accumulator
#define ISAR ((f3850_t*)system->f8devices[0].device)->isar
#define W    ((f3850_t*)system->f8devices[0].device)->status_register

#define J    ((f3850_t*)system->f8devices[0].device)->scratchpad[0x09]
#define HU   ((f3850_t*)system->f8devices[0].device)->scratchpad[0x0A]
#define HL   ((f3850_t*)system->f8devices[0].device)->scratchpad[0x0B]
#define KU   ((f3850_t*)system->f8devices[0].device)->scratchpad[0x0C]
#define KL   ((f3850_t*)system->f8devices[0].device)->scratchpad[0x0D]
#define QU   ((f3850_t*)system->f8devices[0].device)->scratchpad[0x0E]
#define QL   ((f3850_t*)system->f8devices[0].device)->scratchpad[0x0F]

#define DC0  system->f8devices[0].dc0
#define DC1  system->f8devices[0].dc1
#define PC0  system->f8devices[0].pc0
#define PC1  system->f8devices[0].pc1

opcode_t opcodes[256] =
{
  { 1, "LR A, Ku",   "Loads r12 (upper byte of K) into the accumulator." },
  { 1, "LR A, Kl",   "Loads r13 (lower byte of K) into the accumulator." },
  { 1, "LR A, Qu",   "Loads r14 (upper byte of Q) into the accumulator." },
  { 1, "LR A, Ql",   "Loads r15 (lower byte of Q) into the accumulator." },
  { 1, "LR Ku, A",   "Loads the accumulator into r12 (upper byte of K)." },
  { 1, "LR Kl, A",   "Loads the accumulator into r13 (lower byte of K)." },
  { 1, "LR Qu, A",   "Loads the accumulator into r14 (upper byte of Q)." },
  { 1, "LR Ql, A",   "Loads the accumulator into r15 (lower byte of Q)." },
  { 1, "LR K, PC1",  "Loads PC1 into K (r12 and r13)." },
  { 1, "LR PC1, K",  "Loads K (r12 and r13) into PC1." },
  { 1, "LR A, ISAR", "Loads the register referenced by the ISAR into the accumulator."},
  { 1, "LR ISAR, A", "Loads the accumulator into the register referenced by the ISAR."},
  { 1, "PK",         "Loads PC0 into PC1, then loads K (r12 and r13) into PC0."},
  { 1, "LR PC0, Q",  "Loads Q (r14 and r15) into PC0."},
  { 1, "LR Q, DC0",  "Loads DC0 into Q (r14 and r15)."},
  { 1, "LR DC0, Q",  "Loads Q (r14 and r15) into DC0."},
  { 1, "LR DC0, H",  "Loads H (r10 and r11) into DC0."},
  { 1, "LR H, DC0",  "Loads DC0 into H (r10 and r11)."},
  { 1, "SR 1",       "Shifts the accumulator right by one bit."},
  { 1, "SL 1",       "Shifts the accumulator left by one bit."},
  { 1, "SR 4",       "Shifts the accumulator right by four bits."},
  { 1, "SL 4",       "Shifts the accumulator left by four bits."},
  { 1, "LM",         "Loads the value addressed by DC0 into the accumulator."},
  { 1, "ST",         "Loads the accumulator into DC0."},
  { 1, "COM",        "Complements the accumulator."},
  { 1, "LNK",        "Adds the carry status flag to the accumulator."},
  { 1, "DI" ,        "Disables interrupt requests."},
  { 1, "EI" ,        "Enables interrupt requests."},
  { 1, "POP",        "Loads PC1 into PC0."},

};

#define F8_OP(a) void a(f8_system_t *system)

static void (*operations[256])(f8_system_t *system);

int get_status(f8_system_t *system, const unsigned flag)
{
  return (W & flag) != 0;
}

void set_status(f8_system_t *system, const unsigned flag, unsigned enable)
{
  W = enable ? W | flag : W & ~flag;
}

void add(f8_system_t *system, f8_byte *dest, unsigned src)
{
  unsigned result = dest->u + src;

  /* Sign bit is set if the result is positive or zero */
  set_status(system, STATUS_SIGN, !(result & B10000000));

  /* We carried if we went above max 8-bit */
  set_status(system, STATUS_CARRY, result > 0xFF);

  /* We zeroed if the result was zero */
  set_status(system, STATUS_ZERO, (result & 0xFF) == 0);

  /* We overflowed up or down if result goes beyond 8 bit (changed sign) */
  set_status(system, STATUS_OVERFLOW, ((result ^ dest->u) & (result ^ src) & 0x80));

  dest->u = result;
}

/**
 * See Section 6.4
 */
void add_bcd(f8_system_t *system, f8_byte *augend, unsigned addend)
{
  unsigned tmp = augend->u + addend;
  int c = 0;
  int ic = 0;

  if (((augend->u + addend) & 0xff0) > 0xf0)
    c = 1;
  if ((augend->u & 0x0f) + (addend & 0x0f) > 0x0F)
    ic = 1;

  add(system, augend, addend);

  if (!c && !ic)
    tmp = ((tmp + 0xa0) & 0xf0) + ((tmp + 0x0a) & 0x0f);
  else if (!c && ic)
    tmp = ((tmp + 0xa0) & 0xf0) + (tmp & 0x0f);
  else if (c && !ic)
    tmp = (tmp & 0xf0) + ((tmp + 0x0a) & 0x0f);

  augend->u = tmp;
}

/**
 * Gets a pointer to the byte the ISAR (Indirect Scratchpad Address Register)
 * is addressing. The lower 4 bits of the current opcode determine how the
 * ISAR is addressing memory.
 * 00 - 0B : Returns the specified byte directly; ignores ISAR.
 * 0C      : Returns the byte addressed by ISAR.
 * 0D      : Returns the byte addressed by ISAR, increments ISAR.
 * 0E      : Returns the byte addressed by ISAR, decrements ISAR.
 * 0F      : Returns NULL.
 * See Table 6-2.
 **/
static f8_byte *isar(f8_system_t *system)
{
  /* Last 4 bits only */
  u8 opcode = system->dbus.u & B00001111;
  f8_byte *address = NULL;

  if (opcode < 12)
  {
    /* Address scratchpad directly for first 12 bytes */
    address = &f8_main_cpu(system)->scratchpad[opcode];
  }
  else if (opcode != 0x0F)
    address = &f8_main_cpu(system)->scratchpad[ISAR & 0x3F];

  if (opcode == 0x0D)
    ISAR = (ISAR & B00111000) | ((ISAR + 1) & B00000111);
  else if (opcode == 0x0E)
    ISAR = (ISAR & B00111000) | ((ISAR - 1) & B00000111);

  return address;
}

static void update_status(f8_system_t *system)
{
  add(system, &A, 0);
}

/**
 * 00
 * LR A, KU
 */
F8_OP(lr_a_ku)
{
  A = KU;
}

/**
 * 01
 * LR A, KL
 */
F8_OP(lr_a_kl)
{
  A = KL;
}

/**
 * 02
 * LR A, QU
 */
F8_OP(lr_a_qu)
{
  A = QU;
}

/**
 * 03
 * LR A, QL
 */
F8_OP(lr_a_ql)
{
  A = QL;
}

/**
 * 04
 * LR KU, A
 */
F8_OP(lr_ku_a)
{
  KU = A;
}

/**
 * 05
 * LR KL, A
 */
F8_OP(lr_kl_a)
{
  KL = A;
}

/**
 * 06
 * LR QU, A
 */
F8_OP(lr_qu_a)
{
  QU = A;
}

/**
 * 07
 * LR QL, A
 */
F8_OP(lr_ql_a)
{
  QL = A;
}

/*
   08
   LR K, PC1
   Load a word into K from the backup process counter.
*/
F8_OP(lr_k_pc1)
{
#if PF_ROMC
  romc07(system);
  KU = system->dbus;
  romc0b(system);
  KL = system->dbus;
#else
  PC1 = read_16(&KU);
  system->cycles += CYCLE_LONG * 2;
#endif
}

/*
   09
   LR PC1, K
   Load a word into the backup process counter from K.
*/
F8_OP(lr_pc1_k)
{
#if PF_ROMC
  system->dbus = KU;
  romc15(system);
  system->dbus = KL;
  romc18(system);
#else
  PC1 = read_16(&KU);
  system->cycles += CYCLE_LONG * 2;
#endif
}

/* 0A */
F8_OP(lr_a_isar)
{
  A.u = ISAR & B00111111;
}

/* 0B */
F8_OP(lr_isar_a)
{
  ISAR = A.u & B00111111;
}

/* 0C */
/* PK: Loads process counter into backup, K into process counter */
F8_OP(pk)
{
#if PF_ROMC
  system->dbus = KL;
  romc12(system);
  system->dbus = KU;
  romc14(system);
#endif
}

/* 0D */
F8_OP(lr_pc0_q)
{
#if PF_ROMC
  system->dbus = QL;
  romc17(system);
  system->dbus = QU;
  romc14(system);
#endif
}

/* 0E */
F8_OP(lr_q_dc0)
{
#if PF_ROMC
  romc06(system);
  QU = system->dbus;
  romc09(system);
  QL = system->dbus;
#endif
}

/* 0F */
F8_OP(lr_dc0_q)
{
#if PF_ROMC
  system->dbus = QU;
  romc16(system);
  system->dbus = QL;
  romc19(system);
#endif
}

/* 10 */
F8_OP(lr_dc0_h)
{
#if PF_ROMC
  system->dbus = HU;
  romc16(system);
  system->dbus = HL;
  romc19(system);
#endif
}

/* 11 */
F8_OP(lr_h_dc0)
{
#if PF_ROMC
  romc06(system);
  HU = system->dbus;
  romc09(system);
  HL = system->dbus;
#endif
}

static void shift(f8_system_t *system, u8 right, u8 amount)
{
  A.u = right ? A.u >> amount : A.u << amount;
  set_status(system, STATUS_OVERFLOW, FALSE);
  set_status(system, STATUS_ZERO,     A.u == 0);
  set_status(system, STATUS_CARRY,    FALSE);
  set_status(system, STATUS_SIGN,     A.s >= 0);
}

/**
 * 12
 * SR 1 - Shift Right 1
 * The contents of the accumulator are shifted right either one or four bit
 * positions, depending on the value of the SR instruction operand.
 */
F8_OP(sr_a)
{
  shift(system, TRUE, 1);
}

/**
 * 13
 * SL 1 - Shift Left 1
 * The contents of the accumulator are shifted left either one or four bit
 * positions, depending on the value of the SL instruction operand.
 */
F8_OP(sl_a)
{
  shift(system, FALSE, 1);
}

/* 14 */
F8_OP(sr_a_4)
{
  shift(system, TRUE, 4);
}

/* 15 */
F8_OP(sl_a_4)
{
  shift(system, FALSE, 4);
}

/**
 * 16
 * LM - Load Accumulator from Memory
 * The contents of the memory byte addressed by the DC0 register are loaded
 * into the accumulator. The contents of the DC0 registers are incremented
 * as a result of the LM instruction execution.
 */
F8_OP(lm)
{
#if PF_ROMC
  romc02(system);
  A = system->dbus;
#else
  f8_read(system, &A, DC0, sizeof(A));
  DC0++;
#endif
}

/**
 * 17
 * ST - Store to Memory
 * The contents of the accumulator are stored in the memory location addressed
 * by the Data Counter (DC0) registers.
 * The DC registers' contents are incremented as a result of the instruction
 * execution.
 */
F8_OP(st)
{
#if PF_ROMC
  system->dbus = A;
  romc05(system);
#else
  f8_write(system, DC0, &A, sizeof(A));
  DC0++;
#endif
}

/**
 * 18
 * COM - Complement
 * The accumulator is loaded with its one's complement.
 */
F8_OP(com)
{
  A.u ^= 0xFF;
  update_status(system);
}

/**
 * 19
 * LNK - Link Carry to the Accumulator
 * The carry bit is binary added to the least significant bit of the
 * accumulator. The result is stored in the accumulator.
 */
F8_OP(lnk)
{
  add(system, &A, get_status(system, STATUS_CARRY));
}

/**
 * 1A
 * DI - Disable Interrupt
 * The interrupt control bit, ICB, is reset; no interrupt requests will be
 * acknowledged by the 3850 CPU.
 */
F8_OP(di)
{
#if PF_ROMC
  romc1c(system);
#else
  system->cycles += CYCLE_LONG;
#endif
  set_status(system, STATUS_INTERRUPTS, FALSE);
}

/**
 * 1B
 * EI - Enable Interrupt
 * The interrupt control bit is set. Interrupt requests will now be
 * acknowledged by the CPU.
 */
F8_OP(ei)
{
#if PF_ROMC
   romc1c(system);
#else
   system->cycles += CYCLE_LONG;
#endif
   set_status(system, STATUS_INTERRUPTS, TRUE);
}

/**
 * 1C
 * POP - Return from Subroutine
 * The contents of the Stack Registers (PC1) are transferred to the
 * Program Counter Registers (PC0).
 */
F8_OP(pop)
{
#if PF_ROMC
  romc04(system);
#else
  PC0 = PC1 - 1;
  system->cycles += CYCLE_SHORT;
#endif
}

/* 1D */
F8_OP(lr_w_j)
{
#if PF_ROMC
  romc1c(system);
#else
  system->cycles += CYCLE_LONG;
#endif
  W = J.u & B00011111;
}

/* 1E */
F8_OP(lr_j_w)
{
  J.u = W & B00011111;
}

/**
 * 1F
 * INC - Increment Accumulator
 * The content of the accumulator is increased by one binary count.
 */
F8_OP(inc)
{
  add(system, &A, 1);
  system->cycles += CYCLE_SHORT;
}

/**
 * 20
 * LI - Load Immediate
 * The value provided by the operand of the LI instruction is loaded into the
 * accumulator.
 */
F8_OP(li)
{
#if PF_ROMC
  romc03(system);
  A = system->dbus;
#else
  A = get_immediate(system);
#endif
}

/**
 * 21
 * NI - AND Immediate
 * An 8-bit value provided by the operand of the NI instruction is ANDed with
 * the contents of the accumulator. The results are stored in the accumulator.
 */
F8_OP(ni)
{
#if PF_ROMC
  romc03(system);
  A.u &= system->dbus.u;
#else
  A.u &= get_immediate(system);
#endif
  add(system, &A, 0);
}

/**
 * 22
 * OI - OR Immediate
 * An 8-bit value provided by the operand of the I/O instruction is ORed with
 * the contents of the accumulator. The results are stored in the accumulator.
 */
F8_OP(oi)
{
#if PF_ROMC
  romc03(system);
  A.u |= system->dbus.u;
#else
  A.u |= get_immediate(system);
#endif
  add(system, &A, 0);
}

/**
 * 23
 * XI - Exclusive-OR Immediate
 * The contents of the 8-bit value provided by the operand of the XI
 * instruction are EXCLUSIVE-ORed with the contents of the accumulator.
 * The results are stored in the accumulator.
 */
F8_OP(xi)
{
#if PF_ROMC
  romc03(system);
  A.u ^= system->dbus.u;
#else
  A.u ^= get_immediate(system);
#endif
  add(system, &A, 0);
}

/**
 * 24
 * AI - Add Immediate to Accumulator
 * The 8-bit (two hexadecimal digit) value provided by the instruction operand
 * is added to the current contents of the accumulator. Binary addition is
 * performed.
 */
F8_OP(ai)
{
#if PF_ROMC
  romc03(system);
  add(system, &A, system->dbus.u);
#else
  add(system, &A, get_immediate(system));
#endif
}

/**
 * 25
 * CI - Compare Immediate
 * The contents of the accumulator are subtracted from the operand of the CI
 * instruction. The result is not saved but the status bits are set or reset to
 * reflect the results of the operation.
 */
F8_OP(ci)
{
  f8_byte immediate;

  romc03(system);
  immediate = system->dbus;
  add(system, &immediate, (~A.u & 0xFF) + 1);
}

/**
 * 26
 * IN - Input Long Address
 * The data input to the I/O port specified by the operand of the IN
 * instruction is stored in the accumulator.
 * The I/O port addresses 4 through 255 may be addressed by the IN
 * instruction.
 */
F8_OP(in)
{
  io_t *io;

#if PF_ROMC
  /* Apparently only 128 devices can be hooked up, don't know why */
  romc03(system);
  W = 0; /* todo: why? */
  io = &system->io_ports[system->dbus.u & B01111111];
#else
  io = &system->io_ports[get_immediate(system) & 0xFF];
#endif

  if (io->func_in)
    io->func_in(io->device_in, &io->data);

#if PF_ROMC
  romc1b(system);
#else
  system->cycles += CYCLE_LONG;
#endif

  A = io->data;
  add(system, &A, 0);
}

/**
 * 27
 * OUT - Output Long Address
 * The I/O port addressed by the operand of the OUT instruction is loaded
 * with the contents of the accumulator.
 * I/O ports with addresses from 4 through 255 may be accessed with the
 * OUT instruction.
 * @todo What happens when 0-3 are addressed?
 */
F8_OP(out)
{
  u8 address;
  io_t *io;

#if PF_ROMC
  romc03(system);
  address = system->dbus.u;
#else
  address = get_immediate(system) & 0xFF;
#endif

#if PF_SAFETY
  if (address < 4)
    return;
#endif

  io = &system->io_ports[address];
  if (io->func_out)
    io->func_out(io->device_out, &io->data, A);
  else
    io->data = A;
}

/**
 * 28
 * PI - Call to Subroutine Immediate
 * The contents of the Program Counters are stored in the Stack Registers, PC1,
 * then the then the 16-bit address contained in the operand of the PI
 * instruction is loaded into the Program Counters. The accumulator is used as
 * a temporary storage register during transfer of the most significant byte of
 * the address. Previous accumulator results will be altered.
 */
F8_OP(pi)
{
  romc03(system);
  A = system->dbus;
  romc0d(system);
  romc0c(system);
  system->dbus = A;
  romc14(system);
/*
  A = get_immediate(system);
  PC1 = PC0 + 2;//hack
  PC0 = (get_immediate(system)) & 0xFF;
  PC0 |= (A << 8);
  PC0--; //hack
*/
}

/**
 * 29
 * JMP - Branch Immediate
 * As the result of a JMP instruction execution, a branch to the memory
 * location addressed by the second and third bytes of the instruction
 * occurs. The second byte contains the high order eight bits of the
 * memory address; the third byte contains the low order eight bits of
 * the memory address.
 * The accumulator is used to temporarily store the most significant byte
 * of the memory address; therefore, after the JMP instruction is executed,
 * the initial contents of the accumulator are lost.
 */
F8_OP(jmp)
{
#if PF_ROMC
  romc03(system);
  A = system->dbus;
  romc0c(system);
  system->dbus = A;
  romc14(system);
#else
  A = get_immediate(system);
  PC0 = get_immediate(system);
#endif
}

/**
 * 2A
 * DCI - Load DC Immediate
 * The DCI instruction is a three-byte instruction. The contents of the second
 * byte replace the high order byte of the DC0 registers; the contents of the
 * third byte replace the low order byte of the DC0 registers.
 */
F8_OP(dci)
{
#if PF_ROMC
  romc11(system);
  romc03(system);
  romc0e(system);
  romc03(system);
#else
  DC0 = get_immediate(system) << 8;
  DC0 |= get_immediate(system);
#endif
}

/**
 * 2B
 * NOP - No Operation
 * No function is performed.
 */
F8_OP(nop)
{
  /* This does nothing to intentionally silence a warning */
  (void)system;
}

/**
 * 2C
 * XDC - Exchange Data Counters
 * Execution of the instruction XDC causes the contents of the auxiliary data
 * counter registers (DC1) to be exchanged with the contents of the data
 * counter registers (DC0).
 * This instruction is only significant when a 3852 or 3853 Memory Interface
 * device is part of the system configuration.
 * @todo "The PSUs will have DC0 unaltered." ...?
 */
F8_OP(xdc)
{
#if PF_ROMC
  romc1d(system);
#else
  unsigned temp = DC0;

  DC0 = DC1;
  DC1 = temp;
  system->cycles += CYCLE_SHORT;
#endif
}

/**
 * 30 - 3F
 * DS - Decrease Scratchpad Content
 * The content of the scratchpad register addressed by the operand (Sreg) is
 * decremented by one binary count. The decrement is performed by adding H'FF'
 * to the scratchpad register.
 * @todo Macro and unroll this
 */
F8_OP(ds)
{
  f8_byte *address = isar(system);

  if (address != NULL)
    add(system, address, 0xFF);
}

/**
 * 40 - 4F
 * LR A, r - Load Register
 */
F8_OP(lr_a_r)
{
  f8_byte *address = isar(system);

  if (address)
    A = *address;
}

/**
 * 50 - 5F
 * LR r, A - Load Register
 */
F8_OP(lr_r_a)
{
  f8_byte *address = isar(system);

  if (address)
    *address = A;
}

/**
 * 60 - 67
 * LISU - Load Upper Octal Digit of ISAR
 * A 3-bit value provided by the LISU instruction operand is loaded into the
 * three most significant bits of the ISAR. The three least significant bits
 * of the ISAR are not altered.
 */
F8_OP(lisu)
{
  unsigned immediate = system->dbus.u & B00000111;

  /* Mask to lower 3 bits, load new upper */
  ISAR &= B00000111;
  ISAR |= immediate << 3;
}

/**
 * 68 - 6F
 * LISL - Load Lower Octal Digit of ISAR
 * A 3-bit value provided by the LISL instruction operand is loaded into the
 * three least significant bits of the ISAR. The three most significant bits
 * of the ISAR are not altered.
 */
F8_OP(lisl)
{
  unsigned immediate = system->dbus.u & B00000111;

  /* Mask to upper 3 bits, load new lower */
  ISAR &= B00111000;
  ISAR |= immediate;
}

/**
 * 70
 * CLR - Clear Accumulator
 * The contents of the accumulator are set to zero.
 */

/**
 * 71 - 7F
 * LIS - Load Immediate Short
 * A 4-bit value provided by the LIS instruction operand is loaded into the
 * four least significant bits of the accumulator. The most significant four
 * bits of the accumulator are set to ''0''.
 */

#define F8_OP_LIS(a) F8_OP(lis##a) { A.u = a; }
F8_OP_LIS(0)
F8_OP_LIS(1)
F8_OP_LIS(2)
F8_OP_LIS(3)
F8_OP_LIS(4)
F8_OP_LIS(5)
F8_OP_LIS(6)
F8_OP_LIS(7)
F8_OP_LIS(8)
F8_OP_LIS(9)
F8_OP_LIS(10)
F8_OP_LIS(11)
F8_OP_LIS(12)
F8_OP_LIS(13)
F8_OP_LIS(14)
F8_OP_LIS(15)

#if PF_ROMC
#define F8_OP_BT(a) \
  F8_OP(bt##a) \
  { \
    romc1c(system); \
    if (a & W) \
      romc01(system); \
    else \
      romc03(system); \
  }
#else
#endif

/**
 * 80
 * BT0 - Do Not Branch
 * An effective 3-cycle NO-OP
 */
F8_OP_BT(0)

/**
 * 81
 * BT1 / BP - Branch if Positive
 */
F8_OP_BT(1)

/**
 * 82
 * BT2 / BC - Branch on Carry
 */
F8_OP_BT(2)

/**
 * 83
 * BT3 - Branch if Positive or on Carry
 */
F8_OP_BT(3)

/**
 * 84
 * BT4 / BZ - Branch if Zero
 */
F8_OP_BT(4)

/**
 * 85
 * BT5 - Branch if Positive
 * Same function as BP
 */
F8_OP_BT(5)

/**
 * 86
 * BT6 - Branch if Zero or on Carry
 */
F8_OP_BT(6)

/**
 * BT7 - Branch if Positive or on Carry
 * Same function as BT3
 */
F8_OP_BT(7)

/**
 * 88
 * AM - Add (Binary) Memory to Accumulator
 * The content of the memory location addressed by the DC0 registers is added
 * to the accumulator. The sum is returned in the accumulator. Memory is not
 * altered. Binary addition is performed. The contents of the DC0 registers
 * are incremented by 1.
 */
F8_OP(am)
{
#if PF_ROMC
  romc02(system);
  add(system, &A, system->dbus.s);
#else
  add(system, &A, get_rom(system, DC0));
  DC0++;
#endif
}

/**
 * 89
 * AMD - Add Decimal Accumulator with Memory
 * The accumulator and the memory location addressed by the DC0 registers are
 * assumed to contain two BCD digits. The content of the address memory byte
 * is added to the contents of the accumulator to give a BCD result in the
 * accumulator.
 */
F8_OP(amd)
{
  romc02(system);
  add_bcd(system, &A, system->dbus.s);
}

/**
 * 8A
 * NM - Logical AND from Memory
 * The content of memory addressed by the data counter registers is ANDed with
 * the content of the accumulator. The results are stored in the accumulator.
 * The contents of the data counter registers are incremented.
 */
F8_OP(nm)
{
  romc02(system);
  A.u &= system->dbus.u;
  update_status(system);
}

/**
 * 8B
 * OM - Logical OR from Memory
 * The content of memory byte addressed by...
 * @todo write
 */
F8_OP(om)
{
  romc02(system);
  A.u |= system->dbus.u;
  update_status(system);
}

/**
 * 8C
 * XM - Exclusive OR from Memory
 * @todo write
 */
F8_OP(xm)
{
  romc02(system);
  A.u ^= system->dbus.u;
  update_status(system);
}

/**
 * 8D
 * CM - Compare Memory to Accumulator
 * The CM instruction is the same is the same as the CI instruction except the
 * memory contents addressed by the DC0 registers, instead of an immediate
 * value, are compared to the contents of the accumulator.
 * Memory contents are not altered. Contents of the DC0 registers are
 * incremented.
 */
F8_OP(cm)
{
  f8_byte temp;

  romc02(system);
  temp = system->dbus;
  add(system, &temp, -A.s);
}

/**
 * 8E
 * ADC - Add Accumulator to Data Counter
 * The contents of the accumulator are treated as a signed binary number, and
 * are added to the contents of every DC0 register. The result is stored in the
 * DC0 registers. The accumulator contents do not change.
 * No status bits are modified.
 */
F8_OP(adc)
{
#if PF_ROMC
  system->dbus = A;
  romc0a(system);
#else
  DC0 += A;
#endif
}

/**
 * 8F
 * BR7 - Branch on ISAR
 * Branch will occur if any of the low 3 bits of ISAR are reset.
 */
F8_OP(br7)
{
#if PF_ROMC
  romc03(system);
  if ((ISAR & B00000111) != B00000111)
    romc01(system);
#else
  if ((ISAR & 7) != 7)
    PC0 += (i8)next_op(system);
  else
    PC0++;
#endif
}

/**
 * 90 - 9F
 * BF - Branch on False
 */
F8_OP(bf)
{
#if PF_ROMC
  /* Wait? */
  romc1c(system);
  if (!((system->dbus.u & B00001111) & W))
    romc01(system);
  else
    romc03(system);
#endif
}

/**
 * A0 - AF
 * INS - Input Short Address
 * Data input to the I/O port specified by the operand of the INS instruction
 * is loaded into the accumulator. An I/O port with an address within the
 * range 0 through 15 may be accessed by this instruction.
 * If an I/O port or pin is being used for both input and output, the port or
 * pin previously used for output must be cleared before it can be used to
 * input data.
 */
F8_OP(ins)
{
#if PF_ROMC
  io_t *io = &system->io_ports[system->dbus.u & B00001111];

  if (io->func_in)
    io->func_in(io->device_in, &io->data);

  A = io->data;
  add(system, &A, 0);
#else
  if (port == 0)
    system->io[0] = get_input(0);
  else if (system->io[1] == 0 && port == 1)
    system->io[1] = get_input(1);
  else if (system->io[4] == 0 && port == 4)
    system->io[4] = get_input(4);*/
#endif
}

/**
 * B0 - BF
 * OUTS - Output Short Address
 * The I/O port addressed by the operand of the OUTS instruction object code
 * is loaded with the contents of the accumulator. I/O ports with addresses
 * from 0 to 15 may be accessed by this instruction. The I/O port addresses
 * are defined in Table 6-6. Outs 0 or 1 is CPU port only.
 * No status bits are modified.
 */
F8_OP(outs)
{
#if PF_ROMC
  io_t *io = &system->io_ports[system->dbus.u & B00001111];

  romc1c(system);

  if (io->func_out)
    io->func_out(io->device_out, &io->data, A);
  else
    io->data = A;
#else
   /*u8 port = current_op(system) & 0x0F;
   u8 temp = system->io[port];

   system->io[port] = A;
    Hack for testing, remove this
   if (port == 0 && (temp == 0x60) && (A == 0x40 || A == 0x50))
   {
      u8 x, y;

      x = (system->io[4] ^ 0xFF) & 0x7F;
      y = (system->io[5] ^ 0xFF) & 0x3F;

      vram_write(system->vram, x, y, (system->io[1] & 0xC0) >> 6);
   }
   else if (port == 5)
      sound_push_back(system->io[5] >> 6, system->cycles, system->total_cycles);
   */
#endif
}

/**
 * C0 - CF
 * AS - Binary Addition, Scratchpad Memory to Accumulator
 * The content of the scratchpad register referenced by the instruction
 * operand (Sreg) is added to the accumulator using binary addition. The
 * result of the binary addition is stored in the accumulator. The scratchpad
 * register contents remain unchanged. Depending on the value of Sreg, ISAR
 * may be unaltered, incremented, or decremented.
 */
F8_OP(as)
{
  f8_byte *reg = isar(system);

  if (reg != NULL)
    add(system, &A, reg->u);
}

/*
   D0 - DF
   ASD (Add Source Decimal)
   Add a register to the accumulator as binary-coded decimal.
*/
F8_OP(asd)
{
  f8_byte *reg = isar(system);

  romc1c(system);

  if (reg != NULL)
    add_bcd(system, &A, reg->u);
}

/*
   E0 - EF
   XS (eXclusive or Source)
   Logical XOR a register into the accumulator.
*/
F8_OP(xs)
{
  f8_byte *address = isar(system);

  if (address != NULL)
  {
    A.u ^= address->u;
    update_status(system);
  }
}

/*
   F0 - FF
   NS (aNd Source)
   Logical AND a register into the accumulator.
*/
F8_OP(ns)
{
  f8_byte *address = isar(system);

  if (address != NULL)
  {
    A.u &= address->u;
    update_status(system);
  }
}

/**
 * 2D - 2F
 * Invalid / Undefined opcode
 */
F8_OP(invalid)
{
  nop(system);
}

/*
typedef void F8_OP_T(f8_system_t*);
static F8_OP_T *guh[256] =
{
  lr_a_ku,    lr_a_kl,    lr_a_qu,    lr_a_ql,
  lr_ku_a,    lr_kl_a,    lr_qu_a,    lr_ql_a,
  lr_k_pc1,   lr_pc1_k,   lr_a_isar,  lr_isar_a,
  pk,         lr_pc0_q,   lr_q_dc0,   lr_dc0_q,

  lr_dc0_h,   lr_h_dc0,   sr_a,       sl_a,
  sr_a_4,     sl_a_4,     lm,         st,
  com,        lnk,        di,         ei,
  pop,        lr_w_j,     lr_j_w,     inc,

  li,         ni,         oi,         xi,
  ai,         ci,         in,         out,
  pi,         jmp,        dci,        nop,
  xdc,        invalid,    invalid,    invalid,

  ds0, ds1, ds2, ds3, ds4, ds5, ds6, ds7,
  ds8, ds9, ds10, ds11, ds12, ds13, ds14, ds15,
};
*/

u8 pressf_init(f8_system_t *system)
{
   u32 i = 0;

   for (i = 0x00; i < 0x04; i++)
   {
      operations[0x00 + i] = lr_a_dpchr;
      operations[0x04 + i] = lr_dpchr_a;
   }
   operations[0x08] = lr_k_pc1;
   operations[0x09] = lr_pc1_k;
   operations[0x0A] = lr_a_isar;
   operations[0x0B] = lr_isar_a;
   operations[0x0C] = pk;
   operations[0x0D] = lr_pc0_q;
   operations[0x0E] = lr_q_dc0;
   operations[0x0F] = lr_dc0_q;

   for (i = 0x00; i < 0x08; i++)
   {
      operations[0x60 + i] = lisu;
      operations[0x68 + i] = lisl;
   }
   for (i = 0x00; i < 0x10; i++)
   {
      operations[0x30 + i] = ds;
      operations[0x40 + i] = lr_a_r;
      operations[0x50 + i] = lr_r_a;
      operations[0x90 + i] = bf;
      operations[0xA0 + i] = ins;
      operations[0xB0 + i] = outs;
      operations[0xC0 + i] = as;
      operations[0xD0 + i] = asd;
      operations[0xE0 + i] = xs;
      operations[0xF0 + i] = ns;
   }
   operations[0x10] = lr_dc0_h;
   operations[0x11] = lr_h_dc0;
   operations[0x12] = sr_a;
   operations[0x13] = sl_a;
   operations[0x14] = sr_a_4;
   operations[0x15] = sl_a_4;
   operations[0x16] = lm;
   operations[0x17] = st;
   operations[0x18] = com;
   operations[0x19] = lnk;
   operations[0x1A] = di;
   operations[0x1B] = ei;
   operations[0x1C] = pop;
   operations[0x1D] = lr_w_j;
   operations[0x1E] = lr_j_w;
   operations[0x1F] = inc;

   operations[0x20] = li;
   operations[0x21] = ni;
   operations[0x22] = oi;
   operations[0x23] = xi;
   operations[0x24] = ai;
   operations[0x25] = ci;
   operations[0x26] = in;
   operations[0x27] = out;
   operations[0x28] = pi;
   operations[0x29] = jmp;
   operations[0x2A] = dci;
   operations[0x2B] = nop;
   operations[0x2C] = xdc;
   operations[0x2D] = invalid;
   operations[0x2E] = invalid;
   operations[0x2F] = invalid;

   operations[0x70] = lis0;
   operations[0x71] = lis1;
   operations[0x72] = lis2;
   operations[0x73] = lis3;
   operations[0x74] = lis4;
   operations[0x75] = lis5;
   operations[0x76] = lis6;
   operations[0x77] = lis7;
   operations[0x78] = lis8;
   operations[0x79] = lis9;
   operations[0x7A] = lis10;
   operations[0x7B] = lis11;
   operations[0x7C] = lis12;
   operations[0x7D] = lis13;
   operations[0x7E] = lis14;
   operations[0x7F] = lis15;

   operations[0x80] = bt0;
   operations[0x81] = bt1;
   operations[0x82] = bt2;
   operations[0x83] = bt3;
   operations[0x84] = bt4;
   operations[0x85] = bt5;
   operations[0x86] = bt6;
   operations[0x87] = bt7;

   operations[0x88] = am;
   operations[0x89] = amd;
   operations[0x8A] = nm;
   operations[0x8B] = om;
   operations[0x8C] = xm;
   operations[0x8D] = cm;
   operations[0x8E] = adc;
   operations[0x8F] = br7;

   operations[0x3F] = invalid;
   operations[0x4F] = invalid;
   operations[0x5F] = invalid;
   operations[0xCF] = invalid;
   operations[0xDF] = invalid;
   operations[0xEF] = invalid;
   operations[0xFF] = invalid;

   system->total_cycles = 30000;

   f3850_init(&system->f8devices[0]);

   return TRUE;
}

/**
 * Has the F8 system execute one instruction.
 **/
void pressf_step(f8_system_t *system)
{
#if PF_HAVE_HLE_BIOS
  void (*hle_func)() = hle_get_func_from_addr(PC0);
#endif

#if PF_DEBUGGER
  if (debug_should_break(PC0))
    return;
#endif

#if PF_ROMC
  romc00(system);
#else
  PC0++;
#endif
#if PF_HAVE_HLE_BIOS
  if (hle_func)
  {
    hle_func(system);
    return;
  }
#endif
  operations[system->dbus.u](system);
}

u8 pressf_run(f8_system_t *system)
{
#if PRESS_F_SAFETY
  if (!system)
    return FALSE;
#endif

  system->cycles -= system->total_cycles;
  if (system->cycles < 0)
    system->cycles = 0;
  do
  {
    pressf_step(system);
  } while (system->total_cycles > system->cycles);

  return TRUE;
}

void pressf_reset(f8_system_t *system)
{
  if (system)
  {
    romc08(system);
  }
}
