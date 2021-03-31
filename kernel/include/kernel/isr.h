#ifndef ISR_H
#define ISR_H

#include <stdint.h>

struct registers_state
{
    uint32_t EAX, ECX, EDX, RBX;
    uint32_t originalESP;
    uint32_t EBP, ESI, EDI;
} __attribute__((packed));

#endif
