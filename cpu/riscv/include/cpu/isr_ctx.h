
// SPDX-License-Identifier: MIT

#pragma once

#include "cpu/regs.h"

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef struct sched_thread_t sched_thread_t;
#endif

#ifdef __ASSEMBLER__

#define STRUCT_BEGIN(structname)
#define STRUCT_FIELD_WORD(structname, name, offset)               .equ structname##_##name, offset
#define STRUCT_FIELD_PTR(structname, type, name, offset)          .equ structname##_##name, offset
#define STRUCT_FIELD_STRUCT(structname, type, name, offset, size) .equ structname##_##name, offset
#define STRUCT_END(structname)

#else

#define STRUCT_BEGIN(structname)                                                                                       \
    typedef struct structname structname;                                                                              \
    struct structname {
#define STRUCT_FIELD_WORD(structname, name, offset)               uint32_t name;
#define STRUCT_FIELD_PTR(structname, type, name, offset)          type *name;
#define STRUCT_FIELD_STRUCT(structname, type, name, offset, size) type name;
#define STRUCT_END(structname)                                                                                         \
    }                                                                                                                  \
    ;

#endif



// Context for interrupts, exceptions and traps in relation to threads.
STRUCT_BEGIN(isr_ctx_t)
// Scratch words for use by the ASM code.
STRUCT_FIELD_WORD(isr_ctx_t, scratch0, 0)
STRUCT_FIELD_WORD(isr_ctx_t, scratch1, 4)
STRUCT_FIELD_WORD(isr_ctx_t, scratch2, 8)
STRUCT_FIELD_WORD(isr_ctx_t, scratch3, 12)
STRUCT_FIELD_WORD(isr_ctx_t, scratch4, 16)
STRUCT_FIELD_WORD(isr_ctx_t, scratch5, 20)
STRUCT_FIELD_WORD(isr_ctx_t, scratch6, 24)
STRUCT_FIELD_WORD(isr_ctx_t, scratch7, 28)
// Registers storage.
// The trap/interrupt handler will save registers to here.
// *Note: The syscall handler only saves/restores t0-t3, sp, gp, tp and ra, any other registers are not visible to the
// kernel.*
STRUCT_FIELD_STRUCT(isr_ctx_t, cpu_regs_t, regs, 32, 128)
STRUCT_END(isr_ctx_t)



#ifndef __ASSEMBLER__

// Stack alignment is defined to be 16 by the RISC-V calling convention
enum {
    STACK_ALIGNMENT = 16,
};

// Get the current kernel context.
static inline isr_ctx_t *isr_ctx_get() {
    isr_ctx_t *kctx;
    asm("csrr %0, mscratch" : "=r"(kctx));
    return kctx;
}
// Print a register dump given isr_ctx_t.
void isr_ctx_dump(isr_ctx_t const *ctx);
// Print a register dump of the current registers.
void kernel_cur_regs_dump();
#endif



#undef STRUCT_BEGIN
#undef STRUCT_FIELD_WORD
#undef STRUCT_FIELD_PTR
#undef STRUCT_END
