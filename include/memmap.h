
// SPDX-License-Identifier: MIT

#pragma once

// NOLINTBEGIN
extern char const __start_xip[];
extern char const __stop_xip[];
extern char const __start_sram[];
extern char const __stop_sram[];

#define IS_XIP_RANGE(x, l)  ((char *)(x) >= __start_xip && (char *)(x) + (l) <= __stop_xip)
#define IS_SRAM_RANGE(x, l) ((char *)(x) >= __start_sram && (char *)(x) + (l) <= __stop_sram)
// NOLINTEND
