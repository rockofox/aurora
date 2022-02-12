#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "io.h"
#include "idts.h"
#include "gdt.h"
#include "terminal.h"
#include "string.h"
#include "stdio.h"
#include "keyboard.h"
#include "serial.h"
#include "unistd.h"
#include "multiboot.h"
#include "cpu.h"
#include "mem.h"

extern void shutdown();
void flushBuffer()
{
	for (size_t i = 0; i < 20; i++)
	{
		buffer[i] = 0;
	}
}
void append(char *s, char c)
{
	int len = strlen(s);
	s[len] = c;
	s[len + 1] = '\0';
}

void NMI_enable()
{
	outb(0x70, inb(0x70) & 0x7F);
}

void NMI_disable()
{
	outb(0x70, inb(0x70) | 0x80);
}
unsigned char *VGA = (unsigned char *)0xA0000;
unsigned char *screen = (unsigned char *)0xA0000;
void setpixel(int x, int y, unsigned char color)
{
	int offset;
	if (0 <= x && x < 320)
	{
		if (0 <= y && y < 200)
		{
			offset = 320 * y + x;
			screen[offset] = color;
		}
	}
}
#define lambda(return_type, function_body) \
	({                                     \
		return_type __fn__ function_body   \
			__fn__;                        \
	})

framebuffer_t fb;
void kernel_main(uint32_t magic, multiboot_info_t *mbi)
{
	init_serial();
	serial_println("Serial initialized");
	if (magic != 0x2BADB002)
	{
		serial_println("Invalid magic number");
		char s[20];
		itoa(magic, s, 16);
		serial_println(s);
		return;
	}
	if ((void *)(uint32_t)mbi->framebuffer_addr != NULL && mbi->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT)
	{
		fb.addr = (void *)(uint32_t)mbi->framebuffer_addr;
		fb.width = mbi->framebuffer_width;
		fb.height = mbi->framebuffer_height;
		fb.pitch = mbi->framebuffer_pitch;
		fb.bpp = mbi->framebuffer_bpp;
		fb.red_mask_size = mbi->framebuffer_red_mask_size;
		fb.red_field_position = mbi->framebuffer_red_field_position;
		fb.green_mask_size = mbi->framebuffer_green_mask_size;
		fb.green_field_position = mbi->framebuffer_green_field_position;
		fb.blue_mask_size = mbi->framebuffer_blue_mask_size;
		fb.blue_field_position = mbi->framebuffer_blue_field_position;
	}
	screen = mbi->framebuffer_addr;

	init_gdt();
	/* Initialize terminal interface */
	terminal_initialize(&fb);
	init_memory_management(mbi);

	// outb(0x43, 0x36);
	// outb(0x40, 1193182 / 65536);

	/* Newline support is left as an exercise. */
	// NMI_disable();
	idt_init();

	kprintln("AuroraOS wip");
	kprint(PROMPT);
	// scrollback_write("Hello scrollback");
	// terminal_scrollback();

	while (1)
	{
		// kprintln("AuroraOS wip");
	}
}
