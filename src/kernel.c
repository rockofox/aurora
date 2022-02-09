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

// /* Check if the compiler thinks you are targeting the wrong operating system. */
// #if defined(__linux__)
// #error "You are not using a cross-compiler, you will most certainly run into trouble"
// #endif

// /* This tutorial will only work for the 32-bit ix86 targets. */
// #if !defined(__i386__)
// #error "This tutorial needs to be compiled with a ix86-elf compiler"
// #endif

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
#define GDT_FLAG_DATASEG 0x02
#define GDT_FLAG_CODESEG 0x0a
#define GDT_FLAG_TSS 0x09

#define GDT_FLAG_SEGMENT 0x10
#define GDT_FLAG_RING0 0x00
#define GDT_FLAG_RING3 0x60
#define GDT_FLAG_PRESENT 0x80

#define GDT_FLAG_4K_GRAN 0x800
#define GDT_FLAG_32_BIT 0x400
#define GDT_SIZE 6
uint64_t gdt[GDT_SIZE];
static void set_entry(int i, unsigned int base, unsigned int limit, int flags)
{
	gdt[i] = limit & 0xffffLL;
	gdt[i] |= (base & 0xffffffLL) << 16;
	gdt[i] |= (flags & 0xffLL) << 40;
	gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
	gdt[i] |= ((flags >> 8) & 0xffLL) << 52;
	gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}

void init_gdt(void)
{
	set_entry(0, 0, 0, 0);
	set_entry(1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT);
	set_entry(2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT);
	set_entry(3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	set_entry(4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

	setGdt(gdt, sizeof(gdt) * sizeof(gdtEntry));
	reloadSegments();
}

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

	// outb(0x43,0x36);
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
