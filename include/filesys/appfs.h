
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

// AppFS to bootloader magic.
#define APPFS_TOBOOTLOADER_MAGIC 0x89778e48441c5665

// To bootloader data.
typedef struct {
    uint64_t appfs_magic;
    uint8_t  app;
    uint8_t  padding[7];
    char     arg[64];
} tobootloader_t;
