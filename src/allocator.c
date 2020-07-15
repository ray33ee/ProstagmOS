#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>

#include <allocator.h>
#include <vga.h>


/* List of entries describing all 4GB of memory */
memory_list_entry_t memory_map_list[MEMORY_MAP_LIST_SIZE];

/* Number of total suitable bytes of memory */
uint32_t total_available_memory = 0;

/* Mark a region from 'address' of length 'length' with 'flags' */
void mark_region(uint32_t address, uint32_t length, uint16_t flags)
{
	uint32_t address_index = address / 0x1000; //Convert address to page number, from 0 to 1024 * 1024 - 1
	uint32_t region_page_length = length / 0x1000; //Get number of pages in region
	
	for (uint32_t i = 0; i < region_page_length; ++i)
		memory_map_list[address_index + i].value = flags;
}

/* This function takes the multiboot info structure and updates each entry of memory_map_list accordingly. 
   After this the memory_map_list should be converted into a linked list for faster allocations and frees. 
   We use a pre-function to generate a complete list because the mmap list could be out of order, or incomplete.*/	
void pre_memory_list(multiboot_info_t* multiboot_info)
{
	uint32_t size = 0;
	
	//Iterate over all mmap entries
	while (size < multiboot_info->mmap_length)
	{
		uint8_t * ptr = (uint8_t *)(multiboot_info->mmap_addr + size); //We first cast this to uint8 so adding size moves the pointer bytewise, instead of 32-bit-wise
		multiboot_memory_map_t* mmap_info = (multiboot_memory_map_t *)ptr;
			
		//Because we are currently only able to deal with physical memory addressed with 32-bit, anything with an address above that must be ignored.
		//We test this by making sure that the high 32-bit value is zero AND
		//Also ignore everything before 1MB AND
		//Only change the list if it is available memory, since memory starts as unavailable by default
		if (!mmap_info->addrh && mmap_info->addrl >= 0x100000 && mmap_info->type == 1)
			mark_region(mmap_info->addrl, mmap_info->lenl, 0);
		
		if (!mmap_info->addrh && mmap_info->addrl >= 0x100000)
			printf("Size: %u, Type: %u, Len: %X %X, Addr: %X %X\n", mmap_info->size, mmap_info->type, mmap_info->lenh, mmap_info->lenl, mmap_info->addrh, mmap_info->addrl);
		
		size += mmap_info->size + sizeof(mmap_info->size);
	}

	//All entries from 0 to _end must be accounted for, and marked as unavailable, since this is where the kernel is located
	mark_region(0, KERNEL_END + 4096, 0x400); //We add an extra 4096 to round up to the next page, since _end is unlikely to be page aligned
}

/* Setup the memory_map_list and other data for page frame allocation */
void allocate_initialise(multiboot_info_t* multiboot_info)
{
	//By default, we start with all memory unavailable
	for (uint32_t i = 0; i < MEMORY_MAP_LIST_SIZE; ++i)
		memory_map_list[i].value = 0x400;
	
	//If the 6th bit is set, then the mmap_* data is valid, so we use this to create our map
	if (multiboot_info->flags | 64) 
	{
		pre_memory_list(multiboot_info);
		
		for (int i = 1; i < 1024 * 1024; ++i)
			if (memory_map_list[i].value != memory_map_list[i-1].value)
				printf("Memory map change at %X, from %i to %i.\n", i, memory_map_list[i-1].value, memory_map_list[i].value);
		
	}
	else
	{
		//If the program reaches here, GRUB was unable to create a valid memory map
		
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
