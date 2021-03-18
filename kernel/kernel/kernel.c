#include <stdio.h>
#include <stdlib.h>
#include <kernel/tty.h>

void kernel_main()
{
    terminal_init();

    printf("Hello kernel\n");
}