
 
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

inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
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

void terminal_write_BCD(BCD_t number)
{
	int leading = 1;
	
	for (int i = 0; i < 11; ++i)
	{		
		if (number.data[10-i])
			leading = 0;
		
		if (leading == 0)
		{
			terminal_putchar(number.data[10-i] + 48);
		}
	}

	if (leading == 1)
		terminal_putchar('0');
}

BCD_t double_dabble(uint32_t number)
{
	BCD_t digits;
	
	// Clear entire structure
	for (int i = 0; i < 12; ++i)
		digits.data[i] = 0;
	
	for (int i = 0; i < 32; ++i)
	{		
		int carry = 0;
		
		for (int j = 0; j < 10; ++j)
		{
			// add previous carry
			digits.data[j] += carry;
			
			//If digit is greater than 4, add 3
			if (digits.data[j] > 4)
				digits.data[j] += 3;
				
			
			//if the 5th bit is set, then the nibble has carried
			carry = digits.data[j] & 0x10 ? 1 : 0;
			
			//Clear the 5th bit
			digits.data[j] &= 0xef;
			
		}
		
		carry = (number & 0x80000000) ? 1 : 0;
		
		number <<= 1;
		
		for (int j = 0; j < 10; ++j)
		{
			digits.data[j] <<= 1;
			
			digits.data[j] += carry;
			
			carry = digits.data[j] & 0x10 ? 1 : 0;
			
			digits.data[j] &= 0xef;
		}
	}
	
	return digits;
	
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
				case 'u':
					{uint32_t number = va_arg(args, uint32_t);
					BCD_t digits = double_dabble(number);
					terminal_write_BCD(digits);
					}
					break;
				case 'i':
					{int32_t number = va_arg(args, int32_t);
					if (number & 0x80000000)
					{
						terminal_putchar('-');
						number = ~number + 1;
					}
					BCD_t digits = double_dabble(number);
					terminal_write_BCD(digits);
					}
					break;
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

































