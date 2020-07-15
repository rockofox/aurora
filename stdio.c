#include "terminal.h"

void kprintln(const char* data) {
	terminal_writestring(data);
	if(terminal_row < VGA_HEIGHT) { terminal_row++; } else { terminal_clear(); terminal_row = 0;}
	terminal_column = 0;
}
void kprint(const char* data) {
	terminal_writestring(data);
}