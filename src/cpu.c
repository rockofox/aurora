#include <stdint.h>
#include <stddef.h>
#include "stdio.h"
#include "cpu.h"
#include "serial.h"
#include "mem.h"
#include "framebuffer.h"

struct task_node
{
    struct cpu_state *cpu_state;
    struct task_node *next;
};

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

        /* Ring-0-Segmentregister */
        .cs = 0x18 | 0x03,
        .ss = 0x20 | 0x03,

        /* IRQs einschalten (IF = 1) */
        .eflags = 0x202};

    /*
     * Den angelegten CPU-Zustand auf den Stack des Tasks kopieren, damit es am
     * Ende so aussieht als waere der Task durch einen Interrupt unterbrochen
     * worden. So kann man dem Interrupthandler den neuen Task unterschieben
     * und er stellt einfach den neuen Prozessorzustand "wieder her".
     */
    struct cpu_state *state = (void *)(stack + 4096 - sizeof(new_state));
    *state = new_state;

    struct task_node *task_node = alloc_block();
    task_node->cpu_state = state;
    task_node->next = NULL;
    return task_node;
}

void create_task(void *entry)
{
    struct task_node *task_node = init_task(entry);
    if (task_list == NULL)
    {
        task_list = task_node;
    }
    else
    {
        struct task_node *last_task = task_list;
        while (last_task->next != NULL)
        {
            last_task = last_task->next;
        }
        last_task->next = task_node;
    }
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
struct cpu_state *schedule(struct cpu_state *cpu)
{
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
            current_task = current_task->next;
        }
        else
        {
            current_task = task_list;
        }
    }
    // serial_printf("0x%x\n", current_task);
    return current_task->cpu_state;
}