
// SPDX-License-Identifier: MIT

#include "memprotect.h"

#include "cpu/riscv_pmp.h"
#include "port/hardware_allocation.h"
#include "port/interrupt.h"

// Initialise memory protection driver.
void memprotect_init() {
    // There are no memory protections to enable that can also be disabled.
}
