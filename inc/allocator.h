#ifndef ALLOCATOR_PROSTAGMOS_H

#define ALLOCATOR_PROSTAGMOS_H

#include <multiboot.h>


/* Structure containing information on its corresponding 4k page */
struct memory_list_entry
{
	union
	{
		struct
		{
			uint16_t length : 	10;
			uint16_t available: 	1;
			uint16_t allocated: 	1;
			uint16_t unused: 	4;
		};
		struct
		{
			uint16_t value;
		};
	};
};

/* Typedef alias for struct memory_list_entry */
typedef struct memory_list_entry memory_list_entry_t;

/* One entry for each 4k page, 4GB worth of pages is 1024 * 1024 entries */
#define MEMORY_MAP_LIST_SIZE (1024 * 1024)

/* Variable containing the adress of the end of the kernel */
extern int _end;
#define KERNEL_END ((uint32_t)(&_end))

void allocate_initialise(multiboot_info_t* multiboot_info);

void* allocate_pages(uint32_t);

void free_pages(void *);

#endif
