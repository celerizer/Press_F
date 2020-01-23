#ifndef PRESSF_EMU_H
#define PRESSF_EMU_H

#include "hw/channelf.h"

#define F8_OP(a) void a(channelf_t *system)

typedef struct opcode_t
{
   u8          length;
   const char *format;
   const char *description;
} opcode_t;

const opcode_t opcodes[] =
{
    { 1, "LR A, Ku",   "Loads register 12 (upper byte of K) into the accumulator." },
    { 1, "LR A, Kl",   "Loads register 13 (lower byte of K) into the accumulator." },
    { 1, "LR A, Qu",   "Loads register 14 (upper byte of Q) into the accumulator." },
    { 1, "LR A, Ql",   "Loads register 15 (lower byte of Q) into the accumulator." },
    { 1, "LR Ku, A",   "Loads the accumulator into register 12 (upper byte of K)." },
    { 1, "LR Kl, A",   "Loads the accumulator into register 13 (lower byte of K)." },
    { 1, "LR Qu, A",   "Loads the accumulator into register 14 (upper byte of Q)." },
    { 1, "LR Ql, A",   "Loads the accumulator into register 15 (lower byte of Q)." },
    { 1, "LR K, P",    "Loads the backup process counter into K (registers 12 and 13)." },
    { 1, "LR P, K",    "Loads K (registers 12 and 13) into the backup process counter." },
    { 1, "LR A, ISAR", "Loads the register referenced by the ISAR into the accumulator."},
    { 1, "LR ISAR, A", "Loads the accumulator into the register referenced by the ISAR."}
};

u8   pressf_init  (channelf_t *system);
void pressf_reset (channelf_t *system);
u8   pressf_run   (channelf_t *system);
void pressf_step  (channelf_t *system);

#endif
