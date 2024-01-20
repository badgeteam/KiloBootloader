
# SPDX-License-Identifier: MIT

MAKEFLAGS         += --silent
IDF_PATH          ?= $(shell pwd)/../esp-idf
SHELL             := /usr/bin/env bash
PORT              ?= $(shell find /dev/ -name ttyUSB* -or -name ttyACM* | head -1)
OUTPUT            ?= "$(shell pwd)/firmware"
BUILDDIR          ?= "build"
.PHONY: all clean-tools clean build flash monitor test clang-format-check clang-tidy-check openocd gdb

all: build flash monitor

build:
	cmake -B "$(BUILDDIR)"
	cmake --build "$(BUILDDIR)"
	cmake --install "$(BUILDDIR)" --prefix "$(OUTPUT)"

clang-format-check: build
	echo "clang-format check the following files:"
	jq -r '.[].file' build/compile_commands.json | grep '\.[ch]$$'
	echo "analysis results:"
	clang-format --dry-run $(shell jq -r '.[].file' build/compile_commands.json | grep '\.[ch]$$')

clang-tidy-check: build
	echo "clang-tidy check the following files:"
	jq -r '.[].file' build/compile_commands.json | grep '\.[ch]$$'
	echo "analysis results:"
	clang-tidy -p build $(shell jq -r '.[].file' build/compile_commands.json | grep '\.[ch]$$') --warnings-as-errors="*"

openocd:
	openocd -c 'set ESP_RTOS "none"' -f board/esp32c6-builtin.cfg

gdb:
	riscv32-unknown-elf-gdb -x port/esp32c6/gdbinit build/kbbl.elf

clean-all: clean
clean:
	rm -rf "$(BUILDDIR)"

flash: build
	esptool.py -b 921600 --port "$(PORT)" \
		write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB \
		0x10000 port/esp32c6/bin/badger-os.bin \
		0x0 "$(OUTPUT)/kbbl.bin" \
		0x8000 port/esp32c6/partition-table.bin

monitor:
	echo -e "\033[1mType ^A^X to exit.\033[0m"
	picocom -q -b 115200 $(PORT) | ./tools/address-filter.py "$(OUTPUT)/kbbl.elf"; echo -e '\033[0m'
