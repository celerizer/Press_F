#ifndef PRESSF_EMU_C
#define PRESSF_EMU_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emu.h"

#define A    system->c3850.accumulator
#define ISAR system->c3850.isar
#define W    system->c3850.status_register

#define J    system->c3850.scratchpad[0x09]
#define HU   system->c3850.scratchpad[0x0A]
#define HL   system->c3850.scratchpad[0x0B]
#define KU   system->c3850.scratchpad[0x0C]
#define KL   system->c3850.scratchpad[0x0D]
#define QU   system->c3850.scratchpad[0x0E]
#define QL   system->c3850.scratchpad[0x0F]

#define DC0  system->dc0
#define DC1  system->dc1
#define PC0  system->pc0
#define PC1  system->pc1

#define F8_OP(a) void a(channelf_t *system)

static void (*operations[256])(channelf_t *system);
static unsigned long instruction_count;

u8 bcd(u8 value)
{
   return (((value >> 4) & 0x0F) * 10) + (value & 0x0F);
}

u16 read_16(void *src)
{
   u16 value;

#ifndef BIG_ENDIAN
   value = (((u8*)src)[0] << 8) + ((u8*)src)[1];
#else
   memcpy(&value, src, 2);
#endif

   return value;
}

void write_16(void *dest, u16 src)
{
#ifndef BIG_ENDIAN
   ((u8*)dest)[0] = src >> 8;
   ((u8*)dest)[1] = src & 0xFF;
#else
   memcpy(dest, &src, 2);
#endif
}

u8 current_op(channelf_t *system)
{
   return system->rom[PC0];
}

u8 next_op(channelf_t *system)
{
   return system->rom[PC0 + 1];
}

u8* isar(channelf_t *system)
{
   u8 opcode = current_op(system);
   u8 *address = NULL;

   /* Last 4 bits only */
   opcode &= 0x0F;
   if (opcode <= 0x0B)
   {
      /* Address scratchpad directly for first 12 bytes */
      address = &system->c3850.scratchpad[opcode];
   }
   else if (opcode != 0x0F)
      address = &system->c3850.scratchpad[ISAR];
   if (opcode == 0x0D)
      ISAR++;
   else if (opcode == 0x0E)
      ISAR--;

   return address;
}

u8* dpchr(channelf_t *system)
{
   u8 opcode = current_op(system);

   /* Last 2 bits only */
   opcode &= 0x03;
   switch (opcode)
   {
   case 0:
      return &KU;
   case 1:
      return &KL;
   case 2:
      return &QU;
   case 3:
      return &QL;
   }

   return NULL;
}

u8 get_rom(channelf_t *system, const u16 address)
{
   return system->rom[address];
}

u8 get_status(channelf_t *system, const u8 flag)
{
   return (W & flag) != 0;
}

void set_status(channelf_t *system, const u8 flag, u8 enable)
{
   W = enable ? W | flag : W & ~flag;
}

void lr(u8 *dest, u8 *src)
{
   *dest = *src;
}

/* 
   00 - 03
   LR A, DPCHR
   Load a byte from either K or Q into the accumulator (see dpchr()).
*/
F8_OP(lr_a_dpchr)
{
   lr(&A, dpchr(system));
}

/* 
   04 - 07
   LR DPCHR, A
   Load the accumulator into a byte from either K or Q (see dpchr()).
*/
F8_OP(lr_dpchr_a)
{
   lr(dpchr(system), &A);
}

/*
   08
   LR K, PC1
   Load a word into K from the backup process counter.
*/
F8_OP(lr_k_pc1)
{
   write_16(&KU, PC1);
}

/*
   09
   LR PC1, K
   Load a word from the backup process counter into K.
*/
F8_OP(lr_pc1_k)
{
   write_16(&PC1, read_16(&KU));
}

/* 0A */
F8_OP(lr_a_isar)
{
   lr(&A, &ISAR);
}

/* 0B */
F8_OP(lr_isar_a)
{
   lr(&ISAR, &A);
}

/* 0C */
/* PK: Loads process counter into backup, K into process counter */
F8_OP(pk)
{
   PC1 = PC0;
   PC0 = read_16(&KU);
}

/* 0D */
F8_OP(lr_pc0_q)
{
   write_16(&PC0, read_16(&QU));
}

/* 0E */
F8_OP(lr_dc0_q)
{
   write_16(&DC0, read_16(&QU));
}

/* 0F */
F8_OP(lr_q_dc0)
{
   write_16(&QU, DC0);
}

/* 10 */
F8_OP(lr_h_dc0)
{
   write_16(&DC0, read_16(&HU));
}

/* 11 */
F8_OP(lr_dc0_h)
{
   write_16(&HU, DC0);
}

