#include "virtmem.h"
#include "mem.h"
#include "serial.h"
#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>

struct vmem_context *kernel_context;
struct vmem_context *vmem_get_kernel_context()
{
    return kernel_context;
}
struct vmem_context *create_vmem_context()
{
    struct vmem_context *context = alloc_block();
    context->pagedir = alloc_block();
    for (uint32_t i = 0; i < 1024; i++)
    {
        context->pagedir[i] = 0;
    }
    return context;
}
int vmem_map_page(struct vmem_context *context, uintptr_t page, uintptr_t phys)
{
    uint32_t page_index = page / 0x1000;
    uint32_t pd_index = page_index / 1024;
    uint32_t pt_index = page_index % 1024;
    uint32_t *pd;
    // Check for 4K alignment
    if ((page & 0xfff) || (phys & 0xfff))
    {
        serial_printf("vmem_map_page: page and phys must be 4K aligned\n");
        return -1;
    }
    if (context->pagedir[pd_index] & PTE_PRESENT)
    {
        // Page table exists
        // serial_printf("vmem_map_page: page table exists\n");
        pd = (uint32_t *)(context->pagedir[pd_index] & ~0xfff);
    }
    else
    {
        // Page table doesn't exist, create it
        // serial_printf("vmem_map_page: page table doesn't exist, create it\n");
        pd = alloc_block();
        for (int i = 0; i < 1024; i++)
        {
            pd[i] = 0;
        }
        context->pagedir[pd_index] = (uint32_t)pd | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
    }

    pd[pt_index] = phys | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
    asm volatile("invlpg %0"
                 :
                 : "m"(*(char *)page));
    return 0;
}
void vmem_copy_page_directory(struct vmem_context *dest, struct vmem_context *src)
{
    for (int i = 0; i < 1024; i++)
    {
        dest->pagedir[i] = src->pagedir[i];
    }
}
void vmem_copy_kernel_page_directory(struct vmem_context *dest)
{
    vmem_copy_page_directory(dest, kernel_context);
}
void *vmem_virt_to_phys(struct vmem_context *context, void *virt)
{
    uintptr_t virt_addr = (uintptr_t)virt;
    uintptr_t pd_index = virt_addr / 0x40000000;
    uintptr_t pt_index = (virt_addr / 0x1000) % 1024;
    uint32_t *pd = (uint32_t *)(context->pagedir[pd_index] & ~0xfff);
    uint32_t *pt = (uint32_t *)(pd[pt_index] & ~0xfff);
    uintptr_t offset = virt_addr % 0x1000;
    return (void *)(pt[offset / 0x1000] & ~0xfff);
}
void vmem_activate_context(struct vmem_context *context)
{
    serial_printf("vmem_activate_context: 0x%x\n", context);
    asm volatile("mov %0, %%cr3"
                 :
                 : "r"(context->pagedir));
}
int vmem_alloc_kernel_page(uintptr_t page)
{
    return vmem_map_page(kernel_context, page, page);
}
void init_virtmem(multiboot_info_t *mbi)
{
    uint32_t cr0;
    kernel_context = create_vmem_context();
    int i;
    // Map the kernel space
    for (i = 0; i < 4096 * 1024; i += 0x1000)
    {
        // serial_printf("Allocating page 0x%x\n", i);
        vmem_map_page(kernel_context, i, i);
    }

    // Map the framebuffer
    for (i = 0; i < 4096 * 1024; i += 0x1000)
    {
        // serial_printf("Allocating page 0x%x\n", i);
        vmem_map_page(kernel_context, 0x400000 + i, mbi->framebuffer_addr + i);
    }
    // Map whatever
    // for (i = 0; i < 4096 * 1024; i += 0x1000)
    // {
    //     // serial_printf("Allocating page 0x%x\n", i);
    //     vmem_map_page(kernel_context, 0x1000000 + i, 0x1000000 + i);
    // }
    // for (i = 0; i < 4096 * 1024; i += 0x1000)
    // {
    //     // serial_printf("Allocating page 0x%x\n", i);
    //     vmem_map_page(kernel_context, 0xf0000000 + i, 0xf0000000 + i);
    // }
    // serial_printf("Free regions begin at 0x%x\n", 2 * (4096 * 1024));
    vmem_activate_context(kernel_context);
    asm volatile("mov %%cr0, %0"
                 : "=r"(cr0));
    cr0 |= (1 << 31);
    asm volatile("mov %0, %%cr0"
                 :
                 : "r"(cr0));
    // set_scheduler_locked(false);
}