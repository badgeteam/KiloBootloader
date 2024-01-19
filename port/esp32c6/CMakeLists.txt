
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10.0)

target_sources(${target} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/src/clkconfig.c
	${CMAKE_CURRENT_LIST_DIR}/src/interrupt.c
	${CMAKE_CURRENT_LIST_DIR}/src/intmtx.c
    ${CMAKE_CURRENT_LIST_DIR}/src/memprotect.c
	${CMAKE_CURRENT_LIST_DIR}/src/rawprint.c
	${CMAKE_CURRENT_LIST_DIR}/src/time.c
	
	${CMAKE_CURRENT_LIST_DIR}/src/esp_cache.c
	${CMAKE_CURRENT_LIST_DIR}/src/esp_xip.c
)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_options(${target} PUBLIC -T${CMAKE_CURRENT_LIST_DIR}/linker.ld)
