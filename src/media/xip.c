
// SPDX-License-Identifier: MIT

#ifdef HAS_BOOTMEDIA_XIP

#include "xip.h"

#include "badge_strings.h"
#include "bootmedia.h"
#include "log.h"



// XIP random read function.
static diskoff_t bootmedia_xip_read(bootmedia_t *media, diskoff_t offset, diskoff_t length, void *_mem) {
    (void)media;
    size_t page_size = xip_get_page_size();

    // Memory currently being written to.
    uint8_t *mem  = _mem;
    // Amount read in total.
    size_t   read = 0;

    while (length > 0) {
        // Get a temporary page to map to.
        xip_range_t range = {
            .rom_addr = offset - offset % page_size,
            .map_addr = xip_find_vaddr(),
            .length   = xip_get_page_size(),
            .enable   = true,
        };
        if (!range.map_addr) {
            logk(LOG_ERROR, "Out of XIP to map for reading");
            break;
        }
        if (!xip_map(range, true)) {
            logk(LOG_ERROR, "Unable to map XIP for reading");
            break;
        }

        // Page read start address.
        size_t start = (size_t)offset % page_size;
        // Page read end address.
        size_t end   = start + length > page_size ? page_size : start + length;
        // Copy from the memory-mapped page.
        mem_copy(mem + read, (void const *)(range.map_addr + start), end - start);

        // Unmap the temporary page.
        xip_unmap(range.map_addr, 1);

        // Move on to the next page.
        offset += end - start;
        length -= end - start;
        read   += end - start;
    }

    return read;
}

// XIP memory map function.
static bool bootmedia_xip_mmap(bootmedia_t *media, diskoff_t offset, diskoff_t length, size_t vaddr) {
    (void)media;
    return xip_map(
        (xip_range_t){
            .rom_addr = offset,
            .map_addr = vaddr,
            .length   = length,
            .enable   = true,
        },
        true
    );
}

// Memory map page size function.
diskoff_t bootmedia_xip_page(bootmedia_t *media, diskoff_t *page_size) {
    (void)media;
    if (page_size) {
        if (*page_size < XIP_REGION_MIN_SIZE) {
            xip_set_page_size(XIP_REGION_MIN_SIZE);
        } else if (*page_size > XIP_REGION_MAX_SIZE) {
            xip_set_page_size(XIP_REGION_MAX_SIZE);
        } else {
            xip_set_page_size(*page_size);
        }
    }
    return xip_get_page_size();
}



// XIP boot media.
static bootmedia_t xip_media = {
    .read = bootmedia_xip_read,
    .mmap = bootmedia_xip_mmap,
    .page = bootmedia_xip_page,
};

// Register XIP boot media.
void register_xip_media() {
    bootmedia_register(&xip_media);
#ifdef XIP_MEDIA_PAGE_SIZE
    xip_set_page_size(XIP_MEDIA_PAGE_SIZE);
#else
    xip_set_page_size(XIP_REGION_MAX_SIZE);
#endif
    xip_media.size = xip_rom_size();
}

#endif
