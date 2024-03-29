#include <lumos/idt.h>
#include <lumos/ports.h>
#include <stdio.h>

struct idtDescriptor IDTR;
struct idtEntry IDT[IDT_SIZE] = {0};
extern void load_idt(struct idtDescriptor *IDTR);

void idt_create_entry(struct idtEntry *entry, uint32_t base, uint16_t segmentSelector, uint16_t type)
{
    entry->baseLow = (uint16_t)base & 0x0000FFFF;
    entry->baseHigh = (uint16_t)(base >> 16) & 0x0000FFFF;
    entry->segmentSelector = segmentSelector;
    entry->type = type;
    entry->reserved = 0;
}

void idt_init()
{
#define CREATE_ISR(ISR_num) idt_create_entry(&IDT[ISR_num], (uint32_t)isr##ISR_num, KERNEL_CODE_SELECTOR, 0x8E)
#define CREATE_IRQ(IRQ_num, IDT_num) idt_create_entry(&IDT[IDT_num], (uint32_t)irq##IRQ_num, KERNEL_CODE_SELECTOR, 0x8E)

    extern void isr0();
    extern void isr1();
    extern void isr2();
    extern void isr3();
    extern void isr4();
    extern void isr5();
    extern void isr6();
    extern void isr7();
    extern void isr8();
    extern void isr9();
    extern void isr10();
    extern void isr11();
    extern void isr12();
    extern void isr13();
    extern void isr14();
    extern void isr15();
    extern void isr16();
    extern void isr17();
    extern void isr18();
    extern void isr19();
    extern void isr20();
    extern void isr21();
    extern void isr22();
    extern void isr23();
    extern void isr24();
    extern void isr25();
    extern void isr26();
    extern void isr27();
    extern void isr28();
    extern void isr29();
    extern void isr30();
    extern void isr31();

    extern void irq0();
    extern void irq1();
    extern void irq2();
    extern void irq3();
    extern void irq4();
    extern void irq5();
    extern void irq6();
    extern void irq7();
    extern void irq8();
    extern void irq9();
    extern void irq10();
    extern void irq11();
    extern void irq12();
    extern void irq13();
    extern void irq14();
    extern void irq15();

    outb(PIC_MASTER_COMMAND_PORT, 0x11);
    outb(PIC_SLAVE_COMMAND_PORT, 0x11);
    outb(PIC_MASTER_DATA_PORT, 0x20);
    outb(PIC_SLAVE_DATA_PORT, 0x28);
    outb(PIC_MASTER_DATA_PORT, 0x04);
    outb(PIC_SLAVE_DATA_PORT, 0x02);
    outb(PIC_MASTER_DATA_PORT, 0x01);
    outb(PIC_SLAVE_DATA_PORT, 0x01);
    outb(PIC_MASTER_DATA_PORT, 0xFF);
    outb(PIC_SLAVE_DATA_PORT, 0xFF);

    CREATE_ISR(0);
    CREATE_ISR(1);
    CREATE_ISR(2);
    CREATE_ISR(3);
    CREATE_ISR(4);
    CREATE_ISR(5);
    CREATE_ISR(6);
    CREATE_ISR(7);
    CREATE_ISR(8);
    CREATE_ISR(9);
    CREATE_ISR(10);
    CREATE_ISR(11);
    CREATE_ISR(12);
    CREATE_ISR(13);
    CREATE_ISR(14);
    CREATE_ISR(15);
    CREATE_ISR(16);
    CREATE_ISR(17);
    CREATE_ISR(18);
    CREATE_ISR(19);
    CREATE_ISR(20);
    CREATE_ISR(21);
    CREATE_ISR(22);
    CREATE_ISR(23);
    CREATE_ISR(24);
    CREATE_ISR(25);
    CREATE_ISR(26);
    CREATE_ISR(27);
    CREATE_ISR(28);
    CREATE_ISR(29);
    CREATE_ISR(30);
    CREATE_ISR(31);

    CREATE_IRQ(0, 32);
    CREATE_IRQ(1, 33);
    CREATE_IRQ(2, 34);
    CREATE_IRQ(3, 35);
    CREATE_IRQ(4, 36);
    CREATE_IRQ(5, 37);
    CREATE_IRQ(6, 38);
    CREATE_IRQ(7, 39);
    CREATE_IRQ(8, 40);
    CREATE_IRQ(9, 41);
    CREATE_IRQ(10, 42);
    CREATE_IRQ(11, 43);
    CREATE_IRQ(12, 44);
    CREATE_IRQ(13, 45);
    CREATE_IRQ(14, 46);
    CREATE_IRQ(15, 47);

    IDTR.limit = (uint16_t)sizeof(IDT) - 1;
    IDTR.base = (uint32_t)&IDT;
    load_idt(&IDTR);

#undef CREATE_ISR
#undef CREATE_IRQ
}