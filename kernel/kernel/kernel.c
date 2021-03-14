#include <stdio.h>
#include <stdlib.h>
#include <kernel/tty.h>

void kernel_main()
{
    terminal_init();
    printf("[KERNEL_MAIN] : hi    ");
    char temp_buf[12];
    printf(itoa(19345, temp_buf, 8));
    printf("    ");
    printf(itoa(-193, temp_buf, 10));
    printf("    ");
    printf(itoa(0, temp_buf, 10));
    printf("    ");
    printf(itoa(-1205, temp_buf, 16));
}