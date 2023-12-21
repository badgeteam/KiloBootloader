
// SPDX-License-Identifier: MIT

#pragma once

#include "port/hardware.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INTMTX_CORE0_PMU_INTR_MAP_REG               (INTMTX_BASE + 0x0034)
#define INTMTX_CORE0_EFUSE_INTR_MAP_REG             (INTMTX_BASE + 0x0038)
#define INTMTX_CORE0_LP_RTC_TIMER_INTR_MAP_REG      (INTMTX_BASE + 0x003C)
#define INTMTX_CORE0_LP_UART_INTR_MAP_REG           (INTMTX_BASE + 0x0040)
#define INTMTX_CORE0_LP_I2C_INTR_MAP_REG            (INTMTX_BASE + 0x0044)
#define INTMTX_CORE0_LP_WDT_INTR_MAP_REG            (INTMTX_BASE + 0x0048)
#define INTMTX_CORE0_LP_PERI_TIMEOUT_INTR_MAP_REG   (INTMTX_BASE + 0x004C)
#define INTMTX_CORE0_LP_APM_M0_INTR_MAP_REG         (INTMTX_BASE + 0x0050)
#define INTMTX_CORE0_LP_APM_M1_INTR_MAP_REG         (INTMTX_BASE + 0x0054)
#define INTMTX_CORE0_CPU_INTR_FROM_CPU_0_MAP_REG    (INTMTX_BASE + 0x0058)
#define INTMTX_CORE0_CPU_INTR_FROM_CPU_1_MAP_REG    (INTMTX_BASE + 0x005C)
#define INTMTX_CORE0_CPU_INTR_FROM_CPU_2_MAP_REG    (INTMTX_BASE + 0x0060)
#define INTMTX_CORE0_CPU_INTR_FROM_CPU_3_MAP_REG    (INTMTX_BASE + 0x0064)
#define INTMTX_CORE0_ASSIST_DEBUG_INTR_MAP_REG      (INTMTX_BASE + 0x0068)
#define INTMTX_CORE0_TRACE_INTR_MAP_REG             (INTMTX_BASE + 0x006C)
#define INTMTX_CORE0_CACHE_INTR_MAP_REG             (INTMTX_BASE + 0x0070)
#define INTMTX_CORE0_CPU_PERI_TIMEOUT_INTR_MAP_REG  (INTMTX_BASE + 0x0074)
#define INTMTX_CORE0_GPIO_INTERRUPT_PRO_MAP_REG     (INTMTX_BASE + 0x0078)
#define INTMTX_CORE0_GPIO_INTERRUPT_PRO_NMI_MAP_REG (INTMTX_BASE + 0x007C)
#define INTMTX_CORE0_PAU_INTR_MAP_REG               (INTMTX_BASE + 0x0080)
#define INTMTX_CORE0_HP_PERI_TIMEOUT_INTR_MAP_REG   (INTMTX_BASE + 0x0084)
#define INTMTX_CORE0_HP_APM_M0_INTR_MAP_REG         (INTMTX_BASE + 0x008C)
#define INTMTX_CORE0_HP_APM_M1_INTR_MAP_REG         (INTMTX_BASE + 0x0090)
#define INTMTX_CORE0_HP_APM_M2_INTR_MAP_REG         (INTMTX_BASE + 0x0094)
#define INTMTX_CORE0_HP_APM_M3_INTR_MAP_REG         (INTMTX_BASE + 0x0098)
#define INTMTX_CORE0_LP_APM0_INTR_MAP_REG           (INTMTX_BASE + 0x009C)
#define INTMTX_CORE0_MSPI_INTR_MAP_REG              (INTMTX_BASE + 0x00A0)
#define INTMTX_CORE0_I2S_INTR_MAP_REG               (INTMTX_BASE + 0x00A4)
#define INTMTX_CORE0_UHCI0_INTR_MAP_REG             (INTMTX_BASE + 0x00A8)
#define INTMTX_CORE0_UART0_INTR_MAP_REG             (INTMTX_BASE + 0x00AC)
#define INTMTX_CORE0_UART1_INTR_MAP_REG             (INTMTX_BASE + 0x00B0)
#define INTMTX_CORE0_LEDC_INTR_MAP_REG              (INTMTX_BASE + 0x00B4)
#define INTMTX_CORE0_TWAI0_INTR_MAP_REG             (INTMTX_BASE + 0x00B8)
#define INTMTX_CORE0_TWAI1_INTR_MAP_REG             (INTMTX_BASE + 0x00BC)
#define INTMTX_CORE0_USB_INTR_MAP_REG               (INTMTX_BASE + 0x00C0)
#define INTMTX_CORE0_RMT_INTR_MAP_REG               (INTMTX_BASE + 0x00C4)
#define INTMTX_CORE0_I2C_EXT0_INTR_MAP_REG          (INTMTX_BASE + 0x00C8)
#define INTMTX_CORE0_TG0_T0_INTR_MAP_REG            (INTMTX_BASE + 0x00CC)
#define INTMTX_CORE0_TG0_T1_INTR_MAP_REG            (INTMTX_BASE + 0x00D0)
#define INTMTX_CORE0_TG0_WDT_INTR_MAP_REG           (INTMTX_BASE + 0x00D4)
#define INTMTX_CORE0_TG1_T0_INTR_MAP_REG            (INTMTX_BASE + 0x00D8)
#define INTMTX_CORE0_TG1_T1_INTR_MAP_REG            (INTMTX_BASE + 0x00DC)
#define INTMTX_CORE0_TG1_WDT_INTR_MAP_REG           (INTMTX_BASE + 0x00E0)
#define INTMTX_CORE0_SYSTIMER_TARGET0_INTR_MAP_REG  (INTMTX_BASE + 0x00E4)
#define INTMTX_CORE0_SYSTIMER_TARGET1_INTR_MAP_REG  (INTMTX_BASE + 0x00E8)
#define INTMTX_CORE0_SYSTIMER_TARGET2_INTR_MAP_REG  (INTMTX_BASE + 0x00EC)
#define INTMTX_CORE0_APB_ADC_INTR_MAP_REG           (INTMTX_BASE + 0x00F0)
#define INTMTX_CORE0_PWM_INTR_MAP_REG               (INTMTX_BASE + 0x00F4)
#define INTMTX_CORE0_PCNT_INTR_MAP_REG              (INTMTX_BASE + 0x00F8)
#define INTMTX_CORE0_PARL_IO_INTR_MAP_REG           (INTMTX_BASE + 0x00FC)
#define INTMTX_CORE0_SLC0_INTR_MAP_REG              (INTMTX_BASE + 0x0100)
#define INTMTX_CORE0_SLC1_INTR_MAP_REG              (INTMTX_BASE + 0x0104)
#define INTMTX_CORE0_DMA_IN_CH0_INTR_MAP_REG        (INTMTX_BASE + 0x0108)
#define INTMTX_CORE0_DMA_IN_CH1_INTR_MAP_REG        (INTMTX_BASE + 0x010C)
#define INTMTX_CORE0_DMA_IN_CH2_INTR_MAP_REG        (INTMTX_BASE + 0x0110)
#define INTMTX_CORE0_DMA_OUT_CH0_INTR_MAP_REG       (INTMTX_BASE + 0x0114)
#define INTMTX_CORE0_DMA_OUT_CH1_INTR_MAP_REG       (INTMTX_BASE + 0x0118)
#define INTMTX_CORE0_DMA_OUT_CH2_INTR_MAP_REG       (INTMTX_BASE + 0x011C)
#define INTMTX_CORE0_GPSPI2_INTR_MAP_REG            (INTMTX_BASE + 0x0120)
#define INTMTX_CORE0_AES_INTR_MAP_REG               (INTMTX_BASE + 0x0124)
#define INTMTX_CORE0_SHA_INTR_MAP_REG               (INTMTX_BASE + 0x0128)
#define INTMTX_CORE0_RSA_INTR_MAP_REG               (INTMTX_BASE + 0x012C)
#define INTMTX_CORE0_ECC_INTR_MAP_REG               (INTMTX_BASE + 0x0130)
#define INTMTX_CORE0_INT_STATUS_0_REG               (INTMTX_BASE + 0x0134)
#define INTMTX_CORE0_INT_STATUS_1_REG               (INTMTX_BASE + 0x0138)
#define INTMTX_CORE0_INT_STATUS_2_REG               (INTMTX_BASE + 0x013C)

