#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
#include "serial.h"

#define BITMAP_SIZE 32768
static uint32_t bitmap[BITMAP_SIZE];
extern const void kernel_start;
extern const void kernel_end;

void *memcpy(void *destination, const void *source, size_t num)
{
    uint8_t *dst = (uint8_t *)destination;
    uint8_t *src = (uint8_t *)source;
    for (size_t i = 0; i < num; i++)
    {
        dst[i] = src[i];
    }
    return destination;
}
void *memset(void *destination, int value, size_t num)
{
    long *d = destination;
    for (size_t i = 0; i < num; i++)
    {
        d[i] = value;
    }
    return destination;
}
void mark_free(void *addr)
{
    uint32_t index = (uint32_t)addr / 4096;
    bitmap[index] = 0;
}
void mark_used(void *addr)
{
    uint32_t index = (uint32_t)addr / 4096;
    bitmap[index] = 1;
}
void init_memory_management(multiboot_info_t *mbi)
{
    void *mem_start = (void *)mbi->mmap_addr;
    void *mem_end = (void *)mbi->mmap_addr + mbi->mmap_length;
    while (mem_start < mem_end)
    {
        multiboot_memory_map_t *mmap = mem_start;
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            uint32_t start = mmap->addr;
            uint32_t end = mmap->addr + mmap->len;
            for (uint32_t i = start; i < end; i += 4096)
            {
                mark_free((void *)i);
            }
        }
        mem_start += mmap->size + sizeof(mmap->size);
    }
    uintptr_t kernel_addr = (uintptr_t)&kernel_start;
    while (kernel_addr < (uintptr_t)&kernel_end)
    {
        mark_used((void *)kernel_addr);
        kernel_addr += 4096;
    }
}
void *alloc_block()
{
    for (uint32_t i = 0; i < BITMAP_SIZE; i++)
    {
        if (bitmap[i] == 0)
        {
            const uint32_t new_address = i * 4096;
            bitmap[i] = 1;
            // serial_printf("Allocated block at 0x%x\n", new_address);
            return (void *)(new_address);
        }
    }
    return NULL;
}
void free_block(void *addr)
{
    mark_free(addr);
}