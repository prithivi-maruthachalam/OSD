#include <stdio.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#endif

int putchar(int ch)
{
    unsigned char character = (unsigned char)ch;
#if defined(__is_libk)
    terminal_putchar(character);
#else
    // TODO: Write to stdout
#endif

    // Conforming to the linux implementation
    return (int)character;
}