#pragma once
#include <stdbool.h>
struct cpu_state
{
    // Von Hand gesicherte Register
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;

    uint32_t intr;
    uint32_t error;

    // Von der CPU gesichert
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
};
struct task_node
{
    struct cpu_state *cpu_state;
    uint32_t pid;
    uint32_t cr3;
    struct vmem_context *context;
    struct task_node *next;
};
void init_multitasking(void);
struct cpu_state *schedule(struct cpu_state *cpu);
void create_task(void *entry);
void kill_current_task();
void set_scheduler_locked(bool locked);
void launch_elf(void *image);
struct task_node *get_current_task();