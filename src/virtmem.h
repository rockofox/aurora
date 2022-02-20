#pragma once
#include <stdint.h>
#include "multiboot.h"
#define PTE_PRESENT (1 << 0)
#define PTE_WRITABLE (1 << 1)
#define PTE_USER (1 << 2)

struct vmem_context
{
    uint32_t *pagedir;
};
void init_virtmem(multiboot_info_t *mbi);
int vmem_alloc_kernel_page(uintptr_t page);
int vmem_map_page(struct vmem_context *context, uintptr_t page, uintptr_t phys);
void vmem_activate_context(struct vmem_context *context);
void vmem_copy_page_directory(struct vmem_context *dest, struct vmem_context *src);
void vmem_copy_kernel_page_directory(struct vmem_context *dest);
void *vmem_virt_to_phys(struct vmem_context *context, void *virt);
struct vmem_context *vmem_get_kernel_context();
struct vmem_context *create_vmem_context();