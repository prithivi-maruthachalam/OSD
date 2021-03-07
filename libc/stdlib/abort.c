#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__))
void abort() {
	
	#ifdef(__IS_LIBK)
	printf("Kernel Panic. Aborting.\n");
	#else
	printf("Aborting.")
	#endif

	while (1) __buitlin_unreachable();

}
