#include <lumos/pmm.h>
#include <lumos/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

uintptr_t kernel_end = (uintptr_t)&_kernel_end;
uintptr_t kernel_start = (uintptr_t)&_kernel_start;
uintptr_t VIRTUAL_KERNEL_OFFSET = (uintptr_t)&VIRTUAL_KERNEL_OFFSET_LD;

// head of the lnked list of zones
struct zone *zone_DMA = (struct zone *)&_kernel_end;
struct zone *zone_normal;

// debugging
void printZoneInfo(struct zone *zone);

void init_pmm(multiboot_info_t *mbtStructure)
{
    struct pool *currentPool;

    // make sure we have a valid memory map - 6th bit of flags indicates whether the mmap_addr & mmp_length fields are valid
    if (!(mbtStructure->flags & MBT_FLAG_IS_MMAP))
    {
        printf("[PMM] : No memory map available\n");
        abort();
    }

    // Initialize the DMA zone descriptor
    zone_DMA->totalSize = 0;
    zone_DMA->poolStart = NULL;
    zone_DMA->poolEnd = NULL;
    zone_DMA->nextZone = NULL;
    zone_DMA->zonePhysicalSize = sizeof(struct zone);

    // Initialize the NORMAL ZONE pointer
    zone_normal = NULL; // Don't know the location until we're done with DMA

    // traversing the memory map
    struct mmap_entry_t *section = (struct mmap_entry_t *)(mbtStructure->mmap_addr + VIRTUAL_KERNEL_OFFSET);
    while (section < (struct mmap_entry_t *)(mbtStructure->mmap_addr + mbtStructure->mmap_length + VIRTUAL_KERNEL_OFFSET))
    {
        /* 
            Make sure we're only using the 32-bit address space(4G) - will think of PAE later
            Also make sure we're only going through the free regions
        */
        if (section->base_high || section->type != 1)
        {
            section = (struct mmap_entry_t *)((uint32_t)section + (uint32_t)section->size + sizeof(section->size));
            continue;
        }

        printf("Base: %x\tLength:%x\ttype:%d\n", section->base_low, section->length_low, section->type);

        // Initialisation of DMA and normal zones, in that order
        if (section->base_low > DMA_MAX_ADDRESS || zone_DMA->totalSize >= DMA_TOTAL_SIZE)
        {
            // Add this section as a NORMAl zone pool
        }
        else
        {
            // create a new DMA pool and add it to the existing list
            currentPool = (struct pool *)((uint32_t)zone_DMA + zone_DMA->zonePhysicalSize);
            currentPool->poolStart = (uint32_t *)section->base_low;
            currentPool->nextPool = NULL;
            if (zone_DMA->poolStart == NULL || zone_DMA->poolEnd == NULL)
            {
                // the the first pool
                zone_DMA->poolStart = currentPool;
                printf("\t\tCreating new pool @ %x\n", currentPool);
            }
            else
            {
                // Add pool to the list
                zone_DMA->poolEnd->nextPool = currentPool;
            }
            zone_DMA->poolEnd = currentPool; // update poolEnd
            zone_DMA->zonePhysicalSize += sizeof(struct pool);
            printf("\t\tNew pool added @ %x with \n", zone_DMA->poolEnd);
        }
        section = (struct mmap_entry_t *)((uint32_t)section + (uint32_t)section->size + sizeof(section->size));
    }

    printZoneInfo(zone_DMA);
}

// debugging
void printZoneInfo(struct zone *zone)
{
    printf("Zone info: \n");
    printf("zone totalSize: %x\n", zone->totalSize);
    printf("zone physicalSize: %x\n", zone->zonePhysicalSize);
    struct pool *p = zone_DMA->poolStart;
    while (p != NULL)
    {
        printf("\tPool details: %x\n", p);
        printf("\t\tPoolStart : %x", p->poolStart);
        p = p->nextPool;
        printf("\n");
    }
}