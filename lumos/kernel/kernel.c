#include <lumos/tty.h>
#include <lumos/gdt.h>
#include <lumos/idt.h>
#include <lumos/timer.h>
#include <stdio.h>
#include <stdlib.h>

void kernel_main()
{
    gdt_init();
    terminal_init();
    printf("[kernel_main]: GDT initialized\n");
    idt_init();
    printf("[kernel_main]: IDT initialized\n");
    init_timer(50);

    for (;;)
        ;
}