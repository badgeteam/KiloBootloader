
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10.0)

target_sources(${target} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/src/interrupt.c
	${CMAKE_CURRENT_LIST_DIR}/src/intmtx.c
    ${CMAKE_CURRENT_LIST_DIR}/src/memprotect.c
	${CMAKE_CURRENT_LIST_DIR}/src/rawprint.c
	${CMAKE_CURRENT_LIST_DIR}/src/time.c
	
	${CMAKE_CURRENT_LIST_DIR}/src/port.c
	${CMAKE_CURRENT_LIST_DIR}/src/esp_cache.c
	${CMAKE_CURRENT_LIST_DIR}/src/esp_rom_hp_regi2c_esp32c6.c
	${CMAKE_CURRENT_LIST_DIR}/src/esp_xip.c
)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../esp_common/include)
target_link_options(${target} PUBLIC -T${CMAKE_CURRENT_LIST_DIR}/linker.ld -L${CMAKE_CURRENT_LIST_DIR}/ld)

# ESP32 specific options.
target_compile_definitions(${target} PUBLIC
	-DESP_CLOCK_FREQ_MHZ=160
	-DESP_RTC_FREQ_MHZ=40
)

# Enable XIP boot media.
target_compile_definitions(${target} PUBLIC -DHAS_BOOTMEDIA_XIP)

# Enable ESP partition table.
target_compile_definitions(${target} PUBLIC -DHAS_PARTSYS_ESP)

# Enable AppFS file system.
target_compile_definitions(${target} PUBLIC -DHAS_FILESYS_APPFS)
# Allow unformatted partitions.
target_compile_definitions(${target} PUBLIC -DALLOW_UNFORMATTED_PARTITION)

# Enable ESP image format.
target_compile_definitions(${target} PUBLIC -DHAS_BOOTPROTOCOL_ESP -DESP_CHIP_ID=0x000D)
