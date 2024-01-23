
// SPDX-License-Identifier: MIT

#include "filesys.h"



// First filesystem type.
filesys_type_t *filesys_first = NULL;
// Last filesystem type.
filesys_type_t *filesys_last  = NULL;

// Register a new filesystem type.
// This should only be called from constructor functions.
void filesys_register(filesys_type_t *filesys_type) {
    // Protocol is added to the end of the list.
    filesys_type->next = NULL;

    if (filesys_first) {
        // Second or later filesystem type to be registered.
        filesys_type->prev = filesys_last;
        filesys_last->next = filesys_type;
        filesys_last       = filesys_type;
    } else {
        // First filesystem type to be registered.
        filesys_first      = filesys_type;
        filesys_last       = filesys_type;
        filesys_type->prev = NULL;
    }
}



// File reading function.
diskoff_t file_raw_read(file_t *file, diskoff_t offset, diskoff_t length, void *mem) {
    partition_t *part  = file->filesys->part;
    bootmedia_t *media = part->media;
    if (offset < 0 || offset >= part->length) {
        return -1;
    }
    if (offset + length > part->length) {
        length = part->length - offset;
    }
    return media->read(media, part->offset + offset, length, mem);
}

// File memory mapping function.
bool file_raw_mmap(file_t *file, diskoff_t offset, diskoff_t length, size_t vaddr) {
    partition_t *part  = file->filesys->part;
    bootmedia_t *media = part->media;
    if (offset < 0 || offset >= part->length) {
        return -1;
    }
    if (offset + length > part->length) {
        length = part->length - offset;
    }
    return media->mmap(media, part->offset + offset, length, vaddr);
}

// Make a raw file for a partition.
void filesys_make_raw(partition_t *part, filesys_t *filesys, file_t *file) {
    filesys->part   = part;
    file->filesys   = filesys;
    file->size      = part->length;
    file->read      = file_raw_read;
    file->mmap      = file_raw_mmap;
    file->first_sec = part->offset;
}
