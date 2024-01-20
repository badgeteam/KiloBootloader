
// SPDX-License-Identifier: MIT

#pragma once

// Abstract partition system.
typedef struct partsys partsys_t;

#include "bootmedia.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



// Partition information.
typedef struct {
    // Offset on disk.
    diskoff_t offset;
    // Length on disk.
    diskoff_t length;
    // Is a bootable partition.
    bool      bootable;
    // Is a valid parition.
    bool      valid;
} partition_t;

typedef bool (*partsys_ident_t)(bootmedia_t *media);
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
