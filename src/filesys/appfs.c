
// SPDX-License-Identifier: MIT

#ifdef HAS_FILESYS_APPFS

#include "filesys/appfs.h"

#include "attributes.h"
#include "badge_strings.h"
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

extern tobootloader_t tobootloader;



// Read an AppFS FAT entry.
static bool read_fat(filesys_t *filesys, uint8_t index, appfs_fat_t *fat) {
    if (index == 255)
        return false;
    diskoff_t offset =
        filesys->part->offset + filesys->active_fat * 256 * sizeof(appfs_fat_t) + sizeof(appfs_fat_t) * (index + 1);
    if (filesys->part->media->read(filesys->part->media, offset, sizeof(appfs_fat_t), fat) != sizeof(appfs_fat_t)) {
        logk(LOG_ERROR, "Too few bytes read from media (FAT entry)");
        return false;
    }
    return true;
}

// Go to the correct on-disk location for a file page.
static bool get_sect(file_t *file, uint8_t off) {
    appfs_fat_t fat;
    if (off < file->cur_off) {
        // Rewind to beginning.
        file->cur_off = 0;
        file->cur_sec = file->first_sec;
    }
    while (file->cur_off < off) {
        // Jump to next sector.
        if (!read_fat(file->filesys, file->cur_sec, &fat))
            return false;
        file->cur_off++;
        file->cur_sec = fat.next;
    }
    return true;
}

// File action function.
static diskoff_t appfs_file_action(file_t *file, diskoff_t offset, diskoff_t length, void *mem, bool is_mmap) {
    partition_t *part  = file->filesys->part;
    bootmedia_t *media = part->media;

    // Bounds checks.
    if (offset < 0 || offset > file->size) {
        logkf(
            LOG_WARN,
            "%{cs} at %{" FMT_TYPE_DISKOFF ";d} length %{" FMT_TYPE_DISKOFF ";d} out of bounds",
            is_mmap ? "mmap" : "read",
            offset,
            length
        );
        return -1;
    }
    if (length < 0) {
        logkf(
            LOG_WARN,
            "%{cs} at %{" FMT_TYPE_DISKOFF ";d} has negative length %{" FMT_TYPE_DISKOFF ";d}",
            is_mmap ? "mmap" : "read",
            offset,
            length
        );
        return -1;
    }
    if (offset + length > file->size) {
        logkf(
            LOG_WARN,
            "%{cs} at %{" FMT_TYPE_DISKOFF ";d} truncated from %{" FMT_TYPE_DISKOFF ";d} to %{" FMT_TYPE_DISKOFF ";d}",
            is_mmap ? "mmap" : "read",
            offset,
            length,
            file->size - offset
        );
        length = file->size - offset;
    }

    // Iterate over different pages of the access.
    uint8_t  *ptr  = mem;
    diskoff_t read = 0;
    while (length > 0) {
        // Get a temporary page to map to.
        if (!get_sect(file, offset / PAGE_SIZE))
            break;
        diskoff_t rom_addr = part->offset + PAGE_SIZE + file->cur_sec * PAGE_SIZE;

        // Page access start address.
        size_t start = offset % PAGE_SIZE;
        // Page access end address.
        size_t end   = start + length > PAGE_SIZE ? PAGE_SIZE : start + length;
        // Perform the action.
        if (is_mmap) {
            media->mmap(media, rom_addr + start, end - start, (size_t)mem + read);
        } else {
            media->read(media, rom_addr + start, end - start, ptr + read);
        }

        // Move on to the next page.
        offset += end - start;
        length -= end - start;
        read   += end - start;
    }

    return read;
}

// File reading function.
static diskoff_t appfs_file_read(file_t *file, diskoff_t offset, diskoff_t length, void *mem) {
    return appfs_file_action(file, offset, length, mem, false);
}

// File memory mapping function.
static bool appfs_file_mmap(file_t *file, diskoff_t offset, diskoff_t length, size_t vaddr) {
    return appfs_file_action(file, offset, length, (void *)vaddr, true) >= 0;
}

// Try to identify a filesystem.
static bool filesys_appfs_ident(partition_t *part) {
    bootmedia_t *media = part->media;
    // Check metadata 0.
    char         id[sizeof(appfs_magic)];
    if (media->read(media, part->offset, sizeof(appfs_magic), id) != sizeof(appfs_magic)) {
        logk(LOG_WARN, "Too few bytes read from media (header 0)");
        return false;
    }
    if (mem_equals(id, appfs_magic, sizeof(appfs_magic))) {
        return true;
    }

    // Check metadata 1.
    if (media->read(media, part->offset + PAGE_SIZE / 2, sizeof(appfs_magic), id) != sizeof(appfs_magic)) {
        logk(LOG_WARN, "Too few bytes read from media (header 1)");
        return false;
    }
    if (mem_equals(id, appfs_magic, sizeof(appfs_magic))) {
        return true;
    }

    // If neither has the magic value, it is not an AppFS.
    return false;
}

// Try to open the kernel file on this filesystem.
static bool filesys_appfs_read(partition_t *part, filesys_t *filesys, file_t *file) {
    logk(LOG_INFO, "Trying AppFS filesystem");

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
    hdr0_valid = mem_equals(&hdr0.magic, appfs_magic, sizeof(appfs_magic));
    hdr1_valid = mem_equals(&hdr1.magic, appfs_magic, sizeof(appfs_magic));

    // Select header to use.
    if (hdr0_valid && hdr1_valid) {
        filesys->active_header = hdr1.serial > hdr0.serial;
    } else if (hdr0_valid && !hdr1_valid) {
        filesys->active_header = 0;
    } else if (hdr1_valid && !hdr0_valid) {
        filesys->active_header = 1;
    } else {
        logk(LOG_ERROR, "Both AppFS headers invalid (this is a bug)");
        return false;
    }

    // Validate selected file handle.
    if (tobootloader.appfs_magic != APPFS_TOBOOTLOADER_MAGIC)
        return false;
    if (tobootloader.app == 255)
        return false;

    // Construct handles.
    filesys->part       = part;
    filesys->active_fat = filesys->active_header;
    file->filesys       = filesys;
    file->first_sec     = tobootloader.app;
    file->cur_off       = 0;
    file->cur_sec       = tobootloader.app;
    file->read          = appfs_file_read;
    file->mmap          = appfs_file_mmap;

    // Read first FAT entry.
    appfs_fat_t fat;
    if (!read_fat(filesys, file->first_sec, &fat))
        return false;
    file->size = fat.size;

    // Discard app.
    tobootloader.app = 255;

    return true;
}



// AppFS file system.
static filesys_type_t appfs_filesys = {
    .ident = filesys_appfs_ident,
    .read  = filesys_appfs_read,
};

// Register AppFS file system.
static void register_appfs_filesys() __attribute__((constructor));
static void register_appfs_filesys() {
    filesys_type_register(&appfs_filesys);
}

#endif
