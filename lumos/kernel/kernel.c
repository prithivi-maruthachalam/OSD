#include <lumos/tty.h>
#include <lumos/gdt.h>
#include <lumos/idt.h>
#include <lumos/timer.h>
#include <lumos/keyboard.h>
#include <lumos/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void kernel_main(uint32_t ebx, uint32_t eax)
{
    terminal_init();
    gdt_init();
    printf("[kernel_main]: GDT initialized\n");
    idt_init();
    printf("[kernel_main]: IDT initialized\n");
    init_timer(50);
    printf("[kernel_main]: timer initialized\n");
    init_keyboard();
    printf("[kernel_main]: PS/2 keyboard driver initialized\n");

    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", (unsigned long int)0xC0010000);
    printf("dfsdf: %d\n", -34);

    for (;;)
        ;
}