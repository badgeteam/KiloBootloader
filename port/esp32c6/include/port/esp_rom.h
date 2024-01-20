
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// ROM function: Enable instruction cache.
void     Cache_Enable_ICache(uint32_t autoload_en);
// ROM function: Enable data cache.
void     Cache_Enable_DCache(uint32_t autoload_en);
// ROM function: Disable instruction cache.
// Returns whether autoload was enabled.
uint32_t Cache_Disable_ICache();
// ROM function: Disable data cache.
// Returns whether autoload was enabled.
uint32_t Cache_Disable_DCache();
// ROM function: Get instruction cache line size.
uint32_t Cache_Get_ICache_Line_Size();
// ROM function: Get data cache line size.
uint32_t Cache_Get_DCache_Line_Size();
// Invalidate a range of instruction and/or data cache.
// Returns 0 on success, 1 on error.
int      Cache_Invalidate_Addr(uint32_t address, uint32_t length);
