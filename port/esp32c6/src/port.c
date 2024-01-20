
// SPDX-License-Identifier: MIT

#include "port.h"

#include "port/esp_cache.h"

// Perform early initialization of the port-specific hardware.
void port_early_init() {
}

// Perform full initialization of the port-specific hardware.
void port_init() {
    esp_cache_init();
}
