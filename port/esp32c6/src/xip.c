
// SPDX-License-Identifier: MIT

#include "xip.h"

#include "log.h"
#include "port/hardware.h"
#include "port/reg/esp_spimem.h"

#define SOC_MMU_VALID     (1 << 9)
#define SOC_MMU_SENSITIVE (1 << 10)
#define SOC_MMU_ADDR_MASK ((1 << 9) - 1)

#define SPIMEM0 (*(spimem_t *)(SPI0_BASE))
#define SPIMEM1 (*(spimem_t *)(SPI1_BASE))
#define XIPMEM  SPIMEM0

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
    return 65536 / (1 << XIPMEM.mmu_power_ctrl.mmu_page_size);
}

// Set the XIP page size.
void xip_set_page_size(size_t size) {
    switch (size) {
        case 8192: XIPMEM.mmu_power_ctrl.mmu_page_size = 3; break;
        case 16384: XIPMEM.mmu_power_ctrl.mmu_page_size = 2; break;
        case 32768: XIPMEM.mmu_power_ctrl.mmu_page_size = 1; break;
        case 65536: XIPMEM.mmu_power_ctrl.mmu_page_size = 0; break;
        default: logkf(LOG_ERROR, "Invalid XIP page size: %{size;d}", size); break;
    }
}

// Get number of XIP regions.
size_t xip_regions() {
    return 256;
}

// Get XIP region.
xip_entry_t xip_get(size_t index) {
    // Validate entry index.
    if (index >= xip_regions()) {
        logkf(LOG_ERROR, "Invalid XIP region index: %{size;d}", index);
        return (xip_entry_t){.enable = false};
    }

    // Convert to an abstract entry.
    XIPMEM.mmu_item_index = index;
    uint32_t raw          = XIPMEM.mmu_item_content;
    return (xip_entry_t){
        .rom_addr = xip_get_page_size() * (raw & SOC_MMU_ADDR_MASK),
        .map_addr = xip_base() + xip_get_page_size() * index,
        .length   = xip_get_page_size(),
        .enable   = raw & SOC_MMU_VALID,
        .r        = true,
        .w        = true,
        .x        = true,
    };
}

// Set XIP region.
void xip_set(size_t index, xip_entry_t value) {
    // Validate entry index.
    if (index >= xip_regions()) {
        logkf(LOG_ERROR, "Invalid XIP region index: %{size;d}", index);
        return;
    }

    // Validate entry address.
    if (value.enable && value.map_addr != xip_base() + index * xip_get_page_size()) {
        logkf(LOG_ERROR, "Invalid XIP region map address: %{size;x}", value.map_addr);
        return;
    } else if (value.enable && value.rom_addr & (xip_get_page_size() - 1)) {
        logkf(LOG_ERROR, "Invalid XIP region ROM address: %{size;x}", value.rom_addr);
        return;
    } else if (value.enable && value.rom_addr / xip_get_page_size() >= (1 << 9)) {
        logkf(LOG_ERROR, "Invalid XIP region ROM address: %{size;x}", value.rom_addr);
        return;
    } else if (value.enable && value.length != xip_get_page_size()) {
        logkf(LOG_ERROR, "Invalid XIP region length: %{size;x}", value.length);
        return;
    }

    // Convert to an MMU entry.
    XIPMEM.mmu_item_index   = index;
    XIPMEM.mmu_item_content = (value.rom_addr / xip_get_page_size()) | SOC_MMU_VALID;
}

// Debug: Dump XIP regions.
void xip_dump() {
    size_t page_size = xip_get_page_size();
    logkf(LOG_DEBUG, "XIP page size: %{size;d}", page_size);
    logkf(LOG_DEBUG, "XIP mapping:");
    for (size_t i = 0; i < xip_regions(); i++) {
        // Read MMU entry.
        XIPMEM.mmu_item_index = i;
        uint32_t raw          = XIPMEM.mmu_item_content;
        if (!(raw & SOC_MMU_VALID))
            continue;

        // Print it.
        size_t rom_addr = (raw & SOC_MMU_ADDR_MASK) * page_size;
        size_t map_addr = xip_base() + i * page_size;
        logkf(
            LOG_DEBUG,
            "%{size;x}-%{size;x} to %{size;x}-%{size;x}",
            rom_addr,
            rom_addr + page_size - 1,
            map_addr,
            map_addr + page_size - 1
        );
    }
}
