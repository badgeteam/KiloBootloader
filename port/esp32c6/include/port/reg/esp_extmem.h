
// Generated from ESP-IDF: https://github.com/espressif/esp-idf
// components/soc/esp32c6/include/soc/extmem_reg.h

#include <stdint.h>

typedef volatile struct {
    // The bit is used to disable core0 dbus access L1-Cache, 0: enable, 1: disable
    uint32_t extmem_l1_cache_shut_ibus : 1;
    // The bit is used to disable core0 dbus access L1-Cache, 0: enable, 1: disable
    uint32_t extmem_l1_cache_shut_dbus : 1;
    uint32_t                           : 30;
} extmem_extmem_l1_cache_ctrl_t;

typedef volatile struct {
    uint32_t                                  : 16;
    // The bit is used to close clock gating of  L1-Cache tag memory. 1: close gating,0: open clock gating.
    uint32_t extmem_l1_cache_tag_mem_force_on : 1;
    // The bit is used to power L1-Cache tag memory down. 0: follow rtc_lslp, 1: powerdown
    uint32_t extmem_l1_cache_tag_mem_force_pd : 1;
    // The bit is used to power L1-Cache tag memory up. 0: follow rtc_lslp, 1: power up
    uint32_t extmem_l1_cache_tag_mem_force_pu : 1;
    uint32_t                                  : 13;
} extmem_extmem_l1_cache_tag_mem_power_ctrl_t;

typedef volatile struct {
    uint32_t                                   : 16;
    // The bit is used to close clock gating of  L1-Cache data memory. 1: close gating, 0: open clock gating.
    uint32_t extmem_l1_cache_data_mem_force_on : 1;
    // The bit is used to power L1-Cache data memory down. 0: follow rtc_lslp, 1: power down
    uint32_t extmem_l1_cache_data_mem_force_pd : 1;
    // The bit is used to power L1-Cache data memory up. 0: follow rtc_lslp, 1: power up
    uint32_t extmem_l1_cache_data_mem_force_pu : 1;
    uint32_t                                   : 13;
} extmem_extmem_l1_cache_data_mem_power_ctrl_t;

typedef volatile struct {
    uint32_t                             : 16;
    // The bit is used to enable freeze operation on L1-Cache. It can be cleared by software.
    uint32_t extmem_l1_cache_freeze_en   : 1;
    // The bit is used to configure mode of freeze operation L1-Cache. 0: a miss-access will not stuck. 1: a miss-access
    // will stuck.
    uint32_t extmem_l1_cache_freeze_mode : 1;
    // The bit is used to indicate whether freeze operation on L1-Cache is finished ornot. 0: not finished. 1: finished.
    uint32_t extmem_l1_cache_freeze_done : 1;
    uint32_t                             : 13;
} extmem_extmem_l1_cache_freeze_ctrl_t;

typedef volatile struct {
    uint32_t                                : 16;
    // The bit is used to enable config-bus read L1-Cache data memoryory. 0: disable, 1: enable.
    uint32_t extmem_l1_cache_data_mem_rd_en : 1;
    // The bit is used to enable config-bus write L1-Cache data memoryory. 0: disable,1: enable.
    uint32_t extmem_l1_cache_data_mem_wr_en : 1;
    uint32_t                                : 14;
} extmem_extmem_l1_cache_data_mem_acs_conf_t;

typedef volatile struct {
    uint32_t                               : 16;
    // The bit is used to enable config-bus read L1-Cache tag memoryory. 0: disable, 1: enable.
    uint32_t extmem_l1_cache_tag_mem_rd_en : 1;
    // The bit is used to enable config-bus write L1-Cache tag memoryory. 0: disable, 1: enable.
    uint32_t extmem_l1_cache_tag_mem_wr_en : 1;
    uint32_t                               : 14;
} extmem_extmem_l1_cache_tag_mem_acs_conf_t;

typedef volatile struct {
    // The bit is used to enable the first section of prelock function on L1-Cache.
    uint32_t extmem_l1_cache_prelock_sct0_en : 1;
    // The bit is used to enable the second section of prelock function on L1-Cache.
    uint32_t extmem_l1_cache_prelock_sct1_en : 1;
    uint32_t                                 : 30;
} extmem_extmem_l1_cache_prelock_conf_t;

