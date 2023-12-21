
// SPDX-License-Identifier: MIT

#include "cpu/isr.h"

#include "cpu/isr_ctx.h"
#include "cpu/panic.h"
#include "log.h"
#include "rawprint.h"



// Table of trap names.
static char const *const trapnames[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store address misaligned",
    "Store access fault",
    "ECALL from U-mode",
    "ECALL from S-mode",
    NULL, // Reserved
    "ECALL from M-mode",
    "Instruction page fault",
    "Load page fault",
    NULL, // Reserved
    "Store page fault",
};
enum { TRAPNAMES_LEN = sizeof(trapnames) / sizeof(trapnames[0]) };

// Bitmask of traps that have associated memory addresses.
#define MEM_ADDR_TRAPS 0x00050f0

// Called from ASM on non-system call trap.
void __trap_handler() {
    uint32_t mcause, mstatus, mtval, mepc;
    asm volatile("csrr %0, mstatus" : "=r"(mstatus));
    asm volatile("csrr %0, mcause" : "=r"(mcause));

    // Print trap name.
    if (mcause < TRAPNAMES_LEN && trapnames[mcause]) {
        rawprint(trapnames[mcause]);
    } else {
        rawprint("Trap 0x");
        rawprinthex(mcause, 8);
    }

    // Print PC.
    asm volatile("csrr %0, mepc" : "=r"(mepc));
    rawprint(" at PC 0x");
    rawprinthex(mepc, 8);

    // Print trap value.
    asm volatile("csrr %0, mtval" : "=r"(mtval));
    if (mtval && ((1 << mcause) & MEM_ADDR_TRAPS)) {
        rawprint(" while accessing 0x");
        rawprinthex(mtval, 8);
    }

    rawputc('\r');
    rawputc('\n');

    isr_ctx_t *kctx = isr_ctx_get();
    isr_ctx_dump(kctx);

    // When the bootloader traps it's a bad time.
    panic_poweroff();
}
