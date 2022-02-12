#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
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
extern uint64_t gdt[GDT_SIZE];
typedef uint64_t gdtEntry;
extern void setGdt(gdtEntry *gdt, size_t gdtSize);
extern void reloadSegments();
void set_entry(int i, unsigned int base, unsigned int limit, int flags);