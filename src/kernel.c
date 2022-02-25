#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "io.h"
#include "idts.h"
#include "gdt.h"
#include "terminal.h"
#include <string.h>
#include "stdio.h"
#include "keyboard.h"
#include "serial.h"
#include "unistd.h"
#include "multiboot.h"
#include "cpu.h"
#include "mem.h"
#include "virtmem.h"
#include "font8x8/font8x8_basic.h"
#include "fs/ff14b/source/ff.h"

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
int test_ff();
framebuffer_t fb;
void kernel_main(uint32_t magic, multiboot_info_t *mbi)
{
	mark_used(mbi);
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
		// fb.addr = (void *)(uint32_t)mbi->framebuffer_addr;
		fb.addr = 0x400000;
		// serial_printf("Framebuffer at 0x%x\n", (uint32_t)mbi->framebuffer_addr);
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
	// set_scheduler_locked(true);

	init_memory_management(mbi);
	init_virtmem(mbi);

	init_gdt();
	idt_init();

	// test_ff();

	/* Initialize terminal interface */
	terminal_initialize(&fb);

	kprintln("AuroraOS wip");
	kprint(PROMPT);

	// outb(0x43, 0x36);
	// outb(0x40, 1193182 / 65536);

	/* Newline support is left as an exercise. */
	// NMI_disable();
	// framebuffer_draw_rect(&fb, 0, 0, 123, 123, 0xFF);
	// draw_xbm_scaled(&fb, 120, 120, 8, 8, font8x8_basic['A'], 10);
	// scrollback_write("Hello scrollback");
	// terminal_scrollback();
	// // fat_extBS_32_t *bs = alloc_block();
	// uint8_t bs[512 * 12] = {0};
	// for (uint32_t i = 0;; i++)
	// {
	// 	ata_read_sectors(bs, i, 1);
	// 	// for (uint32_t j = 0; j < 512; j++)
	// 	// {
	// 	// 	// write_serial(bs[j]);
	// 	// 	serial_printf("%x ", bs[j]);
	// 	// }
	// 	// serial_println("");
	// 	// serial_println("___________________");
	// 	// serial_printf("%x ", bs[0]);
	// 	if (bs[0] == 0xeb)
	// 	{
	// 		serial_printf("\e[1;34mFound boot sector at block 0x%x\e[m\n", i);
	// 		// for (int i = 0; i < 512; i++)
	// 		// {
	// 		// 	serial_printf("%x ", bs[i]);
	// 		// }
	// 		fat_extBS_32_t *abs = (fat_extBS_32_t *)bs;
	// 		serial_printf("Name: %s\n", abs->stuff);
	// 		break;
	// 	}
	// 	// Find LBA
	// 	if (bs[0] == 0x0B)
	// 	// memset(bs, 0, 512);
	// }
	// unsigned char buf[512] = {0};
	// ata_read_sectors(buf, 0, 1);
	// uint32_t papos = ((mbr_t *)buf)->partitions[0].first_sector_lba;
	// ata_read_sectors(buf, papos, 1);
	// ata_read_sectors(buf, papos + fat32_get_first_data_sector(buf), 1);
	// fat_dir_entry_t *de = (fat_dir_entry_t *)(buf + 0x20 * 2);
	// uint32_t i = fat32_find_cluster(*de);

	create_task(lambda(
		void,
		()
		{
			while (1)
			{
			}
		}));

	serial_printf("%d modules loaded\n", mbi->mods_count);
	create_task(lambda(
		void,
		()
		{
			kprintln("Hello, world!\n");
			// serial_println("Hello, World!");
			asm volatile("int $0x80"
						 :
						 : "a"(0x1));
			while (1)
			{
			}
		}));
	set_scheduler_locked(true);
	serial_println("Loading multiboot modules");
	multiboot_module_t *modules = (struct multiboot_module_t *)mbi->mods_addr;
	mark_used(modules);
	for (uint16_t i = 0; i < mbi->mods_count; i++)
	{
		multiboot_module_t *addr = modules[i].mod_start;
		launch_elf(addr);
		while (addr < modules[i].mod_end)
		{
			mark_used(addr);
			addr++;
		}
	}
	serial_println("All modules loaded");
	set_scheduler_locked(false);

	serial_println("");
	FRESULT res;
	FATFS fs;
	res = f_mount(&fs, "", 0);
	if (res != FR_OK)
	{
		serial_println("Failed to mount filesystem");
	}
	else
	{
		serial_println("Filesystem mounted");
	}
	// test_ff();
	while (1)
	{
		// kprintln("AuroraOS wip");
	}
}
