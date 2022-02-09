#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>
#include "framebuffer.h"
#define PROMPT "> "
#define VGA_WIDTH 100
#define VGA_HEIGHT 80
// TODO: Split terminal and text mode code base

extern size_t terminal_column;
extern size_t terminal_row;
extern uint8_t terminal_color;
extern uint16_t *terminal_buffer;
extern framebuffer_t *framebuffer;
// extern uint16_t scrollback[80][25 * 4];

void terminal_initialize(framebuffer_t *fb);
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_clear();
void terminal_write(const char *data, size_t size);
void terminal_writestring(const char *data);
void scrollback_write(const char *str);
void terminal_scrollback();
void scrollback_putc(char c);
void terminal_scrolldown(int l);
void terminal_scrollup(int l);
int scrollback_getposition();
#endif