
// SPDX-License-Identifier: MIT

#include "memprotect.h"

#include "cpu/riscv_pmp.h"
#include "port/hardware_allocation.h"
#include "port/interrupt.h"

// Initialise memory protection driver.
void memprotect_init() {
    // Initialise PMP driver.
    riscv_pmp_init();
}
