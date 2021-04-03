#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/timer.h>
#include <stdio.h>
#include <stdlib.h>

#define CLOCKS_PER_SECOND 1000000

void kernel_main()
{
    gdt_init();
    terminal_init();
    printf("[kernel_main]: GDT initialized\n");
    idt_init();
    printf("[kernel_main]: IDT initialized\n");
    init_timer(CLOCKS_PER_SECOND);

    for (;;)
        ;
}