typedef volatile struct {
    // Those bits are used to configure the size of the first section of prelock on L1-Cache, which should be used
    // together with L1_CACHE_PRELOCK_SCT0_ADDR_REG
    uint32_t extmem_l1_cache_prelock_sct0_size : 14;
    uint32_t                                   : 2;
    // Those bits are used to configure the size of the second section of prelock on L1-Cache, which should be used
    // together with L1_CACHE_PRELOCK_SCT1_ADDR_REG
    uint32_t extmem_l1_cache_prelock_sct1_size : 14;
    uint32_t                                   : 2;
} extmem_extmem_l1_cache_prelock_sct_size_t;

typedef volatile struct {
    // The bit is used to enable lock operation. It will be cleared by hardware after lock operation done
    uint32_t extmem_l1_cache_lock_ena   : 1;
    // The bit is used to enable unlock operation. It will be cleared by hardware after unlock operation done
    uint32_t extmem_l1_cache_unlock_ena : 1;
    // The bit is used to indicate whether unlock/lock operation is finished or not. 0: not finished. 1: finished.
    uint32_t extmem_l1_cache_lock_done  : 1;
    uint32_t                            : 29;
} extmem_extmem_l1_cache_lock_ctrl_t;

typedef volatile struct {
    // The bit is used to enable invalidate operation. It will be cleared by hardware after invalidate operation done.
    // Note that this bit and the other sync-bits (clean_ena, writeback_ena, writeback_invalidate_ena) are mutually
    // exclusive, that is, those bits can not be set to 1 at the same time.
    uint32_t extmem_l1_cache_invalidate_ena           : 1;
    // The bit is used to enable clean operation. It will be cleared by hardware afterclean operation done. Note that
    // this bit and the other sync-bits (invalidate_ena, writeback_ena, writeback_invalidate_ena) are mutually
    // exclusive, that is, those bits can not be set to 1 at the same time.
    uint32_t extmem_l1_cache_clean_ena                : 1;
    // The bit is used to enable writeback operation. It will be cleared by hardware after writeback operation done.
    // Note that this bit and the other sync-bits (invalidate_ena, clean_ena, writeback_invalidate_ena) are mutually
    // exclusive, that is,those bits can not be set to 1 at the same time.
    uint32_t extmem_l1_cache_writeback_ena            : 1;
    // The bit is used to enable writeback-invalidate operation. It will be cleared byhardware after
    // writeback-invalidate operation done. Note that this bit and the other sync-bits (invalidate_ena, clean_ena,
    // writeback_ena) are mutually exclusive, that is, those bits can not be set to 1 at the same time.
    uint32_t extmem_l1_cache_writeback_invalidate_ena : 1;
    // The bit is used to indicate whether sync operation (invalidate, clean, writeback, writeback_invalidate) is
    // finished or not. 0: not finished. 1: finished.
    uint32_t extmem_l1_cache_sync_done                : 1;
    uint32_t                                          : 27;
} extmem_extmem_l1_cache_sync_ctrl_t;

typedef volatile struct {
    // The bit is used to enable preload operation on L1-Cache. It will be cleared by hardware automatically after
    // preload operation is done.
    uint32_t extmem_l1_cache_preload_ena   : 1;
    // The bit is used to indicate whether preload operation is finished or not. 0: not finished. 1: finished.
    uint32_t extmem_l1_cache_preload_done  : 1;
    // The bit is used to configure the direction of preload operation. 0: ascending, 1: descending.
    uint32_t extmem_l1_cache_preload_order : 1;
    // The bit is used to set  the gid of l1 cache preload.
    uint32_t extmem_l1_cache_preload_rgid  : 4;
    uint32_t                               : 25;
} extmem_extmem_l1_cache_preload_ctrl_t;

