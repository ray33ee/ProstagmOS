#ifndef ALLOCATOR_PROSTAGMOS_H

#define ALLOCATOR_PROSTAGMOS_H

#include <multiboot.h>

void allocate_initialise(multiboot_info_t* multiboot_info);

void* allocate_pages(uint32_t);

void free_pages(void *);

#endif
