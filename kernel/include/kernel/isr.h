#ifndef ISR_H
#define ISR_H

#include <stdint.h>

struct registers_state
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed));

#endif
