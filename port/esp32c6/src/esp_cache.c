
// SPDX-License-Identifier: MIT

#include "port/esp_cache.h"

#include "log.h"
#include "port/hardware.h"
#include "port/reg/esp_extmem.h"

// Initialise the cache HAL.
void esp_cache_init() {
}

// Disable caches.
void esp_cache_disable(int level_mask, esp_cache_type_t types) {
    if (level_mask != 1) {
        logkf(LOG_ERROR, "Invalid cache disable mask: %{x}", level_mask);
        return;
    }

    if (types & ESP_CACHE_TYPE_DATA) {
    }
}

// Enable caches.
void esp_cache_enable(int level_mask, esp_cache_type_t types) {
    if (level_mask != 1) {
        logkf(LOG_ERROR, "Invalid cache disable mask: %{x}", level_mask);
        return;
    }
}

// Query whether the cache is enabled.
bool esp_cache_is_enabled(int level_mask, esp_cache_type_t types) {
    if (level_mask != 1) {
        logkf(LOG_ERROR, "Invalid cache disable mask: %{x}", level_mask);
        return false;
    }
    return true;
}