/* ======================================================= F8 shift instructions */
void shift(channelf_t *system, u8 right, u8 amount)
{
   A = right ? A << amount : A >> amount;
   set_status(system, STATUS_OVERFLOW, FALSE);
   set_status(system, STATUS_ZERO,     A == 0 ? TRUE : FALSE);
   set_status(system, STATUS_CARRY,    FALSE);
   set_status(system, STATUS_POSITIVE, A & 0x80 ? TRUE : FALSE);
}

/* 12 */
F8_OP(sr_a)
{
   shift(system, TRUE, 1);
}

/* 13 */
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

/* 16 */
F8_OP(lm)
{
   u8 rom_data = get_rom(system, DC0);

   lr(&A, &rom_data);
   DC0++;
}

/* 17 */
F8_OP(st)
{
   /* Does DC0U zero-fill? */
   DC0 = A + 1;
}

/* 18 */
F8_OP(com)
{
   A ^= A;
   set_status(system, STATUS_OVERFLOW, FALSE);
   set_status(system, STATUS_ZERO,     A == 0 ? TRUE : FALSE);
   set_status(system, STATUS_CARRY,    FALSE);
   set_status(system, STATUS_POSITIVE, A & 0x80 ? TRUE : FALSE);
}

/* 19 */
F8_OP(lnk)
{
   set_status(system, STATUS_OVERFLOW, 0x100 - A > STATUS_CARRY ? TRUE : FALSE);
   set_status(system, STATUS_CARRY,    0x100 - A > STATUS_CARRY ? TRUE : FALSE);
   A += get_status(system, STATUS_CARRY) ? STATUS_CARRY : 0;
   set_status(system, STATUS_ZERO,     A == 0 ? TRUE : FALSE);
   set_status(system, STATUS_POSITIVE, A & 0x80 ? TRUE : FALSE);
}

/* 1A */
F8_OP(di)
{
   set_status(system, STATUS_INTERRUPTS, FALSE);
}

/* 1B */
F8_OP(ei)
{
   set_status(system, STATUS_INTERRUPTS, TRUE);
}

/* 1C */
/* A destroyed? */
F8_OP(pop)
{
   PC0 = PC1;
}

/* 1D */
F8_OP(lr_w_j)
{
   lr(&W, &J);
}

/* 1E */
F8_OP(lr_j_w)
{
   lr(&J, &W);
}

void add(channelf_t *system, u8 *dest, u8 src)
{
   *dest += src;
   set_status(system, STATUS_OVERFLOW, *dest < src  ? TRUE : FALSE);
   set_status(system, STATUS_ZERO,     *dest == 0   ? TRUE : FALSE);
   set_status(system, STATUS_CARRY,    *dest < src  ? TRUE : FALSE);
   set_status(system, STATUS_POSITIVE, *dest & 0x80 ? TRUE : FALSE);
}

/* 1F */
F8_OP(inc)
{
   add(system, &A, 1);
}

/* 20 */
F8_OP(li)
{
   A = next_op(system);
   PC0++;
}

u8 get_immediate(channelf_t *system)
{
   u8 immediate = next_op(system);

   PC0++;

   return immediate;
}

/* Hack? We add 0 to accumulator to set flags */
/* 21 */
F8_OP(ni)
{
   A &= get_immediate(system);
   add(system, &A, 0);
}

/* 22 */
F8_OP(oi)
{
   A |= get_immediate(system);
   add(system, &A, 0);
}

/* 23 */
F8_OP(xi)
{
   A ^= get_immediate(system);
   add(system, &A, 0);
}

/* 24 */
F8_OP(ai)
{
   add(system, &A, get_immediate(system));
}

/* 25 */
/* TODO: Subtract function */
F8_OP(ci)
{
   i8 immediate = (i8)get_immediate(system);

   if (A - immediate == 0)
      set_status(system, STATUS_ZERO, TRUE);
   else
      set_status(system, STATUS_ZERO, FALSE);
}

/* 26 */
/* TODO: Data bus, ports */
F8_OP(in)
{
   exit(0);//add(system, &A, 0);
}

/* 27 */
F8_OP(out)
{
   exit(0);//add(system, &A, 0);
}

/* 28 */
F8_OP(pi)
{
   u8 i = get_immediate(system);

   /* Make this look better */
   A = i;
   PC1 = PC0 + 1;
   PC0 = get_immediate(system);
   PC0 += A * 0x100;
}

/* 29 */
F8_OP(jmp)
{
   u8 i = get_immediate(system);

   /* Make this look better */
   A = i;
   PC0 = get_immediate(system);
   PC0 += A * 0x100;
}

/* 2A */
F8_OP(dci)
{
   write_16(&DC0, read_16(&DC0));
   PC0 += 2;
}

/* 2B */
F8_OP(nop)
{
   /* NOP! */
}

/* 2C */
F8_OP(xdc)
{
   DC0 ^= DC1;
   DC1 ^= DC0;
   DC0 ^= DC1;
}

/* 30 - 3F */
/* DS r: Decrease scratchpad byte addressed by ISAR */
F8_OP(ds)
{
   u8 *address = isar(system);

   if (address != NULL)
      *address = *address - 1;
}

