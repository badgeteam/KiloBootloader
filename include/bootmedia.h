
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(BOOTMEDIA64) || __SIZE_MAX__ > __UINT32_MAX__
// Bootable media position.
typedef int64_t diskoff_t;
#define FMT_TYPE_DISKOFF "i64"
#else
// Bootable media position.
typedef int32_t diskoff_t;
#define FMT_TYPE_DISKOFF "i32"
#endif

// Abstract bootable device.
typedef struct bootmedia bootmedia_t;

#include "partsys.h"



// Bootable media random read function.
typedef diskoff_t (*bootmedia_read_t)(bootmedia_t *media, diskoff_t offset, diskoff_t length, void *mem);
// Bootable media memory map function.
typedef bool (*bootmedia_mmap_t)(bootmedia_t *media, diskoff_t offset, diskoff_t length, size_t vaddr);

// Abstract bootable device.
struct bootmedia {
    // Previous boot media.
    bootmedia_t     *prev;
    // Next boot media.
    bootmedia_t     *next;
    // Media ID.
    int              id;
    // Read function.
    bootmedia_read_t read;
    // Optional memory map function.
    bootmedia_mmap_t mmap;
    // Detected partitioning system, if any.
    partsys_t       *partsys;
    // Number of partitions.
    diskoff_t        part_num;
    // Selected partition, or -1 if none selected.
    diskoff_t        part_sel;
};



// First boot media.
extern bootmedia_t *bootmedia_first;
// Last boot media.
extern bootmedia_t *bootmedia_last;

// Register a new boot device.
// This should only be called from constructor functions.
void bootmedia_register(bootmedia_t *media);
