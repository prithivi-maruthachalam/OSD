#ifndef ISR_H
#define ISR_H

#define UNUSED(x) (void)(x)
#include <stdint.h>

void register_interrupt_handler(uint32_t IDT_num, uint32_t handler);

struct registers_state
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed));

#endif
