
// SPDX-License-Identifier: MIT

#pragma once

// Abstract filesystem type.
typedef struct filesys_type filesys_type_t;
// Mounted filesystem information.
typedef struct filesys      filesys_t;
// Abstract file handle.
typedef struct file         file_t;

#include "bootmedia.h"
#include "partsys.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



typedef bool (*filesys_ident_t)(partition_t *part);
typedef bool (*filesys_read_t)(partition_t *part, filesys_t *filesys, file_t *file);
typedef diskoff_t (*file_read_t)(file_t *file, diskoff_t offset, diskoff_t length, void *mem);
typedef bool (*file_mmap_t)(file_t *file, diskoff_t offset, diskoff_t length, size_t vaddr);

// Abstract filesystem type.
struct filesys_type {
    // Previous filesystem type.
    filesys_type_t *prev;
    // Next filesystem type.
    filesys_type_t *next;
    // Try to identify a filesystem.
    filesys_ident_t ident;
    // Try to open the kernel file on this filesystem.
    filesys_read_t  read;
};

// Mounted filesystem information.
struct filesys {
    // Parent partition.
    partition_t *part;
    // Sector size, if any.
    diskoff_t    sect_size;
    // Active filesystem header, if any.
    int          active_header;
    // Active FAT, if any.
    int          active_fat;
};

// Abstract file handle.
struct file {
    // Parent file system.
    filesys_t  *filesys;
    // File size.
    diskoff_t   size;
    // File reading function.
    file_read_t read;
    // File memory mapping function.
    file_mmap_t mmap;
    // File inode, if any.
    int         inode;
    // File first sector number, if any.
    diskoff_t   first_sec;
    // File current offset, if any.
    diskoff_t   cur_off;
    // File current sector number, if any.
    diskoff_t   cur_sec;
};



// First filesystem type.
extern filesys_type_t *filesys_type_first;
// Last filesystem type.
extern filesys_type_t *filesys_type_last;
// Number of filesystem types.
extern size_t          filesys_type_num;

#ifdef ALLOW_UNFORMATTED_PARTITION
// Dummy filesystem type for unformatted partitions.
extern filesys_type_t filesys_type_raw;
#endif

// Register a new filesystem type.
// This should only be called from constructor functions.
void filesys_type_register(filesys_type_t *type);
