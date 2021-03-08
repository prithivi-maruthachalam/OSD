#include <stdio.h>
#include <kernel/tty.h>

void kernel_main()
{
    terminal_init();
    printf("Hello from the kernel!\n");
}