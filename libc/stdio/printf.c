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

int printf(const char* restrict format, ...) {

	va_list params;
	va_start(params, format);

	int written = 0;

	while (*format != '\0') {

		size_t rem = INT_MAX - written;

		// Checking if we are printing an actual %
		if (format[0] != '%' || format[1] == '%') {
			
			if (format[0] == '%') format++;
			
			size_t amount = 1;
			while (format[amount] && format[amount] != '%') amount++; // Stepping through the format string until we encounter a %
		
			if (amount > rem) return -1; // Checking if we exceeded the limit

			if (!print(format, amount)) return -1;

			format += amount;
			written += amount;

			continue;
		
		}	

		format++;
		
		if (*format == 'c') {
		
			format++;

			char c = (char) va_arg(params, int);
		
			if (!rem) return -1;

			if (!print(&c, sizeof(c))) return -1;

			written++;

		} else if (*format == 's') {

			format++;

			const char* str = va_arg(params, const char *);
			size_t len = strlen(str);

			if (len > rem) return -1;
		
			if (!print(str, len)) return -1;
			
			written += len;
		
		} else continue;

	}

	va_end(params);

	return written;

}