#define INTPRI_CORE0_CPU_INT_ENABLE_REG     (INTPRI_BASE + 0x0000)
#define INTPRI_CORE0_CPU_INT_TYPE_REG       (INTPRI_BASE + 0x0004)
#define INTPRI_CORE0_CPU_INT_EIP_STATUS_REG (INTPRI_BASE + 0x0008)
#define INTPRI_CORE0_CPU_INT_PRI_N_REG(N)   (INTPRI_BASE + 0x000C + 4 * (N))
#define INTPRI_CORE0_CPU_INT_THRESH_REG     (INTPRI_BASE + 0x008C)
#define INTPRI_CORE0_CPU_INT_CLEAR_REG      (INTPRI_BASE + 0x00A8)

#define PLIC_MX_INT_ENABLE_REG     (PLIC_MX_BASE + 0x0000)
#define PLIC_MX_INT_TYPE_REG       (PLIC_MX_BASE + 0x0004)
#define PLIC_MX_INT_CLEAR_REG      (PLIC_MX_BASE + 0x0008)
#define PLIC_MX_INT_EIP_STATUS_REG (PLIC_MX_BASE + 0x000C)
#define PLIC_MX_INT_PRI_N_REG(N)   (PLIC_MX_BASE + 0x0010 + 4 * (N))
#define PLIC_MX_INT_THRESH_REG     (PLIC_MX_BASE + 0x0090)

#define INTMTX_PRIO_MAX 15
#define INTMTX_PRIO_MIN 1

// Initialise the interrupt matrix with defaults.
void intmtx_init() __attribute__((noinline));
// Route interrupt source `source_map_reg` to interrupt channel `channel`.
void intmtx_route(int source_map_reg, int channel);
// Set interrupt channel `channel` priority to `prio`.
void intmtx_set_prio(int channel, int prio);
// Get interrupt channel `channel` priority.
int  intmtx_get_prio(int channel);
// Set interrupt threshold value.
void intmtx_set_thresh(int thresh);
// Query whether interrupt channel `channel` is enabled.
bool intmtx_is_enabled(int channel);
// Enable interrupt channel `channel`.
void intmtx_enable(int channel);
// Disable interrupt channel `channel` and return whether it was enabled.
bool intmtx_disable(int channel);
// Acknowledge an interrupt.
void intmtx_ack(int channel);
