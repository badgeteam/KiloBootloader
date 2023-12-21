
// SPDX-License-Identifier: MIT

#include "badge_err.h"
#include "log.h"
#include "memprotect.h"
#include "port/interrupt.h"
#include "time.h"

#include <stdatomic.h>



// The initial kernel stack.
extern char          stack_bottom[] asm("__stack_bottom");
extern char          stack_top[] asm("__stack_top");


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
    isr_ctx_t   tmp_ctx;

    // ISR initialization.
    interrupt_init(&tmp_ctx);
    // Early platform initialization.
    // port_early_init();

    // Timekeeping initialization.
    time_init();

    // Memory protection initialization.
    memprotect_init();

    // Full hardware initialization.
    // port_init();
}
