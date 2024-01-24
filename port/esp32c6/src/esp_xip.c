
// SPDX-License-Identifier: MIT

#include "log.h"
#include "port/esp_cache.h"
#include "port/hardware.h"
#include "port/reg/esp_spimem.h"
#include "xip.h"

#define SOC_MMU_VALID     (1 << 9)
#define SOC_MMU_SENSITIVE (1 << 10)
#define SOC_MMU_ADDR_MASK ((1 << 9) - 1)

#define SPIMEM0 (*(spimem_t *)(SPI0_BASE))
#define SPIMEM1 (*(spimem_t *)(SPI1_BASE))
#define XIPMEM  SPIMEM0

extern uint8_t const start_xip[] asm("__start_xip");
extern uint8_t const stop_xip[] asm("__stop_xip");



// Get XIP base address.
size_t xip_map_base() {
    return (size_t)start_xip;
}

// Detect XIP size, if any.
size_t xip_map_size() {
    return xip_regions() * xip_get_page_size();
}

// Detect XIP size, if any.
size_t xip_rom_size() {
    return XIP_PAGE_PHYS_MAX * xip_get_page_size();
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
    return XIP_PAGE_VIRT_MAX;
}

// Get XIP region.
xip_range_t xip_get(size_t index) {
    // Validate entry index.
    if (index >= xip_regions()) {
        logkf(LOG_ERROR, "Invalid XIP region index: %{size;d}", index);
        return (xip_range_t){.enable = false};
    }

    // Convert to an abstract entry.
    XIPMEM.mmu_item_index = index;
    uint32_t raw          = XIPMEM.mmu_item_content;
    return (xip_range_t){
        .rom_addr = xip_get_page_size() * (raw & SOC_MMU_ADDR_MASK),
        .map_addr = xip_map_base() + xip_get_page_size() * index,
        .length   = xip_get_page_size(),
        .enable   = raw & SOC_MMU_VALID,
    };
}

// Set XIP region.
bool xip_set(size_t index, xip_range_t range) {
    // Validate entry index.
    if (index >= xip_regions()) {
        logkf(LOG_ERROR, "Invalid XIP region index: %{size;d}", index);
        return false;
    }

    // Validate entry address.
    if (range.enable && range.map_addr != xip_map_base() + index * xip_get_page_size()) {
        logkf(
            LOG_ERROR,
            "Invalid XIP region map address: %{size;x}-%{size;x}",
            range.map_addr,
            range.map_addr + range.length - 1
        );
        return false;

    } else if (range.enable && range.rom_addr & (xip_get_page_size() - 1)) {
        logkf(
            LOG_ERROR,
            "Invalid XIP region ROM address: %{size;x}-%{size;x}",
            range.rom_addr,
            range.rom_addr + range.length - 1
        );
        return false;

    } else if (range.enable && range.rom_addr / xip_get_page_size() >= XIP_PAGE_PHYS_MAX) {
        logkf(
            LOG_ERROR,
            "Invalid XIP region ROM address: %{size;x}-%{size;x}",
            range.rom_addr,
            range.rom_addr + range.length - 1
        );
        return false;

    } else if (range.enable && range.length != xip_get_page_size()) {
        logkf(LOG_ERROR, "Invalid XIP region length: %{size;x}", range.length);
        return false;
    }

    // Convert to an MMU entry.
    XIPMEM.mmu_item_index   = index;
    XIPMEM.mmu_item_content = (range.rom_addr / xip_get_page_size()) | SOC_MMU_VALID;

    return esp_cache_flush(range.map_addr, range.length);
}

