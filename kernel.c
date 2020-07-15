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

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif





void sleep(unsigned int millis) {
	int ticksElapsed = 0;
	while(ticksElapsed/57 < millis) {
		if (inb(0x40) == 0)
		{
			ticksElapsed++;
		}
	}
}

extern void shutdown();
void flushBuffer() {
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
unsigned char* VGA = (unsigned char*) 0xA0000;
void setpixel(int x, int y, unsigned char color) {
  int offset;
  if(0 <= x && x < 320) {
    if(0 <= y && y < 200) {
      offset = 320*y + x;
      VGA[offset] = color;
    }
  }
}

uint64_t gdt[3];
static void set_entry(int i, unsigned int base, unsigned int limit, int flags)
{
    gdt[i] = limit & 0xffffLL;
    gdt[i] |= (base & 0xffffffLL) << 16;
    gdt[i] |= (flags & 0xffLL) << 40;
    gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
    gdt[i] |= ((flags >> 8 )& 0xffLL) << 52;
    gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}
#define GDT_FLAG_DATASEG 0x02
#define GDT_FLAG_CODESEG 0x0a
#define GDT_FLAG_TSS     0x09

#define GDT_FLAG_SEGMENT 0x10
#define GDT_FLAG_RING0   0x00
#define GDT_FLAG_RING3   0x60
#define GDT_FLAG_PRESENT 0x80

#define GDT_FLAG_4K_GRAN 0x800
#define GDT_FLAG_32_BIT  0x400

void init_gdt(void)
{
    set_entry(0, 0, 0, 0);
    set_entry(1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT |
        GDT_FLAG_CODESEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT);
    set_entry(2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT |
        GDT_FLAG_DATASEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT);
    set_entry(3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT |
        GDT_FLAG_CODESEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
    set_entry(4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT |
        GDT_FLAG_DATASEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

    setGdt(gdt, sizeof(gdt) * sizeof(gdtEntry));
	reloadSegments();
}
void kernel_main(void)
{
	
	
	init_gdt();

	/* Initialize terminal interface */
	terminal_initialize();
	init_serial();
	serial_println("Serial initialized");

	// terminal_writestring("test");
	// outb(0x43,0x36);
	// outb(0x40, 1193182 / 65536);
	
	
	/* Newline support is left as an exercise. */
	// NMI_disable();
	idt_init();
	
	kprintln("AuroraOS wip");
	kprint(PROMPT);
	while(1) {

	}
}
