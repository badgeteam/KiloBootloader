
// SPDX-License-Identifier: MIT

#pragma once

#include "cpu/regs.h"
#include "cpu/riscv.h"
#include "isr_ctx.h"

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#endif

// Word size of the stack used for interrupt and trap handlers.
#define ISR_STACK_DEPTH 4096

#ifdef __ASSEMBLER__
// clang-format off

// Interrupt vector table implemented in ASM.
    .global __interrupt_vector_table
// Called from ASM on interrupt.
    .global __interrupt_handler
// Called from ASM on system call.
    .global __syscall_handler
// Called from ASM on non-syscall trap.
    .global __trap_handler

// clang-format on
#else
// Interrupt vector table implemented in ASM.
extern void *__interrupt_vector_table[32];
// Callback from ASM to platform-specific interrupt handler.
extern void  __interrupt_handler();
// Signature for system call handler.
#define __SYSCALL_HANDLER_SIGNATURE                                                                                    \
    void __syscall_handler(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long sysnum)
// Suppress unused arguments warning for syscall implementation.
#define __SYSCALL_HANDLER_IGNORE_UNUSED                                                                                \
    (void)a0, (void)a1, (void)a2, (void)a3, (void)a4, (void)a5, (void)a6, (void)sysnum
// Callback from ASM to syscall implementation.
extern __SYSCALL_HANDLER_SIGNATURE;
// Callback from ASM on non-syscall trap.
extern void __trap_handler();
// Return a value from the syscall handler.
extern void __syscall_return(long long value) __attribute__((noreturn));

// Disable interrupts and return whether they were enabled.
static inline bool isr_global_disable() {
    uint32_t mstatus;
    asm volatile("csrrc %0, mstatus, %1" : "=r"(mstatus) : "r"((1U << RV32_MSTATUS_MIE_BIT)));
    return mstatus & (1U << RV32_MSTATUS_MIE_BIT);
}
// Enable interrupts.
static inline void isr_global_enable() {
    asm volatile("csrs mstatus, %0" ::"r"((1U << RV32_MSTATUS_MIE_BIT)));
}
// Explicit context switch from M-mode.
// Interrupts must be disabled on entry and will be re-enabled on exit.
// If the context switch target is not set, this is a NOP.
extern void isr_context_switch();

#endif