// Map an arbitrary page-aligned XIP range.
// If `override` is false and a region already uses part of the virtual address space, this operation will fail.
bool xip_map(xip_range_t range, bool override) {
    if (!range.enable) {
        logk(LOG_WARN, "Region passed to `xip_map` not enabled");
        return false;
    }

    uint32_t page_size = xip_get_page_size();

    // Validate entry address.
    if (range.map_addr < xip_map_base() || range.map_addr + range.length > xip_map_base() + xip_map_size()) {
        logkf(
            LOG_ERROR,
            "Invalid XIP region map address: %{size;x}-%{size;x}",
            range.map_addr,
            range.map_addr + range.length - 1
        );
        return false;

    } else if (range.rom_addr + range.length > xip_rom_size()) {
        logkf(
            LOG_ERROR,
            "Invalid XIP region ROM address: %{size;x}-%{size;x}",
            range.rom_addr,
            range.rom_addr + range.length - 1
        );
        return false;

    } else if (range.rom_addr % page_size != range.map_addr % page_size) {
        logkf(
            LOG_ERROR,
            "Mismatched sub-page address: paddr %{size;x} vs vaddr %{size;x} (mapping %{size;x} to %{size;x})",
            range.rom_addr % page_size,
            range.map_addr % page_size,
            range.rom_addr,
            range.map_addr
        );
        return false;
    }

    // Align range to entire pages.
    if (range.map_addr % page_size) {
        range.length   += range.rom_addr % page_size;
        range.map_addr -= range.rom_addr % page_size;
        range.rom_addr -= range.rom_addr % page_size;
    }
    if (range.length % page_size) {
        range.length += page_size - range.length % page_size;
    }

    if (!override) {
        // Assert the map is currently empty.
        for (size_t i = range.map_addr / page_size; i < (range.map_addr + range.length) / page_size; i++) {
            XIPMEM.mmu_item_index = i;
            if (XIPMEM.mmu_item_content & SOC_MMU_VALID) {
                logkf(
                    LOG_ERROR,
                    "Region at vaddr %{size;x}-%{size;x} overlaps with existing page at vaddr %{size;x}-%{size;x}",
                    range.map_addr,
                    range.map_addr + page_size - 1,
                    i * page_size,
                    i * page_size + page_size - 1
                );
                return false;
            }
        }
    }

    // Update MMU entries.
    for (size_t i = range.map_addr / page_size, x = range.rom_addr / page_size;
         i < (range.map_addr + range.length) / page_size;
         i++, x++) {
        XIPMEM.mmu_item_index   = i;
        XIPMEM.mmu_item_content = x | SOC_MMU_VALID;
    }

    // Invalidate cache range.
    return esp_cache_flush(range.map_addr, range.length);
}

// Unmap an arbitrary page-aligned XIP range.
bool xip_unmap(size_t vaddr, size_t length) {
    uint32_t page_size = xip_get_page_size();

    // Validate range address.
    if (vaddr < xip_map_base() || vaddr + length > xip_map_base() + xip_map_size()) {
        logkf(LOG_ERROR, "Invalid XIP region map address: %{size;x}-%{size;x}", vaddr, vaddr + length - 1);
        return false;
    }

    // Align range to entire pages.
    if (vaddr % page_size) {
        length += vaddr % page_size;
        vaddr  -= vaddr % page_size;
    }
    if (length % page_size) {
        length += page_size - length % page_size;
    }

    // Update MMU entries.
    for (size_t i = vaddr / page_size; i < (vaddr + length) / page_size; i++) {
        XIPMEM.mmu_item_index   = i;
        XIPMEM.mmu_item_content = 0;
    }

    return true;
}

// Get an available virtual address.
// Returns 0 if there are no more free addresses.
size_t xip_find_vaddr() {
    for (ptrdiff_t i = XIP_PAGE_VIRT_MAX - 1; i >= 0; i--) {
        XIPMEM.mmu_item_index = i;
        if (!(XIPMEM.mmu_item_content & SOC_MMU_VALID)) {
            return xip_map_base() + (size_t)i * xip_get_page_size();
        }
    }
    return 0;
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
        size_t map_addr = xip_map_base() + i * page_size;
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
