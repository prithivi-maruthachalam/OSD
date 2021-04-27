#include <lumos/pmm.h>
#include <lumos/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uintptr_t kernel_end = (uintptr_t)&_kernel_end;
uintptr_t kernel_start = (uintptr_t)&_kernel_start;
uintptr_t VIRTUAL_KERNEL_OFFSET = (uintptr_t)&VIRTUAL_KERNEL_OFFSET_LD;

// head of the lnked list of zones
struct zone *zone_DMA = (struct zone *)&_kernel_end;
struct zone *zone_normal;

void init_pmm(multiboot_info_t *mbtStructure)
{
    // make sure we have a valid memory map - 6th bit of flags indicates whether the mmap_addr & mmp_length fields are valid
    if (!(mbtStructure->flags & MBT_FLAG_IS_MMAP))
    {
        printf("[PMM] : No memory map available\n");
        abort();
    }

    // traversing the memory map
    struct mmap_entry_t *entry = (struct mmap_entry_t *)(mbtStructure->mmap_addr + VIRTUAL_KERNEL_OFFSET);
    while (entry < (struct mmap_entry_t *)(mbtStructure->mmap_addr + mbtStructure->mmap_length + VIRTUAL_KERNEL_OFFSET))
    {
        /* 
            Make sure we're only using the 32-bit address space(4G) - will think of PAE later
            Also make sure we're only going through the free regions
        */
        if (entry->base_high || entry->type != 1)
        {
            entry = (struct mmap_entry_t *)((uint32_t)entry + (uint32_t)entry->size + sizeof(entry->size));
            continue;
        }

        printf("Base: %x\tLength:%x\ttype:%d\n", entry->base_low, entry->length_low, entry->type);
        entry = (struct mmap_entry_t *)((uint32_t)entry + (uint32_t)entry->size + sizeof(entry->size));
    }
}
