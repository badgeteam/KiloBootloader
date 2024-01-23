
// SPDX-License-Identifier: MIT

#pragma once

#include "filesys.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



// Abstract boot protocol.
typedef struct bootprotocol bootprotocol_t;

typedef bool (*bootprotocol_func_t)(file_t *file);

// Abstract boot protocol.
struct bootprotocol {
    // Previous boot protocol.
    bootprotocol_t     *prev;
    // Next boot protocol.
    bootprotocol_t     *next;
    // Identify function.
    bootprotocol_func_t ident;
    // Boot function.
    bootprotocol_func_t boot;
};



// First boot protocol.
extern bootprotocol_t *bootprotocol_first;
// Last boot protocol.
extern bootprotocol_t *bootprotocol_last;
// Number of boot protocols.
extern size_t          bootprotocol_num;

// Register a new boot protocol.
// This should only be called from constructor functions.
void bootprotocol_register(bootprotocol_t *protocol);
