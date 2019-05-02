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

#define DC0U system->dc0_upper
#define DC0L system->dc0_lower
#define DC1U system->dc1_upper
#define DC1L system->dc1_lower
#define PC0U system->pc0_upper
#define PC0L system->pc0_lower
#define PC1U system->pc1_upper
#define PC1L system->pc1_lower

#define DC0  &system->dc0_upper
#define DC1  &system->dc1_upper
#define PC0  &system->pc0_upper
#define PC1  &system->pc1_upper

#define F8_OP(a) void a(channelf_t *system)

static void (*operations[256])(channelf_t *system);
static unsigned long instruction_count;

u16 read_16(u8 *src)
{
   u16 value;

#ifndef BIG_ENDIAN
   value = (src[0] << 8) + src[1];
#else
   memcpy(&value, src, 2);
#endif

   return value;
}

void write_16(u8 *dest, u16 src)
{
#ifndef BIG_ENDIAN
   dest[0] = src >> 8;
   dest[1] = src & 0xFF;
#else
   memcpy(dest, &src, 2);
#endif
}

u8 current_op(channelf_t *system)
{
   return system->rom[read_16(PC0)];
}

u8 next_op(channelf_t *system)
{
   return system->rom[read_16(&PC0U) + 1];
}

u8* isar(channelf_t *system)
{
   u8 opcode = current_op(system);
   u8 *address = NULL;

   /* Last 4 bits only */
   opcode &= 0x0F;
   if (opcode <= 0x0B)
      /* Address scratchpad directly for first 12 bytes */
      address = &system->c3850.scratchpad[opcode];
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
   W = enable ? W || flag : W & ~flag;
}

void lr(u8 *dest, u8 *src)
{
   *dest = *src;
}

/* 00 - 03 */
F8_OP(lr_a_dpchr)
{
   lr(&A, dpchr(system));
}

/* 04 - 07 */
void lr_dpchr_a(channelf_t *system)
{
   lr(dpchr(system), &A);
}

/* 08 */
void lr_k_pc1(channelf_t *system)
{
   lr(&KU, &PC1U);
   lr(&KL, &PC1L);
}

/* 09 */
void lr_pc1_k(channelf_t *system)
{
   lr(&PC1U, &KU);
   lr(&PC1L, &KL);
}

/* 0A */
void lr_a_isar(channelf_t *system)
{
   lr(&A, &ISAR);
}

/* 0B */
void lr_isar_a(channelf_t *system)
{
   lr(&ISAR, &A);
}

/* 0C */
void pk(channelf_t *system)
{
   lr(&PC1U, &PC0U);
   lr(&PC1L, &PC0L);
   lr(&PC0U, &QU);
   lr(&PC0L, &QL);
}

/* 0D */
F8_OP(lr_pc0_q)
{
   lr(&PC0U, &QU);
   lr(&PC0L, &QL);
}

/* 0E */
F8_OP(lr_dc0_q)
{
   lr(&DC0U, &QU);
   lr(&DC0L, &QL);
}

/* 0F */
F8_OP(lr_q_dc0)
{
   lr(&QU, &DC0U);
   lr(&QL, &DC0L);
}

/* 10 */
F8_OP(lr_h_dc0)
{
   lr(&HU, &DC0U);
   lr(&HL, &DC0L);
}

/* 11 */
F8_OP(lr_dc0_h)
{
   lr(&DC0U, &HU);
   lr(&DC0L, &HL);
}

/* 12 */
F8_OP(sr_a)
{
   A <<= 1;
}

/* 13 */
F8_OP(sl_a)
{
   A >>= 1;
}

/* 14 */
F8_OP(sr_a_4)
{
   A <<= 4;
}

/* 15 */
F8_OP(sl_a_4)
{
   A >>= 4;
}

/* 16 */
F8_OP(lm)
{
   u8 rom_data = get_rom(system, read_16(&DC0U));

   lr(&A, &rom_data);
   write_16(&DC0U, read_16(&DC0U) + 1);
}

/* 17 */
F8_OP(st)
{
   /* Does DC0U zero-fill? */
   DC0L = A; 
   write_16(&DC0U, read_16(&DC0U) + 1);
}

/* 18 */
F8_OP(com)
{
   A ^= A;
}

/* 19 */
F8_OP(lnk)
{
   A += get_status(system, STATUS_CARRY) ? STATUS_CARRY : 0;
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
F8_OP(pop)
{
   /* A destroyed? */
   write_16(&PC0U, read_16(&PC1U));
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

/* 1F */
F8_OP(inc)
{
   A++;
}

/* 25 */
F8_OP(ci)
{

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
   u8 immediate = (current_op(system) & 0x07) << 5;

   /* Mask to least significant 5 bits */
   system->c3850.isar &= 0x1F;
   system->c3850.isar |= immediate;
}

/* 68 - 6F */
F8_OP(lisl)
{
   u8 immediate = current_op(system) & 0x07;

   /* Mask to most significant 5 bits */
   ISAR &= 0xF8;
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
      write_16(&PC0U, read_16(&PC0U) + next_op(system) + 1);
}

/* 88 */
F8_OP(am)
{
   A = read_16(&DC0U) + A;
   write_16(&DC0U, read_16(&DC0U) + 1);
}

/* 89 */
F8_OP(amd)
{

}

/* 8E */
F8_OP(adc)
{
   write_16(&DC0U, read_16(&DC0U) + A);
}

/* 90 - 9F */
F8_OP(bf)
{
   if (((current_op(system) & 0x07) & W) == 0)
      write_16(&PC0U, read_16(&PC0U) + next_op(system) + 1);
}

/* A0 - AF */
F8_OP(ins)
{
   u8 port;

   port = current_op(system) & 0x0F;
   /* TODO */
}

u8 pressf_init()
{
   u8 i = 0;

   for (i = 0; i < 4; i++)
   {
      operations[0x00 + i] = lr_a_dpchr;
      operations[0x04 + i] = lr_dpchr_a;
   }
   for (i = 0; i < 16; i++)
   {
      operations[0x50 + i] = lr_r_a;
      operations[0x70 + i] = lis;
      operations[0x90 + i] = bf;
      operations[0xA0 + i] = ins;
   }
   operations[0x0A] = lr_a_isar;
   operations[0x0B] = lr_isar_a;
   operations[0x1F] = inc;
   operations[0x70] = clr;
   operations[0x89] = amd;
   operations[0x8E] = adc;

   return TRUE;
}

void pressf_step(channelf_t *system)
{
   u8 op = current_op(system);

   printf("========\n");
   if (operations[op] != NULL)
   {
      operations[op](system);
      printf("PC0: %04X | PC1: %04X | DC0: %04X | DC1: %04X\n", read_16(PC0), read_16(PC1), read_16(DC0), read_16(DC1));
      printf("Operation: %02X\n", op);
      printf("--------\n");
      for (u8 i = 0; i < 4; i++)
      {
         printf("| RAM [%02X]: ", i * 0x10);
         for (u8 j = 0; j < 16; j++)
            printf("%02X ", system->c3850.scratchpad[(i * 0x10 + j)]);
         printf("|\n");
      }
      write_16(PC0, read_16(PC0) + 1);
   }
   else
   {
      printf("Unknown or unsupported opcode %02X at %04X\n", op, read_16(PC0));
      exit(0);
   }
}

u8 pressf_run(channelf_t *system)
{
   if (!system)
      return FALSE;
   else
   {
      instruction_count = 2000000;
      do
      {
         pressf_step(system);
         instruction_count -= 10;
      } while (instruction_count > 0);
   }

   return TRUE;
}

#endif