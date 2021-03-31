#include <stdio.h>
#include <stdlib.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>

void kernel_main()
{
    gdt_init();
    terminal_init();
    printf("[kernel_main]: GDT initialized\n");
    idt_init();
    printf("[kernel_main]: IDT initialized\n");
}