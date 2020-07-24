
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <paging.h>


#include <allocator.h> 

/* The main PD used by the kernel */
uint32_t page_directory[1024] __attribute__(( aligned (4096) )) ;

/* A PT used to map other PTs */
uint32_t page_tables[1024 * 1024] __attribute__(( aligned (4096) )) ;

/* Will map virtual addresses to physical, updating/creating tables if needed */
/*void map_with_allocate(uint32_t* logical, uint32_t* physical, uint16_t flags)
{
	uint32_t pd_index = (uint32_t)logical >> 22;
	
	uint32_t * table_address;
	
			
	if (page_directory_n[pd_index] & 1) // If the present bit is set
	{	

		table_address = page_directory_n[pd_index] & 0xFFFFFC00;

	}
	else
	{
		table_address = allocate_pages(1);
		
		//Zero newly created table
		for (int i = 0; i < 1024; ++i)
			table_address[i] = 0; //This is needed so sure that unused entries are not inadvertently marked as present
		
		printf("Table created! %X, %X, (%i), at %p\n", (uint32_t)logical, (uint32_t)physical, flags, table_address);
		
		//*(uint32_t*)(page_directory_n[(uint32_t)virtual_table_n >> 22] & 0xFFFFFC00) = (uint32_t)table_address | 3;
			
		
			
		page_directory_n[pd_index] = (uint32_t)table_address | flags; //Set corresponding page directory entry to present
	
		
		
	}
	
	uint32_t pt_index = ((uint32_t)logical >> 12) & 1023; // Get the corresponding page table index

	table_address[pt_index] = (uint32_t)physical | flags;
	
}*/

void map_logical_to_physical(uint32_t logical, uint32_t phyiscal, uint16_t flags)
{	
	//If the addresses are not page aligned, we have a problem
	if (logical & 0xfff || phyiscal & 0xfff)
	{
		return;
	}
	
	uint32_t directory_index = logical >> 22;
	uint32_t table_index = logical >> 12;

	
	page_directory[directory_index] = (uint32_t)&page_tables[table_index] | (flags & 0xfff);
	page_tables[table_index] = phyiscal | (flags & 0xfff);
}

void paging_initialise()
{
	//Zero out the PD
	for (int i = 0; i < 1024; ++i)
		page_directory[i] = 0; //This is needed so sure that unused entries are not inadvertently marked as present

	//Loop over kernel space and map to virtual memory
	for (int i = 0; i < get_kernel_end(); i += 4096)
		map_logical_to_physical(i, i, 3);	
	
	
	printf("End: %X\n", get_kernel_end());
	
	
	load_pd_table(page_directory);
	

	enable_paging();
	
	map_logical_to_physical(0xA00000, 0xB8000, 3);
	printf("map: %X\n", page_tables[0xA00000 >> 12]);

	//virtual_table_n[0] = 0x50A000 | 3;

	//*(uint32_t*)(0x507000) = 0x50C000 | 3;
	
	//*(uint32_t*)(0x50C000);
	
	
	*((char*)0xA00000) = 't';
}

































