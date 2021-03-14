#include <stdio.h>
#include <stdlib.h>
#include <kernel/tty.h>

void kernel_main()
{
    terminal_init();
    for (int i = 0; i < 25; i++)
        printf("%d - [KERNEL_MAIN] : hi\n", i);
    printf("l1 -   sd f dfgh  \n");
    printf("l2 -   gd  dsfdfgh  \n dsjhfg ");
}