typedef volatile struct {
    // The bit is used to enable and disable autoload operation on L1-Cache.  1: enable, 0: disable.
    uint32_t extmem_l1_cache_autoload_ena          : 1;
    // The bit is used to indicate whether autoload operation on L1-Cache is finished or not. 0: not finished. 1:
    // finished.
    uint32_t extmem_l1_cache_autoload_done         : 1;
    // The bit is used to configure the direction of autoload operation on L1-Cache. 0: ascending. 1: descending.
    uint32_t extmem_l1_cache_autoload_order        : 1;
    // The field is used to configure trigger mode of autoload operation on L1-Cache. 0/3: miss-trigger, 1: hit-trigger,
    // 2: miss-hit-trigger.
    uint32_t extmem_l1_cache_autoload_trigger_mode : 2;
    uint32_t                                       : 3;
    // The bit is used to enable the first section for autoload operation on L1-Cache.
    uint32_t extmem_l1_cache_autoload_sct0_ena     : 1;
    // The bit is used to enable the second section for autoload operation on L1-Cache.
    uint32_t extmem_l1_cache_autoload_sct1_ena     : 1;
    uint32_t                                       : 22;
} extmem_extmem_l1_cache_autoload_ctrl_t;

typedef volatile struct {
    uint32_t                            : 4;
    // The bit is used to enable interrupt of one of counters overflow that occurs in L1-DCache due to bus0 accesses
    // L1-DCache.
    uint32_t extmem_l1_ibus_ovf_int_ena : 1;
    // The bit is used to enable interrupt of one of counters overflow that occurs in L1-DCache due to bus1 accesses
    // L1-DCache.
    uint32_t extmem_l1_dbus_ovf_int_ena : 1;
    uint32_t                            : 26;
} extmem_extmem_l1_cache_acs_cnt_int_ena_t;

typedef volatile struct {
    uint32_t                            : 4;
    // The bit is used to clear counters overflow interrupt and counters in L1-DCache due to bus0 accesses L1-DCache.
    uint32_t extmem_l1_ibus_ovf_int_clr : 1;
    // The bit is used to clear counters overflow interrupt and counters in L1-DCache due to bus1 accesses L1-DCache.
    uint32_t extmem_l1_dbus_ovf_int_clr : 1;
    uint32_t                            : 26;
} extmem_extmem_l1_cache_acs_cnt_int_clr_t;

typedef volatile struct {
    uint32_t                            : 4;
    // The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache due to bus0 accesses L1-DCache.
    uint32_t extmem_l1_ibus_ovf_int_raw : 1;
    // The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache due to bus1 accesses L1-DCache.
    uint32_t extmem_l1_dbus_ovf_int_raw : 1;
    uint32_t                            : 26;
} extmem_extmem_l1_cache_acs_cnt_int_raw_t;

typedef volatile struct {
    uint32_t                           : 4;
    // The bit indicates the interrupt status of one of counters overflow that occurs in L1-DCache due to bus0 accesses
    // L1-DCache.
    uint32_t extmem_l1_ibus_ovf_int_st : 1;
    // The bit indicates the interrupt status of one of counters overflow that occurs in L1-DCache due to bus1 accesses
    // L1-DCache.
    uint32_t extmem_l1_dbus_ovf_int_st : 1;
    uint32_t                           : 26;
} extmem_extmem_l1_cache_acs_cnt_int_st_t;

typedef volatile struct {
    uint32_t                        : 4;
    // The bit is used to enable dbus0 counter in L1-DCache.
    uint32_t extmem_l1_ibus_cnt_ena : 1;
    // The bit is used to enable dbus1 counter in L1-DCache.
    uint32_t extmem_l1_dbus_cnt_ena : 1;
    uint32_t                        : 14;
    // The bit is used to clear dbus0 counter in L1-DCache.
    uint32_t extmem_l1_ibus_cnt_clr : 1;
    // The bit is used to clear dbus1 counter in L1-DCache.
    uint32_t extmem_l1_dbus_cnt_clr : 1;
    uint32_t                        : 10;
} extmem_extmem_l1_cache_acs_cnt_ctrl_t;

typedef volatile struct {
    // The register records the ID of fail-access when cache accesses L1-Cache.
    uint32_t extmem_l1_cache_fail_id   : 16;
    // The register records the attribution of fail-access when cache accesses L1-Cache.
    uint32_t extmem_l1_cache_fail_attr : 16;
} extmem_extmem_l1_cache_acs_fail_id_attr_t;

