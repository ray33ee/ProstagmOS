#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>

#include <allocator.h>
#include <vga.h>

/* Structure containing information on its corresponding 4k page */
struct memory_list_entry
{
	union
	{
		struct
		{
			uint16_t length : 	10;
			uint16_t unavailable: 	1;
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

/* List of entries describing all 4GB of memory */
memory_list_entry_t memory_map_list[MEMORY_MAP_LIST_SIZE];

/* Number of total suitable bytes of memory */
uint32_t total_available_memory = 0;
//////////////////////////////////////////////////////////////////////////////////////////// MARK EVERYTHING BEFORE END OF KERNEL AS RESERVED
/*  */
void allocate_initialise(multiboot_info_t* multiboot_info)
{
	//Clear the memory map
	for (uint32_t i = 0; i < MEMORY_MAP_LIST_SIZE; ++i)
		memory_map_list[i].value = 0;
	
	//Walk the multiboot info memory map and fill memory_map_list accordingly
	printf("Multiboot info flags: %X\n", multiboot_info->flags);
	
	//If the 6th bit is set, then the mmap_* data is valid
	if (multiboot_info->flags | 64) 
	{
		printf("Multiboot info mmap length: %X\n", multiboot_info->mmap_length);
		printf("Multiboot info mmap addr  : %X\n", multiboot_info->mmap_addr);

		uint32_t size = 0;
		
		uint32_t available_memory = 0;
		
		while (size < multiboot_info->mmap_length)
		{
			uint8_t * ptr = (uint8_t *)(multiboot_info->mmap_addr + size); //We first cast this to uint8 so adding size moves the pointer bytewise, instead of 32-bit-wise
			multiboot_memory_map_t* mmap_info = (multiboot_memory_map_t *)ptr;
			
			//Because we are currently only able to deal with physical memory addressed with 32-bit, anything with an address above that must be ignored.
			//We test this by making sure that the high 32-bit value is zero
			if (!mmap_info->addrh)
			{
				if (mmap_info->type == 1)
					available_memory += mmap_info->lenl;
					
				//If the address is not page aligned
			
				printf("Size: %u, Type: %u, Len: %u %u, Addr: %X %X\n", mmap_info->size, mmap_info->type, mmap_info->lenh, mmap_info->lenl, mmap_info->addrh, mmap_info->addrl);
			}
			
			size += mmap_info->size + sizeof(mmap_info->size);
			
			
		}
		
		printf("Total available memory: %u\n", available_memory);
	}

}

// Allocates 'length' number of contiguous pages of memory
void* allocate_pages(uint32_t length)
{
	
}

// Frees memory pagges at 'address'
void free_pages(void* address)
{

}
