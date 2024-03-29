#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__)) void abort(void)
{
#if defined(__is_libk)
    // TODO: panic!
    printf("kernel: panic: abort()\n");
    asm __volatile__("cli");
    asm __volatile__("hlt");
#else
    // TODO: Terminate process -- SIGABRT
    printf("abort()\n");
#endif

    // shouldn't really get here
    while (1)
    {
        __builtin_unreachable();
    }
}