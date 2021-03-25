#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// constants
void gdt_init();

// structure for gdt entry
struct gdtEntry
{
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t access;
    uint8_t limitHigh_flags;
    uint8_t baseHigh;
} __attribute__((packed));

// structure for gdt_ptr
struct gdtDescriptor
{
    uint16_t limit;
    uint32_t baseAddress;
} __attribute__((packed));

#endif