typedef volatile struct {
    uint32_t                                   : 4;
    // The bit is used to enable interrupt of L1-Cache preload-operation. If preload operation is done, interrupt
    // occurs.
    uint32_t extmem_l1_cache_pld_done_int_ena  : 1;
    uint32_t                                   : 1;
    // The bit is used to enable interrupt of Cache sync-operation done.
    uint32_t extmem_l1_cache_sync_done_int_ena : 1;
    uint32_t                                   : 4;
    // The bit is used to enable interrupt of L1-Cache preload-operation error.
    uint32_t extmem_l1_cache_pld_err_int_ena   : 1;
    uint32_t                                   : 1;
    // The bit is used to enable interrupt of Cache sync-operation error.
    uint32_t extmem_l1_cache_sync_err_int_ena  : 1;
    uint32_t                                   : 18;
} extmem_extmem_l1_cache_sync_preload_int_ena_t;

typedef volatile struct {
    uint32_t                                   : 4;
    // The bit is used to clear interrupt that occurs only when L1-Cache preload-operation is done.
    uint32_t extmem_l1_cache_pld_done_int_clr  : 1;
    uint32_t                                   : 1;
    // The bit is used to clear interrupt that occurs only when Cache sync-operation is done.
    uint32_t extmem_l1_cache_sync_done_int_clr : 1;
    uint32_t                                   : 4;
    // The bit is used to clear interrupt of L1-Cache preload-operation error.
    uint32_t extmem_l1_cache_pld_err_int_clr   : 1;
    uint32_t                                   : 1;
    // The bit is used to clear interrupt of Cache sync-operation error.
    uint32_t extmem_l1_cache_sync_err_int_clr  : 1;
    uint32_t                                   : 18;
} extmem_extmem_l1_cache_sync_preload_int_clr_t;

typedef volatile struct {
    uint32_t                                   : 4;
    // The raw bit of the interrupt that occurs only when L1-Cache preload-operation is done.
    uint32_t extmem_l1_cache_pld_done_int_raw  : 1;
    uint32_t                                   : 1;
    // The raw bit of the interrupt that occurs only when Cache sync-operation is done.
    uint32_t extmem_l1_cache_sync_done_int_raw : 1;
    uint32_t                                   : 4;
    // The raw bit of the interrupt that occurs only when L1-Cache preload-operation error occurs.
    uint32_t extmem_l1_cache_pld_err_int_raw   : 1;
    uint32_t                                   : 1;
    // The raw bit of the interrupt that occurs only when Cache sync-operation error occurs.
    uint32_t extmem_l1_cache_sync_err_int_raw  : 1;
    uint32_t                                   : 18;
} extmem_extmem_l1_cache_sync_preload_int_raw_t;

typedef volatile struct {
    uint32_t                                  : 4;
    // The bit indicates the status of the interrupt that occurs only when L1-Cache preload-operation is done.
    uint32_t extmem_l1_cache_pld_done_int_st  : 1;
    uint32_t                                  : 1;
    // The bit indicates the status of the interrupt that occurs only when Cache sync-operation is done.
    uint32_t extmem_l1_cache_sync_done_int_st : 1;
    uint32_t                                  : 4;
    // The bit indicates the status of the interrupt of L1-Cache preload-operation error.
    uint32_t extmem_l1_cache_pld_err_int_st   : 1;
    uint32_t                                  : 1;
    // The bit indicates the status of the interrupt of Cache sync-operation error.
    uint32_t extmem_l1_cache_sync_err_int_st  : 1;
    uint32_t                                  : 18;
} extmem_extmem_l1_cache_sync_preload_int_st_t;

typedef volatile struct {
    uint32_t                               : 8;
    // The value 2 is Only available which means preload size is error in L1-Cache.
    uint32_t extmem_l1_cache_pld_err_code  : 2;
    uint32_t                               : 2;
    // The values 0-2 are available which means sync map, command conflict and size are error in Cache System.
    uint32_t extmem_l1_cache_sync_err_code : 2;
    uint32_t                               : 18;
} extmem_extmem_l1_cache_sync_preload_exception_t;

typedef volatile struct {
    uint32_t                            : 4;
    // Set this bit to set L1-Cache tag memory as object. This bit should be onehot with the others fields inside this
    // register.
    uint32_t extmem_l1_cache_tag_object : 1;
    uint32_t                            : 5;
    // Set this bit to set L1-Cache data memory as object. This bit should be onehot with the others fields inside this
    // register.
    uint32_t extmem_l1_cache_mem_object : 1;
    uint32_t                            : 21;
} extmem_extmem_l1_cache_object_ctrl_t;

