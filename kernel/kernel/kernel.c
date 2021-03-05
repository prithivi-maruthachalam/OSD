#include <stdio.h>

#include <kernel/tty.h>

void kmain(void) {

	terminal_init();
	
	kprintf("Hello Kernel!");

}

