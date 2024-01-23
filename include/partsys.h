
// SPDX-License-Identifier: MIT

#pragma once

#ifndef PART_NAME_MAX
// Maximum partition name length.
#define PART_NAME_MAX 15
#endif

// Abstract partition system.
typedef struct partsys partsys_t;

#include "bootmedia.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



// Partition information.
typedef struct partition partition_t;
// Partition information.
struct partition {
    // Parent media.
    bootmedia_t *media;
    // Partition flags.
    struct {
        // Partition is bootable.
        uint32_t bootable : 1;
        uint32_t          : 31;
    } flags;
    // Offset on disk.
    diskoff_t offset;
    // Length on disk.
    diskoff_t length;
    // Partition name.
    char      name[PART_NAME_MAX + 1];
};

typedef size_t (*partsys_ident_t)(bootmedia_t *media);
typedef partition_t (*partsys_read_t)(bootmedia_t *media, diskoff_t part_index);

// Abstract partition system.
struct partsys {
    // Previous partition system.
    partsys_t      *prev;
    // Next partition system.
    partsys_t      *next;
    // Try to identify and read a partitioning system.
    partsys_ident_t ident;
    // Read a partition entry.
    partsys_read_t  read;
};



// First partition system.
extern partsys_t *partsys_first;
// Last partition system.
extern partsys_t *partsys_last;

// Register a new partition system.
// This should only be called from constructor functions.
void partsys_register(partsys_t *protocol);
