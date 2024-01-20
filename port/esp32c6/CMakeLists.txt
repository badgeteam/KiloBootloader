
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
	${CMAKE_CURRENT_LIST_DIR}/src/esp_xip.c
)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_options(${target} PUBLIC -T${CMAKE_CURRENT_LIST_DIR}/linker.ld -L${CMAKE_CURRENT_LIST_DIR})

# AppFS supports this chip.
target_compile_definitions(${target} PUBLIC -DHAS_PARTSYS_APPFS)
# ESP image format supports this chip.
target_compile_definitions(${target} PUBLIC -DHAS_BOOTPROTOCOL_ESP -DESP_CHIP_ID=0x000D)
# XIP boot media exists for this platform.
target_compile_definitions(${target} PUBLIC -DHAS_BOOTMEDIA_XIP)
