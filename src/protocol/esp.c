
// SPDX-License-Identifier: MIT

#ifdef HAS_BOOTPROTOCOL_ESP

#include "attributes.h"
#include "bootprotocol.h"
#include "log.h"
#include "memmap.h"
#include "port.h"



#ifndef ESP_PARTTAB_OFF
#define ESP_PARTTAB_OFF 0x8000
#endif

#define ESP_MAX_SEG 16

// ESP boot protocol header.
typedef struct PACKED {
    // Magic byte.
    uint8_t  magic;
    // Segment count.
    uint8_t  segments;
    // SPI mode.
    uint8_t  spi_mode;
    // SPI speed/size.
    uint8_t  spi_sz;
    // Entrypoint.
    uint32_t entry;
    // WP pin state.
    uint8_t  wp_state;
    // Drive settings.
    uint8_t  drive[3];
    // Chip ID.
    uint16_t chip;
    // Deprecated.
    uint8_t  _deprecated;
    // Minimum chip revision.
    uint16_t min_rev;
    // Maximum chip revision.
    uint16_t max_rev;
    // Reserved.
    uint32_t _reserved;
    // SHA256 appended.
    uint8_t  has_sha256;
} esp_boot_hdr_t;

// ESP boot protocol segment.
typedef struct {
    // Segment virtual address.
    uint32_t vaddr;
    // Segment length.
    uint32_t length;
} esp_boot_seg_t;



// ESP segments.
esp_boot_seg_t segs[ESP_MAX_SEG];
// ESP segment physical addresses.
uint32_t       segs_paddr[ESP_MAX_SEG];

// ESP identify function.
static bool bootprotocol_esp_ident(file_t *file) {
    // Try to read the header.
    esp_boot_hdr_t header;
    if (file->read(file, 0, sizeof(header), &header) != sizeof(header)) {
        logk(LOG_WARN, "Too few bytes read from media (header)");
        return false;
    }

    // The boot protocol is detected by combination of chip ID and magic value.
    return header.magic == 0xE9 && header.chip == ESP_CHIP_ID;
}

// ESP boot function.
static bool bootprotocol_esp_boot(file_t *file) {
    logk(LOG_INFO, "Trying ESP boot protocol");
    bootmedia_t *media = file->filesys->part->media;

    // Try to read the header.
    esp_boot_hdr_t header;
    if (file->read(file, 0, sizeof(header), &header) != sizeof(header)) {
        logk(LOG_ERROR, "Too few bytes read from media (header)");
        return false;
    }

    // Check the header more throuroughly.
    if (header.segments == 0 || header.segments > ESP_MAX_SEG) {
        logkf(LOG_ERROR, "Invalid ESP segment count (%{u8;d})", header.segments);
        return false;
    }
    if (header.has_sha256) {
        logk(LOG_WARN, "ESP image has SHA256 appended, ignoring");
    }

    // Lowest common denominator for page size.
    diskoff_t page_size = (DISKOFF_MAX >> 1) + 1;
    // Read segment headers.
    diskoff_t seg_off   = 24;
    for (size_t i = 0; i < header.segments; i++) {
        // Read a segment entry.
        if (file->read(file, seg_off, sizeof(esp_boot_seg_t), &segs[i]) != sizeof(esp_boot_seg_t)) {
            logk(LOG_ERROR, "Too few bytes read from media (segment)");
            return false;
        }
        segs_paddr[i]  = seg_off + sizeof(esp_boot_seg_t);
        seg_off       += sizeof(esp_boot_seg_t) + segs[i].length;
        logkf(
            LOG_INFO,
            "Segment %{size;d}/%{size;d}: map %{size;x} bytes from %{size;x} to %{size;x}",
            i + 1,
            header.segments,
            segs[i].length,
            segs_paddr[i],
            segs[i].vaddr
        );

        if (IS_XIP_RANGE(segs[i].vaddr, segs[i].length)) {
            // Find its maximum allowable page size.
            uint32_t  eq_mask       = segs[i].vaddr ^ segs_paddr[i];
            diskoff_t seg_page_size = 1;
            while (!(eq_mask & 1)) {
                eq_mask       >>= 1;
                seg_page_size <<= 1;
            }

            // Update the maximum allowable page size.
            if (page_size > seg_page_size) {
                page_size = seg_page_size;
            }
        }
    }

    // Update page size.
    if (media->page) {
        page_size = media->page(media, &page_size);
        logkf(LOG_INFO, "Using page size %{" FMT_TYPE_DISKOFF ";d}", page_size);
    }

    // Map segments.
    logkf(LOG_INFO, "Loading kernel");
    for (size_t i = 0; i < header.segments; i++) {
        if (IS_XIP_RANGE(segs[i].vaddr, segs[i].length)) {
            // Try to memory map this.
            file->mmap(file, segs_paddr[i], segs[i].length, segs[i].vaddr);
        } else if (IS_SRAM_RANGE(segs[i].vaddr, segs[i].length)) {
            // Try to read this.
            file->read(file, segs_paddr[i], segs[i].length, (void *)segs[i].vaddr);
        } else {
            // Not loadable to this address.
            logkf(
                LOG_ERROR,
                "Unable to satisfy virtual address range %{size;x}-%{size;x}",
                segs[i].vaddr,
                segs[i].vaddr + segs[i].length
            );
            return false;
        }
    }

    // Read checksum.
    diskoff_t xsum_off   = seg_off;
    diskoff_t padd_size  = ((xsum_off + 15) & ~15) - xsum_off;
    padd_size            = (padd_size - 1) & 15;
    xsum_off            += padd_size;
    uint8_t read_xsum    = 0;
    if (file->read(file, xsum_off, 1, &read_xsum) != 1) {
        logk(LOG_ERROR, "Too few bytes read from media (checksum)");
        return false;
    }


    // Take checksum of loaded segments.
    uint8_t xsum_state = 0xEF;
    for (size_t i = 0; i < header.segments; i++) {
        uint8_t const *ptr = (uint8_t const *)segs[i].vaddr;
        for (size_t x = 0; x < segs[i].length; x++) {
            xsum_state ^= ptr[x];
        }
    }

    // Compare checksums.
    if (read_xsum != xsum_state) {
        logkf(LOG_ERROR, "Checksum mismatch: expected %{u8;x}, got %{u8;x}", read_xsum, xsum_state);
        return false;
    }

    // Hand over control.
    logkf(LOG_INFO, "Jumping to 0x%{size;x}", header.entry);
    if (!port_pre_handover())
        return false;
    ((void (*)())header.entry)();

    return true;
}



// ESP boot protocol.
static bootprotocol_t esp_protocol = {
    .ident = bootprotocol_esp_ident,
    .boot  = bootprotocol_esp_boot,
};

// Register ESP boot protocol.
static void register_esp_protocol() __attribute__((constructor));
static void register_esp_protocol() {
    bootprotocol_register(&esp_protocol);
}

#endif
