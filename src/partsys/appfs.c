
// SPDX-License-Identifier: MIT

#ifdef HAS_PARTSYS_APPFS

#include "partsys.h"

// Try to identify and read a partitioning system.
static bool partsys_appfs_ident(bootmedia_t *media) {
    return false;
}

// Read a partition entry.
static partition_t partsys_appfs_read(bootmedia_t *media, diskoff_t part_index) {
    return (partition_t){.valid = false};
}



// AppFS partitioning system.
static partsys_t appfs_partsys = {
    .ident = partsys_appfs_ident,
    .read  = partsys_appfs_read,
};

// Register AppFS partitioning system.
static void register_appfs_partsys() __attribute__((constructor));
static void register_appfs_partsys() {
    partsys_register(&appfs_partsys);
}

#endif
