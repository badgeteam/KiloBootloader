
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

// Perform early initialization of the port-specific hardware.
void port_early_init();
// Perform full initialization of the port-specific hardware.
void port_init();
// Pre-control handover checks and settings.
bool port_pre_handover();
