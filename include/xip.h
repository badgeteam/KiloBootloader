
// SPDX-License-Identifier: MIT

#pragma once

#include "port/xip.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// XIP mapping.
typedef struct {
    // ROM address.
    size_t rom_addr;
    // Mapped address.
    size_t map_addr;
    // Mapped length.
    size_t length;
    // Region enabled.
    bool   enable;
    // Read permission.
    bool   r;
    // Write permission.
    bool   w;
    // Execute permission.
    bool   x;
} xip_entry_t;

// Get XIP base address.
size_t      xip_base();
// Detect XIP size, if any.
size_t      xip_size();
// Get the XIP page size.
size_t      xip_get_page_size();
// Set the XIP page size.
void        xip_set_page_size(size_t size);
// Get number of XIP regions.
size_t      xip_regions();
// Get XIP region.
xip_entry_t xip_get(size_t index);
// Set XIP region.
void        xip_set(size_t index, xip_entry_t value);
// Debug: Dump XIP regions.
void        xip_dump();
