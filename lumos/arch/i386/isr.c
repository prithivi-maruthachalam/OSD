#include <lumos/isr.h>
#include <lumos/idt.h>
#include <lumos/ports.h>
#include <stdio.h>

static inline void sendEOI(uint32_t IRQnum)
{
    if (IRQnum >= 8)
        outb(PIC_SLAVE_COMMAND_PORT, PIC_EOI);
    outb(PIC_MASTER_COMMAND_PORT, PIC_EOI);
}

void (*handlers[IDT_SIZE])(struct registers_state);
char *exception_messages[] = {
    "Division by zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND range exceeded",
    "Invalid Opcode",
    "Device not available (no math coprocesor)",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-Segment fault",
    "General Protection",
    "Page fault",
    "Reserved",
    "Floating point error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point exception",
    "Virtualisation Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"};

void isr_handler(struct registers_state regs, uint32_t ISR_num, uint32_t errorCode, uint32_t EIP)
{
    printf("\nReceived exception :: GATE %d\n", ISR_num);
    /* TODO:
        if it has an exception handler, use it
        else print message and halt
    */

    printf("%s(%d)\n", exception_messages[ISR_num], errorCode);
    printf("Exception occurred @ %x\n", EIP);
    asm volatile("hlt");
    for (;;)
        ;
    UNUSED(regs);
}

void irq_handler(struct registers_state regs, uint32_t IRQ_num, uint32_t IDT_num)
{
    if (!handlers[IDT_num])
    {
        printf("IRQ called - %d - %d\n", IRQ_num, IDT_num);
    }
    else
    {
        (handlers[IDT_num])(regs);
    }

    sendEOI(IRQ_num);
    UNUSED(regs);
}

void register_interrupt_handler(uint32_t IDT_num, uint32_t handler)
{
    handlers[IDT_num] = (void (*)(struct registers_state))handler;
}