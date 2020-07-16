#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>

#include <vga.h>

#include <allocator.h>

//extern int space;

extern int _end;
extern void load_pd_table(void*);
extern void enable_paging();
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#define KERNEL_END ((uint32_t)(&_end))
 
void kernel_main(uint32_t magic, multiboot_info_t* multiboot_info) 
{
	/* Initialize terminal interface */
	terminal_initialize(0xB8000);
 
	/* Newline support is left as an exercise. */
	printf("Hello world, this is ProstagmOS!\n");
	
	printf("Boot magic value: %X\n", magic);
	
	if (magic != 0x2BADb002)
	{
		terminal_writestring("ProstagmOS only supports multiboot bootloader, and another bootloader was used. aborting...\n");
		return;
	}

	allocate_initialise(multiboot_info);



	printf("Location: %X\n", KERNEL_END);
	
	//Allocate three pages, one for the PD and two for two PTs.
	uint32_t* ptr = (uint32_t*)allocate_pages(3);


	//Zero out the PD and PTs
	for (int i = 0; i < 1024 * 1024 * 3; ++i)
		*ptr = 0;

	printf("Pages: %X, %X, %X.\n", ptr, ptr + 1024, ptr + 2048);

	ptr[0] = (uint32_t)(ptr + 1024) | 3;
	ptr[1] = (uint32_t)(ptr + 2048) | 3;

	uint32_t* page_tables = ptr + 1024;

	for (int i = 0; i < 1024 * 1024 * 2; ++i)
		page_tables[i] = i * 4096 | 3;

	page_tables[0] = 0xB8003;
	
	
	load_pd_table(ptr);

	enable_paging();
	
	*((char*)2) = 't';
	*((char*)2) = 't';

}






































