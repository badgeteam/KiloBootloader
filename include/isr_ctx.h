
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct isr_ctx_t isr_ctx_t;

// Get the current kernel context.
static inline isr_ctx_t *isr_ctx_get();

// Print a register dump given isr_ctx_t.
void isr_ctx_dump(isr_ctx_t const *ctx);
// Print a register dump of the current registers.
void kernel_cur_regs_dump();

#include "cpu/isr_ctx.h"
