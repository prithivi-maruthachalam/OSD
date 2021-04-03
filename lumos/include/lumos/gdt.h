#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// Functions
void gdt_init();

// structure for gdt entry
struct gdtEntry
{
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t type;
    uint8_t limitHigh_flags;
    uint8_t baseHigh;
} __attribute__((packed));

// structure for gdtr
struct gdtDescriptor
{
    uint16_t limit;
    uint32_t baseAddress;
} __attribute__((packed));

struct TSS_segment
{
    // Previous TSS selector (obsolete, because we're only making one TSS)
    uint16_t taskLink;
    uint16_t taskLink_high_reserved; // Reserved

    uint32_t ESP0;
    uint16_t SS0;
    uint16_t SS0_high_reserved; // Reserved

    uint32_t ESP1;
    uint16_t SS1;
    uint16_t SS1_high_reserved; // Reserved

    uint32_t ESP2;
    uint16_t SS2;
    uint16_t SS2_high_reserved; // Reserved

    uint32_t CR3; // Base of the paging structure
    uint32_t EIP;
    uint32_t EFLAGS;
    uint32_t EAX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t EBX;
    uint32_t ESP;
    uint32_t EBP;
    uint32_t ESI;
    uint32_t EDI;

    // segment selector registers
    uint16_t ES;
    uint16_t ES_high_reserved;

    uint16_t CS;
    uint16_t CS_high_reserved;

    uint16_t SS;
    uint16_t SS_high_reserved;

    uint16_t DS;
    uint16_t DS_high_reserved;

    uint16_t FS;
    uint16_t FS_high_reserved;

    uint16_t GS;
    uint16_t GS_high_reserved;

    // LDT segment selector
    uint16_t LDT;
    uint16_t LDT_high_reserved;

    // IO map
    uint16_t IOMap_low_reserved_T;
    uint16_t IO_Map;
} __attribute__((packed));

#endif