#include <stdio.h>
#include <stdlib.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>

void kernel_main()
{
    gdt_init();
    terminal_init();
    printf("[kernel_main]: GDT initialized\n");
}