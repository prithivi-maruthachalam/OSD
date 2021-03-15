#include <stdio.h>
#include <stdlib.h>
#include <kernel/tty.h>

void kernel_main()
{
    terminal_init();
    for (int i = 0; i < 25; i++)
        printf("%d - [KERNEL_MAIN] : hi\n", i);

    for (int i = 0; i <= 85; i++)
    {
        printf("%d", i % 10);
    }
}