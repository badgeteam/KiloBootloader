
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Set the RAMFS root
if(NOT DEFINED BADGER_RAMFS_ROOT)
    set(BADGER_RAMFS_ROOT ${CMAKE_CURRENT_LIST_DIR}/root)
endif()

# Set the C compiler
if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER NAMES riscv32-unknown-linux-gnu-gcc riscv32-linux-gnu-gcc riscv64-unknown-linux-gnu-gcc riscv64-linux-gnu-gcc REQUIRED)
    message("Detected RISC-V C compiler as '${CMAKE_C_COMPILER}'")
else()
    message("Using compiler '${CMAKE_C_COMPILER}' from environment")
endif()

# Determine the compiler prefix
get_filename_component(compiler_name "${CMAKE_C_COMPILER}" NAME)
string(REGEX MATCH "^([A-Za-z0-9_]+\-)*" BADGER_COMPILER_PREFIX "${compiler_name}") 
find_program(BADGER_OBJCOPY NAMES "${BADGER_COMPILER_PREFIX}objcopy" REQUIRED)  
find_program(BADGER_OBJDUMP NAMES "${BADGER_COMPILER_PREFIX}objdump" REQUIRED)

set(target_arch rv32imac_zicsr_zifencei)
if(DEFINED TARGET_ARCH)
	set(target_arch "${TARGET_ARCH}")
endif()

set(target_abi ilp32)
if(DEFINED TARGET_ABI)
	set(target_abi "${TARGET_ABI}")
endif()

# LTO is disabled due to GCC bugs inserting calls to memcpy everywhere
set(common_compiler_flags
    -ffreestanding                     # We do not compile against an OS.
    -march=${target_arch}              # Selects the target CPU.
    -mabi=${target_abi}                # Selects target ABI 
    -nodefaultlibs                     # Do not link any default libraries like libgcc or libc.
    -O2                                # Optimize the code.
    -ggdb                              # Generate debug information in default extended format.
    -fsanitize=undefined               # Adds sanitizer for undefined behaviour.
    -fsanitize-undefined-trap-on-error # Invoke a trap instruction instead of calling into the UBsan runtime.
    -Werror=return-type                # Error when a function doesn't return a value, but declares to do so.
    -Wall -Wextra                      # Ramp up warning level.
    -std=gnu11                         # We use the C11 standard
    -ffunction-sections                # Enable function sections for code size.
)

# we must pass the same options to GCC and LD when using LTO, as the linker will actually do the codegen
add_compile_options(${common_compiler_flags})
add_link_options(${common_compiler_flags} -nostartfiles -Wl,-gc-sections)

# For IDE users.
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(kilobootloader C ASM)

# Define executable file.
set(target kbbl.elf)
add_executable(${target}
    ${CMAKE_CURRENT_LIST_DIR}/src/badgelib/arrays.c
    ${CMAKE_CURRENT_LIST_DIR}/src/badgelib/badge_err.c
    ${CMAKE_CURRENT_LIST_DIR}/src/badgelib/badge_format_str.c
    ${CMAKE_CURRENT_LIST_DIR}/src/badgelib/badge_strings.c
    ${CMAKE_CURRENT_LIST_DIR}/src/badgelib/int_routines.c
    ${CMAKE_CURRENT_LIST_DIR}/src/badgelib/log.c
    ${CMAKE_CURRENT_LIST_DIR}/src/badgelib/num_to_str.c
    ${CMAKE_CURRENT_LIST_DIR}/src/partsys/appfs.c
    ${CMAKE_CURRENT_LIST_DIR}/src/media/xip.c
    ${CMAKE_CURRENT_LIST_DIR}/src/protocol/esp.c
    ${CMAKE_CURRENT_LIST_DIR}/src/bootmedia.c
    ${CMAKE_CURRENT_LIST_DIR}/src/bootprotocol.c
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/partsys.c
)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/include/badgelib)

# Include port-specific.
include(cpu/riscv/CMakeLists.txt)
include(port/esp32c6/CMakeLists.txt)



# Convert the raw binary file into a ESP32 image file
add_custom_command(
    OUTPUT kbbl.bin
    COMMAND cp kbbl.elf kbbl.elf.patch
    COMMAND "${BADGER_OBJCOPY}" -O binary kbbl.elf kbbl.nochecksum.bin
    COMMAND python3 "${CMAKE_CURRENT_LIST_DIR}/tools/pack-image.py" kbbl.nochecksum.bin kbbl.bin
    DEPENDS kbbl.elf
)
# Proxy target to allow installation of the bin file
add_custom_target(kbbl.bin.target ALL DEPENDS kbbl.bin)

# Also build a dump file that is put into the 
add_custom_target(
    kbbl.elf.disasm
    ALL
    COMMAND "${BADGER_OBJDUMP}" -Sd kbbl.elf > kbbl.elf.disasm
    DEPENDS kbbl.elf
)

# Declare which files are installed to the output directory:
install(TARGETS kbbl.elf DESTINATION .)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kbbl.bin" DESTINATION .)

