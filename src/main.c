
// SPDX-License-Identifier: MIT

#include "badge_err.h"
#include "bootmedia.h"
#include "log.h"
#include "memprotect.h"
#include "port/interrupt.h"
#include "rawprint.h"
#include "time.h"
#include "xip.h"



// ISR context.
static isr_ctx_t isr_ctx;

char lol[128];

// After control handover, the booting CPU core starts here and other cores wait.
// This sets up the basics of everything needed by the other systems of the kernel.
// When finished, the booting CPU will perform kernel initialization.
void basic_runtime_init() {
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

    logkf(LOG_DEBUG, "First media: %{size;x}", bootmedia_first);
    logkf(LOG_DEBUG, "Last media:  %{size;x}", bootmedia_last);

    // Map the random text.
    xip_map(
        (xip_range_t){
            .rom_addr = 0x18000,
            .map_addr = 0x42010000,
            .length   = 128,
            .enable   = true,
        },
        false
    );

    // Dump XIP mappings.
    xip_dump();

    // Try to show the text.
    rawprint((char const *)0x42010000);

    // Read the text.
    bootmedia_t *media = bootmedia_first;
    media->read(media, 0x20000, sizeof(lol) - 1, lol);
    rawprint(lol);

    while (1) continue;
}
