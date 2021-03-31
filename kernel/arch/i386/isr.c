#include <kernel/isr.h>
#include <stdio.h>

void isr_handler()
{
    printf("ISR called\n");
}

void irq_handler()
{
    printf("IRQ called\n");
}