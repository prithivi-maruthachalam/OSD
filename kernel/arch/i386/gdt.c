#include <kernel/gdt.h>

struct gdtDescriptor GDTR;
struct gdtEntry GDT_entries[5];

extern void load_gdt(struct gdtDescriptor *);

// to create each entry()
void gdt_create_entry(struct gdtEntry *entry, uint32_t base, uint32_t limit, uint8_t access)
{
    // setting base
    entry->baseLow = (uint16_t)base & 0x0000FFFF;
    entry->baseMid = (uint8_t)(base >> 16) & 0x000000FF;
    entry->baseHigh = (uint8_t)(base >> 24) & 0x000000FF;

    // limit
    entry->limitLow = (uint16_t)limit & 0x0000FFFF;
    entry->limitHigh_flags = (uint8_t)(limit >> 16) & 0x0000000F;

    // flags
    entry->limitHigh_flags |= 0xC0;

    // access flags
    entry->access = access;
}

void gdt_init()
{
    // set up entries
    gdt_create_entry(&GDT_entries[0], 0, 0, 0);
    gdt_create_entry(&GDT_entries[1], 0, 0xFFFFFFFF, 0x9A); // kernel code
    gdt_create_entry(&GDT_entries[2], 0, 0xFFFFFFFF, 0x92); // kernel data
    gdt_create_entry(&GDT_entries[3], 0, 0xFFFFFFFF, 0xFA); // user code
    gdt_create_entry(&GDT_entries[4], 0, 0xFFFFFFFF, 0xF2); // user data
    // TODO: Add TSS descriptor

    GDTR.limit = sizeof(GDT_entries) - 1;
    GDTR.baseAddress = (uint32_t)&GDT_entries;
    load_gdt(&GDTR);
}