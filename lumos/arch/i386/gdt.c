#include <lumos/gdt.h>

struct gdtDescriptor GDTR;
struct gdtEntry GDT_entries[6];
volatile struct TSS_segment TSS;

extern void load_gdt(struct gdtDescriptor *);

// to create each entry()
void gdt_create_entry(struct gdtEntry *entry, uint32_t base, uint32_t limit, uint8_t type, uint8_t flags)
{
    // setting base
    entry->baseLow = (uint16_t)base & 0x0000FFFF;
    entry->baseMid = (uint8_t)(base >> 16) & 0x000000FF;
    entry->baseHigh = (uint8_t)(base >> 24) & 0x000000FF;

    // limit
    entry->limitLow = (uint16_t)limit & 0x0000FFFF;
    entry->limitHigh_flags = (uint8_t)(limit >> 16) & 0x0000000F;

    // flags
    entry->limitHigh_flags |= flags;

    // type flags
    entry->type = type;
}

void gdt_init()
{
    // set up gdt entries
    // entry pointer | base | limit | type | flags
    gdt_create_entry(&GDT_entries[0], 0, 0, 0, 0);
    gdt_create_entry(&GDT_entries[1], 0, 0xFFFFFFFF, 0x9A, 0xC0); // kernel code
    gdt_create_entry(&GDT_entries[2], 0, 0xFFFFFFFF, 0x92, 0xC0); // kernel data
    gdt_create_entry(&GDT_entries[3], 0, 0xFFFFFFFF, 0xFA, 0xC0); // user code
    gdt_create_entry(&GDT_entries[4], 0, 0xFFFFFFFF, 0xF2, 0xC0); // user data
    gdt_create_entry(&GDT_entries[5], (uint32_t)&TSS, (sizeof(TSS) - 1), 0x89, 0x10);
    TSS.SS0 = 0x10; // kernel data
    // todo: set ESP0

    GDTR.limit = sizeof(GDT_entries) - 1;
    GDTR.baseAddress = (uint32_t)&GDT_entries;
    load_gdt(&GDTR);
}