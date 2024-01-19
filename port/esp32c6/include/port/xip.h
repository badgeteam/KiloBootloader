
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

// Minimum XIP page size.
#define XIP_REGION_MIN_SIZE (8 * 1024)
// Maximum XIP page size.
#define XIP_REGION_MAX_SIZE (64 * 1024)
// Maximum number of physical (ROM) pages.
#define XIP_PAGE_PHYS_MAX   256
// Maximum number of virtual (mapped) pages.
#define XIP_PAGE_VIRT_MAX   256
// XIP MMU supports memory protection flags.
#define XIP_MMU_PROTECTED   false
