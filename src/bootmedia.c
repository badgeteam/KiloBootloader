
// SPDX-License-Identifier: MIT

#include "bootmedia.h"



// First boot media.
bootmedia_t *bootmedia_first = NULL;
// Last boot media.
bootmedia_t *bootmedia_last  = NULL;

// Register a new boot device.
// This should only be called from constructor functions.
void bootmedia_register(bootmedia_t *media) {
    // Media is added to the end of the list.
    media->next = NULL;

    if (bootmedia_first) {
        // Second or later boot media to be registered.
        media->prev          = bootmedia_last;
        bootmedia_last->next = media;
        bootmedia_last       = media;
    } else {
        // First boot media to be registered.
        bootmedia_first = media;
        bootmedia_last  = media;
        media->prev     = NULL;
    }
}
