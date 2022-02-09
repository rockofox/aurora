#include <stdint.h>
#include <stddef.h>
#include "stdio.h"

// Tasks to test the scheduler
void task_a(void)
{
    while (1)
    {
        // kprintln("A");
    }
}

void task_b(void)
{
    while (1)
    {
        // kprintln("B");
    }
}

static uint8_t stack_a[4096];
static uint8_t stack_b[4096];
struct cpu_state
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eip;

    uint32_t cs;
    uint32_t eflags;
};
/*
 * Jeder Task braucht seinen eigenen Stack, auf dem er beliebig arbeiten kann,
 * ohne dass ihm andere Tasks Dinge ueberschreiben. Ausserdem braucht ein Task
 * einen Einsprungspunkt.
 */
struct cpu_state *init_task(uint8_t *stack, void *entry)
{
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
        //.esp = unbenutzt (kein Ring-Wechsel)
        .eip = (uint32_t)entry,

        /* Ring-0-Segmentregister */
        .cs = 0x08,
        //.ss  = unbenutzt (kein Ring-Wechsel)

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

    return state;
}