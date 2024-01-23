
// SPDX-License-Identifier: MIT

#pragma once

#include "attributes.h"

#include <stddef.h>
#include <stdint.h>



#define CRC32_FMT          "%{u32;x}"
#define CRC32_FMT_ARG(crc) (crc)



// CRC32 state.
typedef uint32_t crc32_t;
// Initialize a CRC32 checksum.
#define crc32_init() ((crc32_t){-1})
// Add a byte to a CRC32 checksum.
crc32_t _crc32_byte(crc32_t crc, uint8_t val) CONST;
// Update a CRC32 checksum.
crc32_t _crc32_update(crc32_t crc, void const *mem, size_t len) PURE;
// Add a byte to a CRC32 checksum.
#define crc32_byte(crc, val)                                                                                           \
    do {                                                                                                               \
        crc32_t *ptr = (crc);                                                                                          \
        *ptr         = _crc32_byte(*ptr, (val));                                                                       \
    } while (0)
// Update a CRC32 checksum.
#define crc32_update(crc, mem, len)                                                                                    \
    do {                                                                                                               \
        crc32_t *ptr = (crc);                                                                                          \
        *ptr         = _crc32_update(*ptr, (mem), (len));                                                              \
    } while (0)
// Finalize a CRC32 checksum.
#define crc32_final(crc)                                                                                               \
    do {                                                                                                               \
        *(crc) ^= 0xffffffff;                                                                                          \
    } while (0)
