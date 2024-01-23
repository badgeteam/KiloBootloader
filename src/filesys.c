
// SPDX-License-Identifier: MIT

#include "filesys.h"



// First filesystem type.
filesys_type_t *filesys_first = NULL;
// Last filesystem type.
filesys_type_t *filesys_last  = NULL;

// Register a new filesystem type.
// This should only be called from constructor functions.
void filesys_register(filesys_type_t *filesys_type) {
    // Protocol is added to the end of the list.
    filesys_type->next = NULL;

    if (filesys_first) {
        // Second or later filesystem type to be registered.
        filesys_type->prev = filesys_last;
        filesys_last->next = filesys_type;
        filesys_last       = filesys_type;
    } else {
        // First filesystem type to be registered.
        filesys_first      = filesys_type;
        filesys_last       = filesys_type;
        filesys_type->prev = NULL;
    }
}
