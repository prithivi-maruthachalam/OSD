#include <lumos/tty.h>
#include <lumos/gdt.h>
#include <lumos/idt.h>
#include <lumos/timer.h>
#include <lumos/keyboard.h>
#include <lumos/multiboot.h>
#include <lumos/pmm.h>
#include <lumos/vmm.h>
#include <lumos/com.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void kernel_main(multiboot_info_t *mbt)
{
    terminal_init();
    gdt_init();
    idt_init();
    init_timer(50);
    init_keyboard();
    init_com1();

    printf("basic initializations complete. starting physical memory manager\n");
    init_pmm(mbt);

    printf("starting virtual memory manager");
    init_vmm();


    for (;;)
        ;
}