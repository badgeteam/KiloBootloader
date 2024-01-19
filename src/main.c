
// SPDX-License-Identifier: MIT

#include "badge_err.h"
#include "log.h"
#include "memprotect.h"
#include "port/interrupt.h"
#include "time.h"
#include "xip.h"

#include <stdatomic.h>

// ISR context.
static isr_ctx_t isr_ctx;

#define show_csr(name)                                                                                                 \
    do {                                                                                                               \
        long csr;                                                                                                      \
        asm("csrr %0, " #name : "=r"(csr));                                                                            \
        logkf(LOG_INFO, #name ": %{long;x}", csr);                                                                     \
    } while (0)

// After control handover, the booting CPU core starts here and other cores wait.
// This sets up the basics of everything needed by the other systems of the kernel.
// When finished, the booting CPU will perform kernel initialization.
void basic_runtime_init() {
    badge_err_t ec = {0};

    // ISR initialization.
    interrupt_init(&isr_ctx);
    // Early platform initialization.
    // port_early_init();

    // Timekeeping initialization.
    time_init();

    // Memory protection initialization.
    memprotect_init();

    // Full hardware initialization.
    // port_init();

    // Dump XIP mappings.
    xip_dump();

    while (1) continue;
}
