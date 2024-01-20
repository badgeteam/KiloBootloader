
// SPDX-License-Identifier: MIT

#include "partsys.h"



// First partition system.
partsys_t *partsys_first = NULL;
// Last partition system.
partsys_t *partsys_last  = NULL;

// Register a new boot device.
// This should only be called from constructor functions.
void partsys_register(partsys_t *partsys) {
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
