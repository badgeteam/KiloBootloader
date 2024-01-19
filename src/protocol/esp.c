
// SPDX-License-Identifier: MIT

#ifdef HAS_BOOTPROTOCOL_ESP

#include "bootprotocol.h"
#include "log.h"



#ifndef ESP_PARTTAB_OFF
#define ESP_PARTTAB_OFF 0x8000
#endif

#define ESP_MAX_SEG 16

// ESP boot protocol header.
typedef struct {
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
    bool     has_sha256;
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
bool bootprotocol_esp_ident(bootmedia_t *media, diskoff_t offset) {
    // Try to read the header.
    esp_boot_hdr_t header;
    if (media->read(media, offset, sizeof(header), &header) != sizeof(header)) {
        logk(LOG_WARN, "Too few bytes read from media (header)");
        return false;
    }

    // The boot protocol is detected by combination of chip ID and magic value.
    return header.magic == 0xE9 && header.chip == ESP_CHIP_ID;
}

// ESP boot function.
bool bootprotocol_esp_boot(bootmedia_t *media, diskoff_t offset) {
    // Try to read the header.
    esp_boot_hdr_t header;
    if (media->read(media, offset, sizeof(header), &header) != sizeof(header)) {
        logk(LOG_ERROR, "Too few bytes read from media (header)");
        return false;
    }

    // Check the header more throuroughly.
    if (header.magic != 0xE9 || header.chip != ESP_CHIP_ID) {
        logk(LOG_ERROR, "Invalid ESP boot protocol header");
        return false;
    }
    if (header.segments == 0 || header.segments > ESP_MAX_SEG) {
        logkf(LOG_ERROR, "Invalid ESP segment count (%{u8;d})", header.segments);
        return false;
    }
    if (header.has_sha256) {
        logk(LOG_WARN, "ESP image has SHA256 appended, ignoring");
    }

    // Read segment headers.
    diskoff_t seg_off = offset + 24;
    for (size_t i = 0; i < header.segments; i++) {
        if (media->read(media, seg_off, sizeof(esp_boot_seg_t), &segs[i]) != sizeof(esp_boot_seg_t)) {
            logk(LOG_ERROR, "Too few bytes read from media (segment)");
            return false;
        }
        segs_paddr[i]  = seg_off;
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
    }

    return true;
}



// ESP boot protocol.
static bootprotocol_t esp_protocol = {
    .ident = bootprotocol_esp_ident,
    .boot  = bootprotocol_esp_boot,
};

// Register XIP boot media.
static void register_esp_protocol() __attribute__((constructor));
static void register_esp_protocol() {
    bootprotocol_register(&esp_protocol);
}

#endif