typedef struct {
    // Padding.
    volatile uint32_t             _padding0[1];
    extmem_extmem_l1_cache_ctrl_t extmem_l1_cache_ctrl;
    // Padding.
    volatile uint32_t             _padding1[6];
    volatile struct {
        uint32_t                                                       : 4;
        // Set this bit as 1 to enable L1-DCache wrap around mode.
        uint32_t extmem_l1_cache_wrap_around_ctrl_extmem_l1_cache_wrap : 1;
        uint32_t                                                       : 27;
    };
    extmem_extmem_l1_cache_tag_mem_power_ctrl_t  extmem_l1_cache_tag_mem_power_ctrl;
    extmem_extmem_l1_cache_data_mem_power_ctrl_t extmem_l1_cache_data_mem_power_ctrl;
    extmem_extmem_l1_cache_freeze_ctrl_t         extmem_l1_cache_freeze_ctrl;
    extmem_extmem_l1_cache_data_mem_acs_conf_t   extmem_l1_cache_data_mem_acs_conf;
    extmem_extmem_l1_cache_tag_mem_acs_conf_t    extmem_l1_cache_tag_mem_acs_conf;
    // Padding.
    volatile uint32_t                            _padding2[16];
    extmem_extmem_l1_cache_prelock_conf_t        extmem_l1_cache_prelock_conf;
    // Those bits are used to configure the start virtual address of the first sectionof prelock on L1-Cache, which
    // should be used together with L1_CACHE_PRELOCK_SCT0_SIZE_REG
    volatile uint32_t                            extmem_l1_cache_prelock_sct0_addr;
    // Those bits are used to configure the start virtual address of the second section of prelock on L1-Cache, which
    // should be used together with L1_CACHE_PRELOCK_SCT1_SIZE_REG
    volatile uint32_t                            extmem_l1_cache_prelock_sct1_addr;
    extmem_extmem_l1_cache_prelock_sct_size_t    extmem_l1_cache_prelock_sct_size;
    extmem_extmem_l1_cache_lock_ctrl_t           extmem_l1_cache_lock_ctrl;
    volatile struct {
        // Those bits are used to indicate which caches in the two-level cache structure will apply this lock/unlock
        // operation. [4]: L1-Cache
        uint32_t extmem_l1_cache_lock_map_extmem_l1_cache_lock_map : 6;
        uint32_t                                                   : 26;
    };
    // Those bits are used to configure the start virtual address of the lock/unlock operation, which should be used
    // together with CACHE_LOCK_SIZE_REG
    volatile uint32_t extmem_l1_cache_lock_addr;
    volatile struct {
        // Those bits are used to configure the size of the lock/unlock operation, which should be used together with
        // CACHE_LOCK_ADDR_REG
        uint32_t extmem_l1_cache_lock_size_extmem_l1_cache_lock_size : 16;
        uint32_t                                                     : 16;
    };
    extmem_extmem_l1_cache_sync_ctrl_t extmem_l1_cache_sync_ctrl;
    volatile struct {
        // Those bits are used to indicate which caches in the two-level cache structure will apply the sync operation.
        // [4]: L1-Cache
        uint32_t extmem_l1_cache_sync_map_extmem_l1_cache_sync_map : 6;
        uint32_t                                                   : 26;
    };
    // Those bits are used to configure the start virtual address of the sync operation, which should be used together
    // with CACHE_SYNC_SIZE_REG
    volatile uint32_t extmem_l1_cache_sync_addr;
    volatile struct {
        // Those bits are used to configure the size of the sync operation, which should be used together with
        // CACHE_SYNC_ADDR_REG
        uint32_t extmem_l1_cache_sync_size_extmem_l1_cache_sync_size : 24;
        uint32_t                                                     : 8;
    };
    // Padding.
    volatile uint32_t                     _padding3[12];
    extmem_extmem_l1_cache_preload_ctrl_t extmem_l1_cache_preload_ctrl;
    // Those bits are used to configure the start virtual address of preload on L1-Cache, which should be used together
    // with L1_CACHE_PRELOAD_SIZE_REG
    volatile uint32_t                     extmem_l1_dcache_preload_addr;
    volatile struct {
        // Those bits are used to configure the size of the first section of prelock on L1-Cache, which should be used
        // together with L1_CACHE_PRELOAD_ADDR_REG
        uint32_t extmem_l1_dcache_preload_size_extmem_l1_cache_preload_size : 14;
        uint32_t                                                            : 18;
    };
    // Padding.
    volatile uint32_t                      _padding4[20];
    extmem_extmem_l1_cache_autoload_ctrl_t extmem_l1_cache_autoload_ctrl;
    // Those bits are used to configure the start virtual address of the first sectionfor autoload operation on
    // L1-Cache. Note that it should be used together with L1_CACHE_AUTOLOAD_SCT0_SIZE and L1_CACHE_AUTOLOAD_SCT0_ENA.
    volatile uint32_t                      extmem_l1_cache_autoload_sct0_addr;
    volatile struct {
        // Those bits are used to configure the size of the first section for autoload operation on L1-Cache. Note that
        // it should be used together with L1_CACHE_AUTOLOAD_SCT0_ADDR and L1_CACHE_AUTOLOAD_SCT0_ENA.
        uint32_t extmem_l1_cache_autoload_sct0_size_extmem_l1_cache_autoload_sct0_size : 28;
        uint32_t                                                                       : 4;
    };
    // Those bits are used to configure the start virtual address of the second section for autoload operation on
    // L1-Cache. Note that it should be used together with L1_CACHE_AUTOLOAD_SCT1_SIZE and L1_CACHE_AUTOLOAD_SCT1_ENA.
    volatile uint32_t extmem_l1_cache_autoload_sct1_addr;
    volatile struct {
        // Those bits are used to configure the size of the second section for autoload operation on L1-Cache. Note that
        // it should be used together with L1_CACHE_AUTOLOAD_SCT1_ADDR and L1_CACHE_AUTOLOAD_SCT1_ENA.
        uint32_t extmem_l1_cache_autoload_sct1_size_extmem_l1_cache_autoload_sct1_size : 28;
        uint32_t                                                                       : 4;
    };
    // Padding.
    volatile uint32_t                        _padding5[4];
    extmem_extmem_l1_cache_acs_cnt_int_ena_t extmem_l1_cache_acs_cnt_int_ena;
    extmem_extmem_l1_cache_acs_cnt_int_clr_t extmem_l1_cache_acs_cnt_int_clr;
    extmem_extmem_l1_cache_acs_cnt_int_raw_t extmem_l1_cache_acs_cnt_int_raw;
    extmem_extmem_l1_cache_acs_cnt_int_st_t  extmem_l1_cache_acs_cnt_int_st;
    volatile struct {
        uint32_t                                                               : 4;
        // The bit is used to enable interrupt of access fail that occurs in L1-DCache dueto cpu accesses L1-DCache.
        uint32_t extmem_l1_cache_acs_fail_int_ena_extmem_l1_cache_fail_int_ena : 1;
        uint32_t                                                               : 27;
    };
    volatile struct {
        uint32_t                                                               : 4;
        // The bit is used to clear interrupt of access fail that occurs in L1-DCache due to cpu accesses L1-DCache.
        uint32_t extmem_l1_cache_acs_fail_int_clr_extmem_l1_cache_fail_int_clr : 1;
        uint32_t                                                               : 27;
    };
    volatile struct {
        uint32_t                                                               : 4;
        // The raw bit of the interrupt of access fail that occurs in L1-DCache.
        uint32_t extmem_l1_cache_acs_fail_int_raw_extmem_l1_cache_fail_int_raw : 1;
        uint32_t                                                               : 27;
    };
    volatile struct {
        uint32_t                                                             : 4;
        // The bit indicates the interrupt status of access fail that occurs in L1-DCache due to cpu accesses L1-DCache.
        uint32_t extmem_l1_cache_acs_fail_int_st_extmem_l1_cache_fail_int_st : 1;
        uint32_t                                                             : 27;
    };
    extmem_extmem_l1_cache_acs_cnt_ctrl_t           extmem_l1_cache_acs_cnt_ctrl;
    // Padding.
    volatile uint32_t                               _padding6[16];
    // The register records the number of hits when bus0 accesses L1-Cache.
    volatile uint32_t                               extmem_l1_ibus_acs_hit_cnt;
    // The register records the number of missing when bus0 accesses L1-Cache.
    volatile uint32_t                               extmem_l1_ibus_acs_miss_cnt;
    // The register records the number of access-conflicts when bus0 accesses L1-Cache.
    volatile uint32_t                               extmem_l1_ibus_acs_conflict_cnt;
    // The register records the number of times that L1-Cache accesses L2-Cache due tobus0 accessing L1-Cache.
    volatile uint32_t                               extmem_l1_ibus_acs_nxtlvl_cnt;
    // The register records the number of hits when bus1 accesses L1-Cache.
    volatile uint32_t                               extmem_l1_dbus_acs_hit_cnt;
    // The register records the number of missing when bus1 accesses L1-Cache.
    volatile uint32_t                               extmem_l1_dbus_acs_miss_cnt;
    // The register records the number of access-conflicts when bus1 accesses L1-Cache.
    volatile uint32_t                               extmem_l1_dbus_acs_conflict_cnt;
    // The register records the number of times that L1-Cache accesses L2-Cache due tobus1 accessing L1-Cache.
    volatile uint32_t                               extmem_l1_dbus_acs_nxtlvl_cnt;
    // Padding.
    volatile uint32_t                               _padding7[16];
    extmem_extmem_l1_cache_acs_fail_id_attr_t       extmem_l1_cache_acs_fail_id_attr;
    // The register records the address of fail-access when cache accesses L1-Cache.
    volatile uint32_t                               extmem_l1_cache_acs_fail_addr;
    extmem_extmem_l1_cache_sync_preload_int_ena_t   extmem_l1_cache_sync_preload_int_ena;
    extmem_extmem_l1_cache_sync_preload_int_clr_t   extmem_l1_cache_sync_preload_int_clr;
    extmem_extmem_l1_cache_sync_preload_int_raw_t   extmem_l1_cache_sync_preload_int_raw;
    extmem_extmem_l1_cache_sync_preload_int_st_t    extmem_l1_cache_sync_preload_int_st;
    extmem_extmem_l1_cache_sync_preload_exception_t extmem_l1_cache_sync_preload_exception;
    volatile struct {
        uint32_t                                                        : 4;
        // set this bit to reset sync-logic inside L1-Cache. Recommend that this should only be used to initialize
        // sync-logic when some fatal error of sync-logic occurs.
        uint32_t extmem_l1_cache_sync_rst_ctrl_extmem_l1_cache_sync_rst : 1;
        uint32_t                                                        : 27;
    };
    volatile struct {
        uint32_t                                                          : 4;
        // set this bit to reset preload-logic inside L1-Cache. Recommend that this shouldonly be used to initialize
        // preload-logic when some fatal error of preload-logicoccurs.
        uint32_t extmem_l1_cache_preload_rst_ctrl_extmem_l1_cache_pld_rst : 1;
        uint32_t                                                          : 27;
    };
    volatile struct {
        uint32_t                                                                   : 4;
        // set this bit to clear autoload-buffer inside L1-Cache. If this bit is active, autoload will not work in
        // L1-Cache. This bit should not be active when autoload works in L1-Cache.
        uint32_t extmem_l1_cache_autoload_buf_clr_ctrl_extmem_l1_cache_ald_buf_clr : 1;
        uint32_t                                                                   : 27;
    };
    volatile struct {
        uint32_t                                                               : 4;
        // The bit is used to clear the unallocate request buffer of l1 cache where the unallocate request is responsed
        // but not completed.
        uint32_t extmem_l1_unallocate_buffer_clear_extmem_l1_cache_unalloc_clr : 1;
        uint32_t                                                               : 27;
    };
    extmem_extmem_l1_cache_object_ctrl_t extmem_l1_cache_object_ctrl;
    volatile struct {
        // Set this bits to select which way of the tag-object will be accessed. 0: way0, 1: way1, 2: way2, 3: way3, ?,
        // 7: way7.
        uint32_t extmem_l1_cache_way_object_extmem_l1_cache_way_object : 3;
        uint32_t                                                       : 29;
    };
    // Those bits stores the virtual address which will decide where inside the specified tag memory object will be
    // accessed.
    volatile uint32_t extmem_l1_cache_vaddr;
    // This is a constant place where we can write data to or read data from the tag/data memory on the specified cache.
    volatile uint32_t extmem_l1_cache_debug_bus;
    // Padding.
    volatile uint32_t _padding8[105];
    volatile struct {
        // version control register. Note that this default value stored is the latest date when the hardware logic was
        // updated.
        uint32_t extmem_date_extmem_date : 28;
        uint32_t                         : 4;
    };
} extmem_t;
