
// SPDX-License-Identifier: MIT

#ifdef HAS_PARTSYS_ESP

#include "badge_strings.h"
#include "log.h"
#include "partsys.h"



#ifndef ESP_PARTTAB_OFFSET
// Partition table offset.
#define ESP_PARTTAB_OFFSET 0x8000
#endif
#ifndef ESP_PARTTAB_SIZE
// Partition table size in bytes.
#define ESP_PARTTAB_SIZE 0xc00
#endif

// Partition magic value.
#define ESP_PART_MAGIC 0x50aa
// MD5 sum magic value.
#define ESP_MD5_MAGIC  0xebeb

// Partition type: application.
#define PART_TYPE_APP   0x00
// Partition type: data.
#define PART_TYPE_DATA  0x01
// Partition type: AppFS
#define PART_TYPE_APPFS 0x43

// Partition subtype: factory app.
#define PART_SUBTYPE_APP_FACTORY 0x00
// Partition subtype: OTA0 app.
#define PART_SUBTYPE_APP_OTA0    0x10
// Partition subtype: OTA15 app.
#define PART_SUBTYPE_APP_OTA15   0x1f
// Partition subtype: test app.
#define PART_SUBTYPE_APP_TEST    0x20

// Partition subtype: OTA selection data.
#define PART_SUBTYPE_DATA_OTA      0x00
// Partition subtype: PHY init data.
#define PART_SUBTYPE_DATA_PHY      0x01
// Partition subtype: NVS data.
#define PART_SUBTYPE_DATA_NVS      0x02
// Partition subtype: core dump data.
#define PART_SUBTYPE_DATA_COREDUMP 0x03
// Partition subtype: NVS key data.
#define PART_SUBTYPE_DATA_NVS_KEY  0x04
// Partition subtype: ESPHTTPD data.
#define PART_SUBTYPE_DATA_ESPHTTPD 0x80
// Partition subtype: FAT filesystem.
#define PART_SUBTYPE_DATA_FATFS    0x81
// Partition subtype: SPIFFS filesystem.
#define PART_SUBTYPE_DATA_SPIFFS   0x82
// Partition subtype: LittleFS filesystem.
#define PART_SUBTYPE_DATA_LITTLEFS 0x83

// Partition subtype: AppFS.
#define PART_SUBTYPE_APPFS 0x3

// ESP partition table entry.
typedef struct {
    // Partition table entry magic value.
    uint16_t magic;
    // Partition type.
    uint8_t  type;
    // Partition subtype.
    uint8_t  subtype;
    // Partition offset.
    uint32_t offset;
    // Partition size.
    uint32_t size;
    // Partition label.
    char     label[16];
    // Parition flags.
    struct {
        // Partition is encrypted.
        uint32_t encrypted : 1;
        uint32_t           : 31;
    } flags;
} esp_part_entry_t;



// Try to identify and read a partitioning system.
static diskoff_t partsys_esp_ident(bootmedia_t *media) {
    // Partition table identifiable by magic value.
    esp_part_entry_t entry;
    if (media->read(media, ESP_PARTTAB_OFFSET, sizeof(esp_part_entry_t), &entry) != sizeof(esp_part_entry_t)) {
        logk(LOG_WARN, "Too few bytes read from media (magic)");
        return 0;
    } else if (entry.magic != ESP_PART_MAGIC) {
        return 0;
    }

    // Count partitions.
    diskoff_t i;
    for (i = 0; i < (diskoff_t)(ESP_PARTTAB_SIZE / sizeof(esp_part_entry_t)); i++) {
        // Read partition entry.
        if (media->read(media, ESP_PARTTAB_OFFSET + i * sizeof(esp_part_entry_t), sizeof(esp_part_entry_t), &entry) !=
            sizeof(esp_part_entry_t)) {
            return i;
        } else if (entry.magic != ESP_PART_MAGIC) {
            return i;
        }

        // Show we found it.
        char const *type = NULL;
        if (entry.type == PART_TYPE_APP) {
            switch (entry.subtype) {
                case PART_SUBTYPE_APP_FACTORY: type = "factory app"; break;
                case PART_SUBTYPE_APP_OTA0 ... PART_SUBTYPE_APP_OTA15: type = "OTA app"; break;
                case PART_SUBTYPE_APP_TEST: type = "test app"; break;
            }
        } else if (entry.type == PART_TYPE_DATA) {
            switch (entry.subtype) {
                case PART_SUBTYPE_DATA_OTA: type = "OTA selection data"; break;
                case PART_SUBTYPE_DATA_PHY: type = "PHY init data"; break;
                case PART_SUBTYPE_DATA_NVS: type = "NVS data"; break;
                case PART_SUBTYPE_DATA_COREDUMP: type = "core dump data"; break;
                case PART_SUBTYPE_DATA_NVS_KEY: type = "NVS key data"; break;
                case PART_SUBTYPE_DATA_ESPHTTPD: type = "ESPHTTPD data"; break;
                case PART_SUBTYPE_DATA_FATFS: type = "FAT filesystem"; break;
                case PART_SUBTYPE_DATA_SPIFFS: type = "SPIFFS filesystem"; break;
                case PART_SUBTYPE_DATA_LITTLEFS: type = "LittleFS filesystem"; break;
            }
        } else if (entry.type == PART_TYPE_APPFS && entry.subtype == PART_SUBTYPE_APPFS) {
            type = "AppFS filesystem";
        }
        if (type) {
            logkf(
                LOG_INFO,
                "Partition %{size;d} at %{u32;x}-%{u32;x}: %{cs}",
                i,
                entry.offset,
                entry.offset + entry.size - 1,
                type
            );
        } else {
            logkf(
                LOG_INFO,
                "Partition %{size;d} %{cs} at %{u32;x}-%{u32;x}: unknown (%{u8;x}/%{u8;x})",
                i,
                entry.offset,
                entry.offset + entry.size - 1,
                entry.type,
                entry.subtype
            );
        }
    }
    return i;
}

// Read a partition entry.
static partition_t partsys_esp_read(bootmedia_t *media, diskoff_t part_index) {
    // Read partition entry.
    esp_part_entry_t entry;
    if (media->read(
            media,
            ESP_PARTTAB_OFFSET + sizeof(esp_part_entry_t) * part_index,
            sizeof(esp_part_entry_t),
            &entry
        ) != sizeof(esp_part_entry_t)) {
        logk(LOG_ERROR, "Too few bytes read from media (partition entry; this is a bug)");
        return (partition_t){.flags = {.bootable = false}};
    } else if (entry.magic != ESP_PART_MAGIC) {
        return (partition_t){.flags = {.bootable = false}};
    }

    // Convert partition format.
    partition_t part;
    part.media      = media;
    part.offset     = entry.offset;
    part.length     = entry.size;
    size_t name_len = cstr_length_upto(
        entry.label,
        sizeof(part.name) - 1 < sizeof(entry.label) ? sizeof(part.name) - 1 : sizeof(entry.label)

    );
    mem_copy(part.name, entry.label, name_len);
    part.name[name_len] = 0;
    part.flags.bootable = entry.type == PART_TYPE_APP || entry.type == PART_TYPE_APPFS;

    return part;
}



// Esp partitioning system.
static partsys_t esp_partsys = {
    .ident = partsys_esp_ident,
    .read  = partsys_esp_read,
};

// Register Esp partitioning system.
static void register_esp_partsys() __attribute__((constructor));
static void register_esp_partsys() {
    partsys_register(&esp_partsys);
}

#endif
