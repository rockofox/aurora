#include "terminal.h"
#include "unistd.h"
void kprintln(const char* data) {
	scrollback_write(data);
	
	terminal_row++;
	terminal_column = 0;
	while (terminal_row > VGA_HEIGHT + scrollback_getposition())
	{
		terminal_scrolldown(VGA_HEIGHT);
	}
}
void kprint(const char* data) {
	scrollback_write(data);
}
void kputc(char c) {
	scrollback_putc(c);
}