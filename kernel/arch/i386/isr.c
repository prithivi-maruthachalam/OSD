#include <kernel/isr.h>
#include <stdio.h>

void isr_handler(struct registers_state regs, uint32_t ISR_num, uint32_t errorCode)
{
    printf("ISR called - %d\n", ISR_num);
}

void irq_handler()
{
    printf("IRQ called\n");
}