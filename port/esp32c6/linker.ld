
/* SPDX-License-Identifier: MIT */

PHDRS
{
	hdrseg    PT_LOAD;
	codeseg   PT_LOAD;
	rodataseg PT_LOAD;
	dataseg   PT_LOAD;
}

SECTIONS
{
	/DISCARD/ : { *(.note.gnu.build-id) }

	/* Physical memory. */
	__start_xip    = 0x42000000;
	__stop_xip     = 0x43000000;
	__start_sram   = 0x40800000;
	__stop_sram    = 0x40880000;
	__start_lpsram = 0x50000000;
	__stop_lpsram  = 0x50004000;
	
	/* Application to bootloader data. */
	tobootloader = __start_lpsram;
	
	/* ROM symbols. */
	INCLUDE esp32c6.rom.newlib.ld
	INCLUDE esp32c6.rom.net80211.ld
	INCLUDE esp32c6.rom.heap.ld
	INCLUDE esp32c6.rom.ld
	INCLUDE esp32c6.rom.newlib-normal.ld
	INCLUDE esp32c6.rom.spiflash.ld
	INCLUDE esp32c6.rom.pp.ld
	INCLUDE esp32c6.rom.libgcc.ld
	INCLUDE esp32c6.rom.version.ld
	INCLUDE esp32c6.rom.phy.ld
	INCLUDE esp32c6.rom.rvfp.ld
	INCLUDE esp32c6.rom.coexist.ld
	INCLUDE esp32c6.rom.wdt.ld
	INCLUDE esp32c6.rom.api.ld
	INCLUDE esp32c6.peripherals.ld

	__section_alignment = 16;
	
	
	/* ESP image header. */
	.esphdr : AT(0) {
		BYTE(0xE9);			/* Magic byte. */
		BYTE(3);			/* Segment count. */
		BYTE(0x02);			/* SPI mode. */
		BYTE(0x10);			/* SPI speed/size. */
		LONG(_start);		/* Entrypoint. */
		BYTE(0xee);			/* WP pin state. */
		BYTE(0x00);			/* Drive settings. */
		BYTE(0x00);
		BYTE(0x00);
		SHORT(0x000D);		/* Chip (ESP32-C6). */
		BYTE(0x00);			/* (deprecated) */
		SHORT(0x0000);		/* Min chip rev. */
		SHORT(0x0000);		/* Max chip rev. */
		LONG(0x00000000);	/* (reserved) */
		BYTE(0x01);			/* SHA256 appended (appended). */
	} :hdrseg
	
	/* ESP image segment 0. */
	.espseg.0 : AT(LOADADDR(.esphdr) + SIZEOF(.esphdr)) {
		LONG(__start_data);
		LONG(__stop_data - __start_data);
	} :hdrseg
	
	/* RAM sections. */
	. = __stop_sram - 0x20000;
	__start_data = .;
	.data : AT(SIZEOF(.esphdr) + SIZEOF(.espseg.0)) {
		__global_pointer$ = .;
		*(.data) *(.data*)
		*(.sdata) *(.sdata*)
		. = ALIGN(__section_alignment);
	} :dataseg
	__stop_data = .;
	
	/* ESP image segment 1. */
	.espseg.1 : AT(LOADADDR(.data) + SIZEOF(.data)) {
		LONG(__start_text);
		LONG(__stop_text - __start_text);
	} :codeseg
	
	/* Code sections. */
	__start_text = .;
	.text : AT(LOADADDR(.espseg.1) + SIZEOF(.espseg.1)) {
		. = ALIGN(256);
		*(.interrupt_vector_table)
		*(.text) *(.text*)
		. = ALIGN(__section_alignment);
	} :codeseg
	__stop_text = .;
	
	/* ESP image segment 2. */
	.espseg.2 : AT(LOADADDR(.text) + SIZEOF(.text)) {
		LONG(__start_rodata);
		LONG(__stop_rodata - __start_rodata);
	} :rodataseg
	
	/* Read-only data sections. */
	__start_rodata = .;
	.init_array : AT(LOADADDR(.espseg.2) + SIZEOF(.espseg.2)) {
		/* Create symbols to keep track of `.init_array`. */
		__start_init_array = .;
		KEEP(*(.init_array))
		__stop_init_array = .;
	} :rodataseg
	.rodata : AT(LOADADDR(.init_array) + SIZEOF(.init_array)) {
		*(.rodata) *(.rodata*)
		*(.srodata) *(.srodata*)
		*(.eh_frame) *(.eh_frame_hdr*)
		. = ALIGN(__section_alignment);
	} :rodataseg
	__stop_rodata = .;
	
	__start_bss = .;
	.bss : {
		*(.bss) *(.bss*)
		*(.sbss) *(.sbss*)
		. = ALIGN(__section_alignment);
	} :NONE
	__stop_bss = .;
	
	/* Unallocated physical memory. */
	__start_free_xip    = (__stop_rodata + 0xffff) & 0xffff0000;
	__stop_free_xip     = __stop_xip;
	__start_free_sram   = (__stop_bss + 0xf) & 0xfffffff0;
	__stop_free_sram    = __stop_sram;
	__start_free_lpsram = __start_lpsram;
	__stop_free_lpsram  = __stop_lpsram;
}

ENTRY(_start)
