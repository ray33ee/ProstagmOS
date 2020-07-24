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
			uint32_t pointer : 	20;
			/*union
			{
				struct
				{*/
					uint16_t available: 	1; //1 if the regions is allocatable, and not reserved for some other function
					uint16_t allocated: 	1; //1 if the region has been allocated by the kernel
				/*};
				struct
				{
					uint16_t flags: 	2;
				};
			};*/
			
			uint32_t unused: 	10;
		};
		struct
		{
			uint32_t value;
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

uint32_t get_kernel_end();

void allocate_initialise(multiboot_info_t* multiboot_info);

void* allocate_pages(uint32_t);

void free_pages(void *);

void display_pages();

#endif
