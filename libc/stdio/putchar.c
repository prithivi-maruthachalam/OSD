#include <stdio.h>

#ifdef __IS_LIBK
#include <kernel/tty.h>
#endif

int putchar(int ic) {

	#ifdef __IS_LIBK
	char c = (char) ic;
	termincal_write(&c, sizeof(c));
	#endif

	return ic;

}
