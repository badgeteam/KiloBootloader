
// SPDX-License-Identifier: MIT

#include "badge_err.h"
#include "bootprotocol.h"
#include "checksum.h"
#include "log.h"
#include "memprotect.h"
#include "port.h"
#include "port/interrupt.h"
#include "time.h"



static void bootstrap();

// ISR context.
static isr_ctx_t isr_ctx;

// After control handover, the booting CPU core starts here and other cores wait.
// This sets up the basics of everything needed by the bootloader.
// When finished, the booting CPU will continue by bootstrapping the kernel / firmware.
void basic_runtime_init() {
    // ISR initialization.
    interrupt_init(&isr_ctx);
    // Early platform initialization.
    port_early_init();

    // Timekeeping initialization.
    time_init();

    // Memory protection initialization.
    memprotect_init();

    // Full hardware initialization.
    port_init();

    // Continue to bootstrapping.
    bootstrap();
}

#ifndef PARTTAB_SIZE
#define PARTTAB_SIZE 16
#endif
// Number of discovered partitions.
size_t      partnum;
// Partition boot order.
size_t      ordertab[PARTTAB_SIZE];
// Global partition list.
partition_t parttab[PARTTAB_SIZE];

// Detect partition systems and register partitions.
static void register_partitions() {
    // Iterate boot media looking for partitions.
    for (bootmedia_t *media = bootmedia_first; media; media = media->next) {
        media->partsys = NULL;
        for (partsys_t *partsys = partsys_first; partsys; partsys = partsys->next) {
            diskoff_t count = partsys->ident(media);
            if (count > 0) {
                media->part_num = count;
                media->part_sel = -1;
                media->partsys  = partsys;
                break;
            }
        }
#ifdef ALLOW_UNPARTITIONED_MEDIA
        if (!media->partsys) {
            media->partsys  = &partsys_raw;
            media->part_num = 1;
            media->part_sel = 0;
        }
#endif
    }

    // Build the partition table.
    partnum = 0;
    for (bootmedia_t *media = bootmedia_first; media; media = media->next) {
        if (!media->part_num)
            continue;
        for (diskoff_t i = 0; i < media->part_num; i++) {
            if (partnum >= PARTTAB_SIZE) {
                logkf(LOG_WARN, "Too many partitions, using first %{size;d}");
                return;
            }
            parttab[partnum] = media->partsys->read(media, i);
            if (parttab[partnum].flags.bootable)
                partnum++;
        }
    }
}

// Look for a known filesystem.
static filesys_type_t *find_filesys(partition_t *part) {
    for (filesys_type_t *type = filesys_type_first; type; type = type->next) {
        if (type->ident(part)) {
            return type;
        }
    }
#ifdef ALLOW_UNFORMATTED_PARTITION
    return &filesys_type_raw;
#else
    return NULL;
#endif
}

// Try to boot from a file.
static void try_file(file_t *file) {
    // Try known boot protocols.
    for (bootprotocol_t *protocol = bootprotocol_first; protocol; protocol = protocol->next) {
        if (protocol->ident(file)) {
            protocol->boot(file);
            return;
        }
    }
}

// After basic runtime initialization, the booting CPU core continues here.
// This looks for bootable media and any bootable images on said media.
// When finished, a kernel is bootstrapped or the bootloader gives up and halts.
static void bootstrap() {
    logk(LOG_INFO, "KiloBootloader v0.1");
    logkf(
        LOG_INFO,
        "I know %{size;d} partitioning system%{c}, "
        "%{size;d} filesystem%{c} and "
        "%{size;d} boot protocol%{c}",
        partsys_num,
        partsys_num != 1 ? 's' : 0,
        filesys_type_num,
        filesys_type_num != 1 ? 's' : 0,
        bootprotocol_num,
        bootprotocol_num != 1 ? 's' : 0
    );

    // Boot media discovery.
#ifdef HAS_BOOTMEDIA_XIP
    extern void register_xip_media();
    register_xip_media();
#endif
    logkf(LOG_INFO, "Found %{size;d} bootable media", bootmedia_num);

    // Partition discovery.
    register_partitions();
    logkf(LOG_INFO, "Found %{size;d} bootable partition%{c}", partnum, partnum != 1 ? 's' : 0);

    // Determine boot order.
    for (size_t x = 0; x < partnum; x++) {
        size_t highest_pri   = PART_PRIO_MIN;
        size_t highest_index = 0;
        for (size_t y = 0; y < partnum; y++) {
            if (parttab[y].flags.bootable && parttab[y].prio <= highest_pri) {
                highest_pri   = parttab[y].prio;
                highest_index = y;
            }
        }
        parttab[highest_index].flags.bootable = false;
        ordertab[x]                           = highest_index;
    }

    // Try to boot the partitions in order.
    for (size_t i = 0; i < partnum; i++) {
        filesys_type_t *type = find_filesys(&parttab[ordertab[i]]);
        if (!type)
            continue;
        filesys_t filesys;
        file_t    file;
        if (!type->read(&parttab[ordertab[i]], &filesys, &file))
            continue;
        try_file(&file);
    }

    logk(LOG_FATAL, "Failed to boot!");
    while (1) continue;
}
