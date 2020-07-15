#ifndef VGA_PROSTAGMOS_H

#define VGA_PROSTAGMOS_H

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

struct BCD
{
	// 10 characters is enough to encode any 32 bit number into BCD.
	char data[10];
	char PADDING[2]; //Pad structure to make it 4-byte divisible
};

typedef struct BCD BCD_t;

BCD_t double_dabble(uint32_t number);

void toAsciiString(BCD_t *);
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) ;
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) ;
 
size_t strlen(const char* str) ;
 
void terminal_initialize(uint32_t address) ;
 
void terminal_setcolor(uint8_t color) ;
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) ;
 
void terminal_putchar(char c);
 
void terminal_write_bytes(const uint8_t* data, size_t size) ;
 
void terminal_writestring(const char* data) ;

char nibble_to_char(uint8_t hex);

void terminal_put_hex_byte(uint8_t byte);

void terminal_write_byte_list(const uint8_t* bytes, uint32_t len);

void terminal_write_uint32(uint32_t number);

void terminal_write_BCD(BCD_t number);

#endif
