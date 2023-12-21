
// SPDX-License-Identifier: MIT

#include "cpu/isr_ctx.h"

#include "rawprint.h"

// Number of columns in register dump.
// Must be a power of 2.
#define COLS 4

// Register names table.
static char const regnames[32][4] = {
    "PC ", "RA ", "SP ", "GP ", "TP ", "T0 ", "T1 ", "T2 ", "S0 ", "S1 ", "A0 ", "A1 ", "A2 ", "A3 ", "A4 ", "A5 ",
    "A6 ", "A7 ", "S2 ", "S3 ", "S4 ", "S5 ", "S6 ", "S7 ", "S8 ", "S9 ", "S10", "S11", "T3 ", "T4 ", "T5 ", "T6 ",
};

// Print a register dump given cpu_regs_t.
void kernel_reg_dump_arr(uint32_t const *arr) {
    // Print all registers.
    rawprint("Register dump:\n");
    for (int y = 0; y < 32; y += COLS) {
        for (int x = 0; x < COLS; x++) {
            rawputc(' ');
            rawputc(' ');
            rawprint(regnames[y + x]);
            rawprint(" 0x");
            rawprinthex(arr[y + x], 8);
        }
        rawputc('\r');
        rawputc('\n');
    }
}

// Print a register dump given isr_ctx_t.
void isr_ctx_dump(isr_ctx_t const *ctx) {
    kernel_reg_dump_arr((uint32_t const *)&ctx->regs);
}
