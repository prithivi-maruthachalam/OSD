#include <stdio.h>

#include <kernel/tty.h>

void kmain(void) {

	terminal_init();
	
	printf("Hello Kernel!");

}

