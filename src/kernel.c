#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <vga.h>

//extern int space;
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

 
void kernel_main(uint32_t magic, void* multiboot_struct) 
{
	/* Initialize terminal interface */
	terminal_initialize(0xB8000);
 
	/* Newline support is left as an exercise. */
	terminal_writestring("Hello world, this is ProstagmOS!\n");
	
	float f = 1.0f + 3.0f;
	
	terminal_writestring("Boot magic value: ");
	terminal_write_uint32(magic);
	terminal_writestring("\n");
	
	if (magic != 0x2BADb002)
	{
		terminal_writestring("ProstagmOS only supports multiboot bootloader, and another bootloader was used. aborting...\n");
		return;
	}

	

	
	printf("hello '%X'\n", 0x44);
	//printf("hello '%s'\n", "testoimg");
	printf("hello '%c' - %X, %p.\n", 'f', 0x11223344, &kernel_main);

}






































