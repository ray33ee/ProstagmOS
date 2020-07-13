
 
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <vga.h>
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void terminal_initialize(uint32_t address) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) address;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
	if (c != '\n')
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	if (++terminal_column == VGA_WIDTH || c == '\n') {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}
 
void terminal_write_bytes(const uint8_t* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	while (*data)
		terminal_putchar(*data++);
}

char nibble_to_char(uint8_t hex)
{
	if (hex < 10)
		return hex + 48;
	else
		return hex + 55;
}

void terminal_put_hex_byte(uint8_t byte)
{
	terminal_putchar(nibble_to_char(byte >> 4 & 0x0f));
	terminal_putchar(nibble_to_char(byte & 0x0f));
}

void terminal_write_byte_list(const uint8_t* bytes, uint32_t len)
{
	for (uint32_t i = len; i > 0; --i)
		terminal_put_hex_byte(bytes[i - 1]);
}

void terminal_write_uint8(uint8_t number)
{
	terminal_put_hex_byte(number);
}

void terminal_write_uint16(uint16_t number)
{
	terminal_write_byte_list((const uint8_t*)&number, 2);
}

void terminal_write_uint32(uint32_t number)
{
	terminal_write_byte_list((const uint8_t*)&number, 4);
}

int printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	
	while (*format != 0)
	{
		if (*format == '%')
		{
			char specifier = *(format+1);
			switch (specifier)
			{
				case '%':
					terminal_putchar('%');
					break;
				case 'X':
					terminal_write_uint32(va_arg(args, uint32_t));
					break;
				case 'c':
					{char c = va_arg(args, uint32_t); //va_arg seems unable to handle small objects like char, so we treat it like an int then shrink down to a char
					terminal_putchar(c);
					break;}
				case 's':
					terminal_writestring(va_arg(args, const char*));
					break;
				case 'p':
					{void *ptr = va_arg(args, void *);
					terminal_write_uint32((uint32_t)ptr);
					break;}
				default:
					terminal_writestring("INVALID/UNSUPPORTED PRINTF FORMAT - We don't support exceptions (yet) so we print this message instead.\n");
					while (1);
			}
			++format;
		}
		else
		{
			terminal_putchar(*format);
		}
		++format;
	}
	
	va_end(args);
}

































