
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>

// Initialise memory protection driver.
void memprotect_init();

// Set the range of external RAM assigned to userland.
void memprotect_set_user_extram(size_t base, size_t top);
// Set the range of SRAM assigned to userland.
void memprotect_set_user_sram(size_t base, size_t top);
// Set the range of flash assigned to userland.
void memprotect_set_user_flash(size_t base, size_t top);
