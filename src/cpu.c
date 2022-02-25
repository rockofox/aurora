#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "stdio.h"
#include "cpu.h"
#include "serial.h"
#include "mem.h"
#include "framebuffer.h"
#include "elf.h"
#include "virtmem.h"

static struct task_node *task_list = NULL;
static struct task_node *current_task = NULL;

struct task_node *init_task(void *entry)
{
    uint8_t *stack = alloc_block();
    uint8_t *user_stack = alloc_block();
    /*
     * CPU-Zustand fuer den neuen Task festlegen
     */
    struct cpu_state new_state = {
        .eax = 0,
        .ebx = 0,
        .ecx = 0,
        .edx = 0,
        .esi = 0,
        .edi = 0,
        .ebp = 0,
        .esp = (uint32_t)user_stack + 4096,
        .eip = (uint32_t)entry,

        .cs = 0x18 | 0x03,
        .ss = 0x20 | 0x03,

        .eflags = 0x202};

    struct cpu_state *state = (void *)(stack + 4096 - sizeof(new_state));
    *state = new_state;

    struct task_node *task_node = alloc_block();
    task_node->cpu_state = state;
    task_node->next = NULL;
    task_node->pid = 0;
    task_node->context = create_vmem_context();
    // vmem_map_page(task_node->context, 0, 0);
    vmem_copy_kernel_page_directory(task_node->context);
    // task_node->cr3 = (uint32_t)vmem_virt_to_phys(vmem_get_kernel_context(), task_node->context->pagedir);
    // task_node->context = vmem_create_page_dir();

    return task_node;
}
bool scheduler_locked = false;
void set_scheduler_locked(bool locked)
{
    scheduler_locked = locked;
}
void create_task(void *entry)
{
    struct task_node *task_node = init_task(entry);
    if (task_list == NULL)
    {
        task_node->pid = 0;
        task_list = task_node;
    }
    else
    {
        struct task_node *last_task = task_list;
        while (last_task->next != NULL)
        {
            last_task = last_task->next;
        }
        task_node->pid = last_task->pid + 1;
        last_task->next = task_node;
    }
}
void append_task(struct task_node *task)
{
    asm("cli");
    if (task_list == NULL)
    {
        task->pid = 0;
        task_list = task;
    }
    else
    {
        struct task_node *last_task = task_list;
        while (last_task->next != NULL)
        {
            last_task = last_task->next;
        }
        task->pid = last_task->pid + 1;
        last_task->next = task;
    }
    asm("sti");
}
uint32_t last_load_addr = 0x200000;
uint32_t get_free_load_addr()
{
    uint32_t addr = last_load_addr;
    last_load_addr += 0x1000;
    return addr;
}
void launch_elf(void *image)
{
    // return;
    uint32_t free_load_addr = get_free_load_addr();
    struct task_node *task_node = init_task(image);
    struct elf_header *header = (struct elf_header *)image;
    serial_printf("launch_elf: free load addr 0x%x\n", free_load_addr);
    // task_node->context = create_vmem_context();
    vmem_copy_kernel_page_directory(task_node->context);
    struct elf_program_header *program_header;
    if (header->magic != ELF_MAGIC)
    {
        serial_printf("Tried to load invalid ELF file (0x464C457F != 0x%x) \n", header->magic);
        return;
    }
    program_header = ((char *)image + header->ph_offset);
    for (int i = 0; i < header->ph_entry_count; i++)
    {
        void *dest = (void *)(program_header[i].virt_addr);
        void *src = (char *)image + program_header[i].offset;
        if (program_header->type != 1)
        {
            serial_printf("Skipping unsupported elf header with type %d\n", program_header->type);
            continue;
        }

        vmem_map_page(task_node->context, program_header->virt_addr, free_load_addr);
        memset(program_header->virt_addr, 0, program_header->file_size);
        memcpy(program_header->virt_addr, image + program_header->offset, program_header->file_size);
    }
    // task_node->cpu_state->eip = header->entry;
    task_node->cpu_state->eip = header->entry + 1;
    // last_load_addr += program_header->mem_size;

    append_task(task_node);
}
void idle()
{
    while (1)
    {
    }
}
void init_multitasking(void)
{
}
void kill_current_task()
{
    // return;
    if (current_task->pid == 0)
    {
        // serial_printf("Can't kill init task\n");
        return;
    }
    asm volatile("cli");
    scheduler_locked = true;
    if (current_task->pid > 100)
        serial_printf("Killing 0x%x\n", current_task->pid);
    struct task_node *task = task_list;
    while (task->next != current_task)
    {
        task = task->next;
    }
    task->next = current_task->next;
    if (task->next != NULL)
    {
        current_task = task->next;
    }
    else
    {
        current_task = task_list;
    }
    scheduler_locked = false;

    asm volatile("sti");
}
struct cpu_state *schedule(struct cpu_state *cpu)
{
    if (scheduler_locked)
    {
        return cpu;
    }
    if (task_list == NULL)
    {
        return cpu;
    }
    if (current_task == NULL)
    {
        current_task = task_list;
    }
    else
    {
        if (current_task->next != NULL)
        {
            // serial_printf("Switching to %d\n", current_task->next->pid);
            current_task = current_task->next;
        }
        else
        {
            // serial_printf("Switching to %d\n", task_list->pid);
            current_task = task_list;
        }
    }
    // serial_printf("0x%x\n", current_task);
    // serial_printf("Switching to %d\n", current_task->pid);
    return current_task->cpu_state;
}
struct task_node *get_current_task()
{
    return current_task;
}