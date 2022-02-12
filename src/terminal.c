#include <stdint.h>
#include <stddef.h>
#include "string.h"
#include "terminal.h"
#include "serial.h"
#include "font8x8/font8x8_basic.h"

/* Hardware text mode color constants. */
enum vga_color
{
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

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;
framebuffer_t *framebuffer;

#define SCROLLBACK_SCREENS 4

uint16_t scrollback[80][VGA_HEIGHT * SCROLLBACK_SCREENS] = {0};
unsigned int scrollPosition = 0;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

void terminal_initialize(framebuffer_t *fb)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t *)0xB8000;
    framebuffer = fb;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}
void terminal_rawentryat(uint16_t ent, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    if (ent != 0)
    {
        draw_xbm(framebuffer, x * 8, y * 8, 8, 8, font8x8_basic[(char)ent]);
    }
    else
    {

        framebuffer_draw_rect(framebuffer, x * 8, y * 8, 8, 8, 0);
    }

    // terminal_buffer[index] = ent;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;

    for (size_t i = 0; i < 32; i++)
    {
        for (size_t j = 0; j < 32; j++)
        {
            framebuffer_set_pixel(framebuffer, x + i, y + j, 255);
        }
    }

    // terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH)
    {
        terminal_column = 0;

        if (++terminal_row == VGA_HEIGHT)
        {
            terminal_row = 0;
        }
    }
}

void terminal_clear()
{
    for (int i = 0; i < VGA_WIDTH; i++)
    {
        for (int j = 0; j < VGA_HEIGHT; j++)
        {
            scrollback[i][j] = 0;
        }
    }
    terminal_scrollback();
    terminal_column = 0;
    terminal_row = 0;
}

void terminal_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char *data)
{
    terminal_write(data, strlen(data));
}

void scrollback_write(const char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        scrollback[terminal_column++][terminal_row] = vga_entry(str[i], VGA_COLOR_WHITE);
    }
    terminal_scrollback();
}

void terminal_scrolldown(int l)
{
    scrollPosition += l;
    terminal_scrollback();
}

void terminal_scrollup(int l)
{
    if (scrollPosition > 0)
    {
        scrollPosition -= l;
        terminal_scrollback();
    }
}

int scrollback_getposition()
{
    return scrollPosition;
}

void scrollback_putc(char c)
{
    scrollback[terminal_column++][terminal_row] = c;
    terminal_scrollback();
}

void terminal_scrollback()
{
    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        for (size_t y = 0; y < VGA_HEIGHT; y++)
        {
            terminal_rawentryat(scrollback[x][y + scrollPosition], x, y);
        }
    }
}