/* 40 - 4F */
F8_OP(lr_a_r)
{
   u8 *address = isar(system);

   if (address != NULL)
      lr(&A, address);
}

/* 50 - 5F */
F8_OP(lr_r_a)
{
   u8 *address = isar(system);

   if (address != NULL)
      lr(address, &A);
}

/* 60 - 67 */
F8_OP(lisu)
{
   u8 immediate = (current_op(system) & 0x07) << 3;

   /* Mask to lower 3 bits, load new upper */
   ISAR &= 0x07;
   ISAR |= immediate;
}

/* 68 - 6F */
F8_OP(lisl)
{
   u8 immediate = current_op(system) & 0x07;

   /* Mask to upper 3 bits, load new lower */
   ISAR &= 0x38;
   ISAR |= immediate;
}

/* 70 */
F8_OP(clr)
{
   A = 0;
}

/* 71 - 7F */
F8_OP(lis)
{
   A = current_op(system) & 0x0F;
}

/* 80 - 87 */
F8_OP(bt_n)
{
   if ((current_op(system) & 0x07) & W)
      PC0 += (i8)next_op(system) + 1;
}

/* 88 */
F8_OP(am)
{
   A = DC0 + A;
   DC0++;
}

/* 89 */
F8_OP(amd)
{

}

/* 8E */
F8_OP(adc)
{
   DC0 += A;
}

/* 90 - 9F */
F8_OP(bf)
{
   if (((current_op(system) & 0x0F) & W) == 0)
      PC0 += (i8)next_op(system) + 1;
}

/* 94 */
F8_OP(bnz)
{
   if (!get_status(system, STATUS_ZERO))
      PC0 += (i8)next_op(system);
   else
      PC0++;
}

/* A0 - AF */
F8_OP(ins)
{
   /*u8 port;

   port = current_op(system) & 0x0F;
   TODO */
}

/* B0 - BF */
F8_OP(outs)
{
   /* TODO */
}

/*
   C0 - CF 
   AS (Add Source)
   Add a register to the accumulator.
*/
F8_OP(as)
{
   u8 *address = isar(system);

   if (address != NULL)
      A += *address;
}

/* 
   D0 - DF 
   ASD (Add Source Decimal)
   Add a register to the accumulator as binary-coded decimal. 
*/
F8_OP(asd)
{
   u8 *address = isar(system);

   if (address != NULL)
      A += bcd(*address);
}

/* 
   E0 - EF
   XS (eXclusive or Source)
   Logical XOR a register into the accumulator.
*/
F8_OP(xs)
{
   u8 *address = isar(system);

   if (address != NULL)
      A ^= *address;
}

/* 
   F0 - FF
   NS (aNd Source)
   Logical AND a register into the accumulator.
*/
F8_OP(ns)
{
   u8 *address = isar(system);

   if (address != NULL)
      A &= *address;
}

/*
   2D - 2F
   Invalid / Undefined opcode
*/
F8_OP(invalid_opcode)
{
   printf("Invalid opcode %02X at %04X\n", current_op(system), PC0);
}

u8 pressf_init(channelf_t *system)
{
   u8 i = 0;

   /* For testing BIOS */
   for (i = 0x00; i < 0x40; i++)
      system->c3850.scratchpad[i] = 0xFF;

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
      operations[0x70 + i] = lis;
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
   operations[0x2D] = invalid_opcode;
   operations[0x2E] = invalid_opcode;
   operations[0x2F] = invalid_opcode;

   operations[0x70] = clr;
   operations[0x89] = amd;
   operations[0x8E] = adc;
   operations[0x94] = bnz;

   return TRUE;
}

void pressf_step(channelf_t *system)
{
   u8 op = current_op(system);

   printf("========\n");
   if (operations[op] != NULL)
   {
      printf("A   : %04X | ISAR: %04X | W   : %04X | \n", A, ISAR, W);
      printf("PC0 : %04X | PC1 : %04X | DC0 : %04X | DC1 : %04X\n", PC0, PC1, DC0, DC1);
      printf("Operation: %02X\n", op);
      printf("--------\n");
      for (u8 i = 0; i < 4; i++)
      {
         printf("| RAM [%02X]: ", i * 0x10);
         for (u8 j = 0; j < 16; j++)
            printf("%02X ", system->c3850.scratchpad[(i * 0x10 + j)]);
         printf("|\n");
      }

      operations[op](system);
      PC0++;
   }
   else
   {
      printf("Unknown or unsupported opcode %02X at %04X\n", op, PC0);
      exit(0);
   }
}

u8 pressf_run(channelf_t *system)
{
   if (!system)
      return FALSE;
   else
   {
      instruction_count = 5000;
      do
      {
         pressf_step(system);
         instruction_count -= 10;
      } while (instruction_count > 0);
   }

   exit(0); //remove
   return TRUE;
}

#endif