
// SPDX-License-Identifier: MIT

#include "badge_err.h"
#include "bootprotocol.h"
#include "checksum.h"
#include "log.h"
#include "memprotect.h"
#include "port.h"
#include "port/interrupt.h"
#include "time.h"



static void bootstrap();

// ISR context.
static isr_ctx_t isr_ctx;

// After control handover, the booting CPU core starts here and other cores wait.
// This sets up the basics of everything needed by the bootloader.
// When finished, the booting CPU will continue by bootstrapping the kernel / firmware.
void basic_runtime_init() {
    // ISR initialization.
    interrupt_init(&isr_ctx);
    // Early platform initialization.
    port_early_init();

    // Timekeeping initialization.
    time_init();

    // Memory protection initialization.
    memprotect_init();

    // Full hardware initialization.
    port_init();

    // Continue to bootstrapping.
    bootstrap();
}

// After basic runtime initialization, the booting CPU core continues here.
// This looks for bootable media and any bootable images on said media.
// When finished, a kernel is bootstrapped or the bootloader gives up and halts.
static void bootstrap() {
    logk(LOG_INFO, "KiloBootloader v0.1");

    partsys_first->ident(bootmedia_first);
    bootprotocol_first->boot(bootmedia_first, 0x10000);

    logk(LOG_FATAL, "Failed to boot!");
    while (1) continue;
}
