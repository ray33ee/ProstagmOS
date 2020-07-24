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

void display_pages()
{
	uint32_t pointer = 0;
		
	while (pointer != MEMORY_MAP_LIST_SIZE - 1)
	{
		printf("Loop Index: %X (av: %i, al: %i) , Pointer: %X\n", pointer, memory_map_list[pointer].available, memory_map_list[pointer].allocated, memory_map_list[pointer].pointer);
		pointer = memory_map_list[pointer].pointer;
	}	
	
}

uint32_t get_kernel_end()
{
	return memory_map_list[0].pointer << 12;
}

/* Mark a region from 'address' of length 'length' with 'flags' */
void mark_region(uint32_t address, uint32_t length, uint32_t flags)
{
	uint32_t address_index = address >> 12; //Convert address (4GB) to page number, from 0 to 1024 * 1024 - 1, by dividing by 4096
	uint32_t region_page_length = length >> 12; //Get number of pages in region, by dividing by 4096
	
	for (uint32_t i = 0; i < region_page_length; ++i)
		memory_map_list[address_index + i].available = flags ? 1 : 0;
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
			mark_region(mmap_info->addrl, mmap_info->lenl, 1);
		
		//if (!mmap_info->addrh && mmap_info->addrl >= 0x100000)
		//	printf("Size: %u, Type: %u, Len: %X %X, Addr: %X %X\n", mmap_info->size, mmap_info->type, mmap_info->lenh, mmap_info->lenl, mmap_info->addrh, mmap_info->addrl);
		
		size += mmap_info->size + sizeof(mmap_info->size);
	}

	//All entries from 0 to _end must be accounted for, and marked as unavailable, since this is where the kernel is located
	mark_region(0, KERNEL_END + 4096, 0); //We add an extra 4096 to round up to the next page, since _end is unlikely to be page aligned
}

/* Setup the memory_map_list and other data for page frame allocation */
void allocate_initialise(multiboot_info_t* multiboot_info)
{
	//By default, we start with all memory unavailable
	for (uint32_t i = 0; i < MEMORY_MAP_LIST_SIZE; ++i)
		memory_map_list[i].value = 0;
	
	//If the 6th bit is set, then the mmap_* data is valid, so we use this to create our map
	if (multiboot_info->flags & 64) 
	{
		pre_memory_list(multiboot_info);
		
		uint32_t index = 0;
		
		for (int i = 1; i < MEMORY_MAP_LIST_SIZE; ++i)
			if (memory_map_list[i].available != memory_map_list[i-1].available)
			{
				//printf("Memory map change at %X, from %i to %i.\n", i, memory_map_list[i-1].value, memory_map_list[i].value);
				memory_map_list[index].pointer = i;
				//printf("Index: %X, Pointer: %X\n", index, memory_map_list[index].length);
				index = i;
			}
		
		//Map the last block to the end 
		memory_map_list[index].pointer = MEMORY_MAP_LIST_SIZE - 1;
		
	}
	else
	{
		//If the program reaches here, GRUB was unable to create a valid memory map
		
	}

}

// Allocates 'length' number of contiguous pages of memory
void* allocate_pages(uint32_t length)
{
	//	Walk over memory_map_list
	
	//		When we find an entry with a) an availibility of 1, and b) with length greater then or equal to 'length', we
	
	//			Save the current record pointer
	//			Update the record pointer to (current_location + length)
	//			Update the record status to allocated (and available)
	//			Update the record at pointer (current_location + length) to saved pointer
	//			return a pointer to the page
	//	If program flow reaches here, alloc_pages was unable to find a continuous block of memory of the desired size, so we reutrn null
	
	uint32_t index = 0;
		
	while (index != MEMORY_MAP_LIST_SIZE - 1)
	{
		uint32_t block_length = memory_map_list[index].pointer - index - 1;
		
		if (memory_map_list[index].available && !memory_map_list[index].allocated && block_length >= length)
		{
			//printf("Length: %X\n", memory_map_list[index].pointer - index - 1);
			
			uint32_t next_pointer = memory_map_list[index].pointer; //Save the pointer to the current next item
			
			memory_map_list[index].pointer = index + length; //Set pointer to newly created node
			
			memory_map_list[index].allocated = 1; //Mark the identified region as allocated
			
			if (block_length != length) //If the block is exactly the required length, then we do not need to create an intermediate node
				memory_map_list[index + length].pointer = next_pointer; //Set the pointer of the newly created node to fix the list
				
			
			return (void *)(index << 12); //Return pointer to the allocated page
		}
		index = memory_map_list[index].pointer;
	}	
	
	
	return NULL; //If the allocation failed, return NULL
}

// Frees memory pagges at 'address'
void free_pages(void* address)
{

	uint32_t page_index = (uint32_t)address >> 12;
	
	uint32_t list_iterator = 0;
	
	while (memory_map_list[list_iterator].pointer != page_index)
		list_iterator = memory_map_list[list_iterator].pointer;
	
	//Get the memory list entry for the block corresponding to address,, and fetch the entries for the blocks before and after too
	memory_list_entry_t * previous = &memory_map_list[list_iterator];
	memory_list_entry_t * this = &memory_map_list[previous->pointer];
	memory_list_entry_t * next = &memory_map_list[this->pointer];
	
	//Mark the block to free as unallocated
	this->allocated = 0;

	//A block is mergable (with the deallocated block) if it is available, and not already allocated
	int previous_mergable = previous->available && !previous->allocated; 
	int next_mergable = next->available && !next->allocated; 
	
	//If we cannot merge the unallocated block with any other block, there is nothing left to do
	if (!previous_mergable && !next_mergable)
		return;
		
	//If the previous and next blocks are mergable, all blocks can be merged into one space
	if (previous_mergable && next_mergable)
	{
		previous->pointer = next->pointer;
		return;
	}
	
	//If only one of the blocks is mergable, merge that one
	if (previous_mergable)
		previous->pointer = this->pointer;
	else
	{
		this->pointer = next->pointer;
	}		
	
}
























