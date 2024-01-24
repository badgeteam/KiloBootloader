
// SPDX-License-Identifier: MIT

#include "port.h"

#include "modem/modem_lpcon_struct.h"
#include "modem/modem_syscon_struct.h"
#include "port/esp_cache.h"
#include "port/hardware.h"
#include "soc/lp_aon_struct.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lp_timer_struct.h"
#include "soc/lp_wdt_struct.h"
#include "soc/pcr_struct.h"
#include "soc/pmu_struct.h"
#include "soc/regi2c_bbpll.h"
#include "soc/regi2c_defs.h"
#include "soc/timer_group_struct.h"

#define PMU_MODE_HP_ACTIVE 0
#define PMU_MODE_HP_MODEM  1
#define PMU_MODE_HP_SLEEP  2
#define PMU_MODE_LP_ACTIVE 0
#define PMU_MODE_LP_SLEEP  1



uint8_t esp_rom_regi2c_read(uint8_t block, uint8_t host_id, uint8_t reg_add);
uint8_t esp_rom_regi2c_read_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);

void esp_rom_regi2c_write(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
void esp_rom_regi2c_write_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);
void esp_rom_set_cpu_ticks_per_us(uint32_t ticks);
void esp_rom_delay_us(uint32_t us);

// Enable ESP32C6 PLL.
static void c6_enable_pll() {
    // Enable PLL.
    PMU.imm.clk_power.tie_low_xpd_bb_i2c       = true;
    PMU.imm.clk_power.tie_low_xpd_bbpll_i2c    = true;
    PMU.imm.clk_power.tie_low_global_bbpll_icg = true;

    // Calibrate PLL.
    MODEM_LPCON.clk_conf.clk_i2c_mst_en = true;
    REG_CLR_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_HIGH);
    REG_SET_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_LOW);

    // Send calibration command.
    uint8_t i2c_bbpll_lref = 5 << I2C_BBPLL_OC_DCHGP_LSB;
    uint8_t i2c_bbpll_dcur = (1 << I2C_BBPLL_OC_DLREF_SEL_LSB) | (3 << I2C_BBPLL_OC_DHREF_SEL_LSB) | 3;
    esp_rom_regi2c_write(I2C_BBPLL, I2C_BBPLL_HOSTID, I2C_BBPLL_OC_REF_DIV, i2c_bbpll_lref);
    esp_rom_regi2c_write(I2C_BBPLL, I2C_BBPLL_HOSTID, I2C_BBPLL_OC_DIV_7_0, 8);
    esp_rom_regi2c_write_mask(
        I2C_BBPLL,
        I2C_BBPLL_HOSTID,
        I2C_BBPLL_OC_DR1,
        I2C_BBPLL_OC_DR1_MSB,
        I2C_BBPLL_OC_DR1_LSB,
        0
    );
    esp_rom_regi2c_write_mask(
        I2C_BBPLL,
        I2C_BBPLL_HOSTID,
        I2C_BBPLL_OC_DR3,
        I2C_BBPLL_OC_DR3_MSB,
        I2C_BBPLL_OC_DR3_LSB,
        0
    );
    esp_rom_regi2c_write(I2C_BBPLL, I2C_BBPLL_HOSTID, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);
    esp_rom_regi2c_write_mask(
        I2C_BBPLL,
        I2C_BBPLL_HOSTID,
        I2C_BBPLL_OC_VCO_DBIAS,
        I2C_BBPLL_OC_VCO_DBIAS_MSB,
        I2C_BBPLL_OC_VCO_DBIAS_LSB,
        2
    );

    // Wait for calibration to finish.
    while (!REG_GET_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_CAL_DONE));

    REG_CLR_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_LOW);
    REG_SET_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_HIGH);
    MODEM_LPCON.clk_conf.clk_i2c_mst_en = false;
}

// Initialize ESP32C6 clocks.
static void c6_init_clocks() {
    PMU.hp_sys[PMU_MODE_HP_ACTIVE].icg_modem.code       = 2;
    MODEM_SYSCON.clk_conf_power_st.clk_modem_apb_st_map = BIT(2);
    MODEM_LPCON.clk_conf_power_st.clk_i2c_mst_st_map    = BIT(2);
    MODEM_LPCON.clk_conf_power_st.clk_lp_apb_st_map     = BIT(2);
    PMU.imm.modem_icg.update_dig_icg_modem_en           = true;
    PMU.imm.sleep_sysclk.update_dig_icg_switch          = true;

    // Initialise system clocks and power management.
    PMU.hp_sys[PMU_MODE_HP_ACTIVE].regulator0.dbias = 25;
    PMU.lp_sys[PMU_MODE_HP_ACTIVE].regulator0.dbias = 26;
    PCR.mspi_clk_conf.mspi_fast_hs_div_num          = 5;

    // // Enable the PLL.
    // c6_enable_pll();
    // // Set CPU clock to PLL at 160MHz.
    // PCR.cpu_freq_conf.cpu_hs_div_num = 0;
    // PCR.sysclk_conf.soc_clk_sel      = 1;
    // esp_rom_set_cpu_ticks_per_us(160);

    // Select RTC clock sources.
    PMU.lp_sys[PMU_MODE_LP_ACTIVE].clk_power.xpd_fosc = true;
    esp_rom_delay_us(100);
    LP_CLKRST.lp_clk_conf.fast_clk_sel = 0;
    esp_rom_delay_us(100);
    LP_CLKRST.lp_clk_conf.slow_clk_sel = 0;
    esp_rom_delay_us(100);
}

// Initialize ESP32C6 watchdogs.
static void c6_init_watchdog() {
    LP_WDT.swd_wprotect.swd_wkey       = 0x50d83aa1;
    LP_WDT.swd_config.swd_auto_feed_en = 1;
    LP_WDT.wprotect.wdt_wkey           = 0x50d83aa1;
    LP_WDT.config0.wdt_en              = 0;
    TIMERG0.wdtwprotect.wdt_wkey       = 0x50d83aa1;
    TIMERG0.wdtconfig0.wdt_en          = 0;
    TIMERG1.wdtwprotect.wdt_wkey       = 0x50d83aa1;
    TIMERG1.wdtconfig0.wdt_en          = 0;
}

// Perform early initialization of the port-specific hardware.
void port_early_init() {
    c6_init_watchdog();
    c6_init_clocks();
}

// Perform full initialization of the port-specific hardware.
void port_init() {
    esp_cache_init();
}

// Pre-control handover checks and settings.
bool port_pre_handover() {
    // Send ESP-IDF information about clocks.
    LP_AON.store[4].val = ESP_RTC_FREQ_MHZ * 0x00010001;

    // Disable interrupts.
#ifdef __riscv
    asm("csrci mstatus, 8");
    asm("csrw mie, 0");
    asm("csrw mip, 0");
#endif

    return true;
}
