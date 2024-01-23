
// SPDX-License-Identifier: MIT

#include "partsys.h"



// First partition system.
partsys_t *partsys_first = NULL;
// Last partition system.
partsys_t *partsys_last  = NULL;
// Number of partitioning systems.
size_t     partsys_num   = 0;

// Register a new partitioning system.
// This should only be called from constructor functions.
void partsys_register(partsys_t *partsys) {
    partsys_num++;

    // Protocol is added to the end of the list.
    partsys->next = NULL;

    if (partsys_first) {
        // Second or later partition system to be registered.
        partsys->prev      = partsys_last;
        partsys_last->next = partsys;
        partsys_last       = partsys;
    } else {
        // First partition system to be registered.
        partsys_first = partsys;
        partsys_last  = partsys;
        partsys->prev = NULL;
    }
}

#ifdef ALLOW_UNPARTITIONED_MEDIA
// Read a partition entry.
partition_t partsys_raw_read(bootmedia_t *media, diskoff_t partnum) {
    (void)partnum;
    return (partition_t){
        .offset = 0,
        .length = media->size,
    };
}

// Dummy partition system for unpartitioned media.
partsys_t partsys_raw = {
    .read = partsys_raw_read,
};
#endif
