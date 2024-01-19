
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
} xip_range_t;

// Get XIP base address.
size_t      xip_map_base();
// Detect XIP size, if any.
size_t      xip_map_size();
// Detect XIP size, if any.
size_t      xip_rom_size();
// Get the XIP page size.
size_t      xip_get_page_size();
// Set the XIP page size.
void        xip_set_page_size(size_t size);
// Get number of XIP regions.
size_t      xip_regions();
// Get XIP region.
xip_range_t xip_get(size_t index);
// Set XIP region.
bool        xip_set(size_t index, xip_range_t value);
// Map an arbitrary page-aligned XIP range.
// If `override` is false and a region already uses part of the virtual address space, this operation will fail.
bool        xip_map(xip_range_t range, bool override);
// Unmap an arbitrary page-aligned XIP range.
bool        xip_unmap(size_t vaddr, size_t length);
// Debug: Dump XIP regions.
void        xip_dump();
