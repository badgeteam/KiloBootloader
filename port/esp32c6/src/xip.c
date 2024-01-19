
// SPDX-License-Identifier: MIT

#include "xip.h"

#include "port/hardware.h"
#include "port/reg/esp_spimem.h"

#define SPIMEM0 (*(spimem_t)(SPI0_BASE))
#define SPIMEM1 (*(spimem_t)(SPI1_BASE))

extern uint8_t const start_xip[] asm("__start_xip");
extern uint8_t const stop_xip[] asm("__stop_xip");



// Get XIP base address.
size_t xip_base() {
    return (size_t)start_xip;
}

// Detect XIP size, if any.
size_t xip_size() {
    return xip_regions() * xip_get_page_size();
}

// Get the XIP page size.
size_t xip_get_page_size() {
}

// Set the XIP page size.
void xip_set_page_size(size_t size) {
}

// Get number of XIP regions.
size_t xip_regions() {
    return 256;
}

// Get XIP region.
xip_entry_t xip_get(size_t index);
// Set XIP region.
void        xip_set(size_t index);
