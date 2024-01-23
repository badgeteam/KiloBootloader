
// SPDX-License-Identifier: MIT

#ifdef HAS_FILESYS_APPFS

#include "attributes.h"
#include "filesys.h"
#include "log.h"



// Number of data pages.
#define PAGES     255
// Size of each data page.
#define PAGE_SIZE 65536
// AppFS header magic value.
static char const appfs_magic[8] = "AppFsDsc";

// AppFS sector usage status.
typedef enum {
    // Sector is free.
    APPFS_USE_FREE    = 0xff,
    // Sector cannot be used.
    APPFS_USE_ILLEGAL = 0x55,
    // Sector is in use as file data.
    APPFS_USE_DATA    = 0,
} appfs_use_t;

// AppFS header.
typedef struct {
    // Magic value, must be `appfs_magic`.
    uint8_t  magic[8];
    // Header serial number.
    uint32_t serial;
    // Header and FAT CRC.
    uint32_t crc32;
} appfs_hdr_t;

// AppFS FAT entry.
// Fields marked as "1st sector only" must be written 0xff on 2nd or later sectors of a file.
typedef struct {
    // Filename, 1st sector only.
    char     name[48];
    // Additional description, 1st sector only.
    char     title[64];
    // Size in byes.
    uint32_t size;
    // Next sector index, or 0 if end of file.
    uint8_t  next;
    // What this sector is used for, one of `APPFS_USE_*`
    uint8_t  used;
    // File version, 1st sector only.
    uint16_t version;
    // Reserved.
    uint8_t  _reserved[8];
} appfs_fat_t;



// Try to identify a filesystem.
static bool filesys_appfs_ident(partition_t *part) {
    bootmedia_t *media = part->media;
    // Check metadata 0.
    char         id[sizeof(appfs_magic)];
    if (media->read(media, part->offset, sizeof(appfs_magic), id) != sizeof(appfs_magic)) {
        logk(LOG_WARN, "Too few bytes read from media (header 0)");
        return false;
    }
    if (!memcmp(id, appfs_magic, sizeof(appfs_magic))) {
        return true;
    }

    // Check metadata 1.
    if (media->read(media, part->offset + PAGE_SIZE / 2, sizeof(appfs_magic), id) != sizeof(appfs_magic)) {
        logk(LOG_WARN, "Too few bytes read from media (header 1)");
        return false;
    }
    if (!memcmp(id, appfs_magic, sizeof(appfs_magic))) {
        return true;
    }

    // If neither has the magic value, it is not an AppFS.
    return false;
}

// Try to open the kernel file on this filesystem.
static bool filesys_appfs_read(partition_t *part, filesys_t *filesys, file_t *file) {
    bootmedia_t *media = part->media;
    // Headers valid.
    bool         hdr0_valid, hdr1_valid;
    // Header data.
    appfs_hdr_t  hdr0, hdr1;

    // Read headers.
    if (media->read(media, part->offset, sizeof(appfs_hdr_t), &hdr0) != sizeof(appfs_hdr_t)) {
        logk(LOG_ERROR, "Too few bytes read from media (header 0)");
        return false;
    }
    if (media->read(media, part->offset + PAGE_SIZE / 2, sizeof(appfs_hdr_t), &hdr1) != sizeof(appfs_hdr_t)) {
        logk(LOG_ERROR, "Too few bytes read from media (header 1)");
        return false;
    }
    hdr0_valid = !memcmp(&hdr0.magic, appfs_magic, sizeof(appfs_magic));
    hdr1_valid = !memcmp(&hdr1.magic, appfs_magic, sizeof(appfs_magic));

    return false;
}



// AppFS file system.
static filesys_type_t appfs_filesys = {
    .ident = filesys_appfs_ident,
    .read  = filesys_appfs_read,
};

// Register AppFS file system.
static void register_appfs_filesys() __attribute__((constructor));
static void register_appfs_filesys() {
    filesys_register(&appfs_filesys);
}

#endif
