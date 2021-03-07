#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {

	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; ++i)
		if (putchar(bytes[i]) == EOF)
			return false;
	
	return true;	

}


