#include <stdio.h>
#include <kernel/tty.h>

void kernel_main()
{
    terminal_init();
    terminal_writestring("test");
}