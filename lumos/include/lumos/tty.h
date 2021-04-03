#ifndef TTY_H
#define TTY_H 1

#include <stddef.h>

void terminal_init();
void terminal_putchar(char c);
void terminal_write(const char *str, size_t size);
void terminal_writestring(const char *str);

#endif