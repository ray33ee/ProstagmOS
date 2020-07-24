#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>

#include <vga.h>

#include <allocator.h>

//extern int space;

extern int _end;
 
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

	display_pages();

	paging_initialise();

	void * first = allocate_pages(1);
	void * second = allocate_pages(1);
	void * third = allocate_pages(1);

	free_pages(first);
	free_pages(third);
	free_pages(second);

display_pages();

	printf("Location: %X\n", KERNEL_END);
	
	

}






































