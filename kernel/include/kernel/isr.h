#ifndef ISR_H
#define ISR_H

#define UNUSED(x) (void)(x)
#include <stdint.h>

struct registers_state
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed));

#endif
