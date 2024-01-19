
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Types of cache in an ESP32.
typedef enum {
    ESP_CACHE_TYPE_NONR        = 0,
    ESP_CACHE_TYPE_DATA        = 1,
    ESP_CACHE_TYPE_INSTRUCTION = 2,
    ESP_CACHE_TYPE_ALL         = 3,
} esp_cache_type_t;

// Initialise the cache HAL.
void esp_cache_init();
// Disable caches.
void esp_cache_disable(int level_mask, esp_cache_type_t types);
// Enable caches.
void esp_cache_enable(int level_mask, esp_cache_type_t types);
// Query whether the cache is enabled.
bool esp_cache_is_enabled(int level_mask, esp_cache_type_t types);
