#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>
#define PROMPT "> "
// TODO: Split terminal and text mode code base

extern const size_t VGA_WIDTH;
extern const size_t VGA_HEIGHT;

extern size_t terminal_column;
extern size_t terminal_row;
extern uint8_t terminal_color;
extern uint16_t* terminal_buffer;


void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_clear();
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
#endif