#include <kernel/isr.h>
#include <kernel/idt.h>
#include <kernel/ports.h>
#include <stdio.h>

static inline void sendEOI(uint32_t IRQnum)
{
    if (IRQnum >= 8)
        outb(PIC_SLAVE_COMMAND_PORT, PIC_EOI);
    outb(PIC_MASTER_COMMAND_PORT, PIC_EOI);
}

void isr_handler(struct registers_state regs, uint32_t ISR_num, uint32_t errorCode)
{
    printf("ISR called - %d - err: %d\n", ISR_num, errorCode);
    UNUSED(regs);
    asm volatile("hlt");
}

void irq_handler(struct registers_state regs, uint32_t IRQ_num, uint32_t IDT_num)
{
    printf("IRQ called - %d - %d\n", IRQ_num, IDT_num);
    UNUSED(regs);
    if (IRQ_num == 1)
    {
        printf("Keypress: ");
        unsigned char scan_code = inb(KEYBOARD_DATA_PORT);
        printf("%x\n", scan_code);
    }

    sendEOI(IRQ_num);
}