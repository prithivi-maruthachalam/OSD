#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#define IDT_SIZE 256
#define KERNEL_CODE_SELECTOR 0x08

// Functions
void idt_init();

// Structure for each IDT entry
struct idtEntry
{
    uint16_t baseLow;
    uint16_t segmentSelector;
    uint8_t reserved; // Must be 0
    uint8_t type;
    uint16_t baseHigh;
} __attribute__((packed));

// structure for idtr
struct idtDescriptor
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
#endif