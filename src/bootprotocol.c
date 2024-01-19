
// SPDX-License-Identifier: MIT

#include "bootprotocol.h"



// First boot protocol.
bootprotocol_t *bootprotocol_first = NULL;
// Last boot protocol.
bootprotocol_t *bootprotocol_last  = NULL;

// Register a new boot device.
// This should only be called from constructor functions.
void bootprotocol_register(bootprotocol_t *protocol) {
    // Protocol is added to the end of the list.
    protocol->next = NULL;

    if (bootprotocol_first) {
        // Second or later boot protocol to be registered.
        protocol->prev          = bootprotocol_last;
        bootprotocol_last->next = protocol;
        bootprotocol_last       = protocol;
    } else {
        // First boot protocol to be registered.
        bootprotocol_first = protocol;
        bootprotocol_last  = protocol;
        protocol->prev     = NULL;
    }
}
