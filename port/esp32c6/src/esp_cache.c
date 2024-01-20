
// SPDX-License-Identifier: MIT

#include "port/esp_cache.h"

#include "log.h"
#include "port/esp_rom.h"
#include "port/hardware.h"
#include "port/reg/esp_extmem.h"

// Shared cache for instruction and data.
#define SOC_SHARED_IDCACHE_SUPPORTED  true
// Cache supports freeze operation.
#define SOC_CACHE_FREEZE_SUPPORTED    true
// Cache supports invalidation.
#define SOC_CACHE_INALIDATE_SUPPORTED false

#define EXTMEM (*(extmem_t *)EXTMEM_BASE)



// Whether to enable autoload on instruction cache.
static uint32_t icache_autoload;
#if !SOC_SHARED_IDCACHE_SUPPORTED
// Whether to enable autoload on data cache.
static uint32_t dcache_autoload;
#endif
// Cache line size.
static size_t line_size;
// Whether cache is enabled.
static bool   enabled;

// Initialise the cache HAL.
void esp_cache_init() {
    // Enable caches.
    icache_autoload = Cache_Disable_ICache();
    Cache_Enable_ICache(icache_autoload);
#if !SOC_SHARED_IDCACHE_SUPPORTED
    dcache_autoload = Cache_Disable_DCache();
    Cache_Enable_DCache(dcache_autoload);
#endif
    enabled = true;

    // Enable cache busses.
    EXTMEM.extmem_l1_cache_ctrl.extmem_l1_cache_shut_dbus = 0;
    EXTMEM.extmem_l1_cache_ctrl.extmem_l1_cache_shut_ibus = 0;

    // Query cache line size.
    line_size = Cache_Get_ICache_Line_Size();
}

// Disable caches.
void esp_cache_disable() {
    Cache_Disable_ICache();
#if !SOC_SHARED_IDCACHE_SUPPORTED
    Cache_Disable_DCache();
#endif
    enabled = false;
}

// Enable caches.
void esp_cache_enable() {
    Cache_Enable_ICache(icache_autoload);
#if !SOC_SHARED_IDCACHE_SUPPORTED
    Cache_Enable_DCache(dcache_autoload);
#endif
    enabled = true;
}

// Try to flush a cache range, if supported.
bool esp_cache_flush(size_t addr, size_t len) {
#if SOC_CACHE_INALIDATE_SUPPORTED
    return !Cache_Invalidate_Addr(addr, len);
#else
    (void)addr;
    (void)len;
    return esp_cache_flush_all();
#endif
}

// Try to flush the entire cache, if supported.
bool esp_cache_flush_all() {
    if (enabled) {
        esp_cache_disable();
        esp_cache_enable();
    }
    return true;
}

// Query whether the cache is enabled.
bool esp_cache_is_enabled() {
    return true;
}
