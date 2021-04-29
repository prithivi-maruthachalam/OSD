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
struct zone *zone_normal = NULL;

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

        // printf("Base: %x\tLength:%x\ttype:%d\n", section->base_low, section->length_low, section->type);

        // Initialisation of DMA and normal zones, in that order
        if (section->base_low > DMA_MAX_ADDRESS || zone_DMA->totalSize >= DMA_TOTAL_SIZE)
        {
            // check if this is the first time the normal zone is being considered
            if (zone_normal == NULL)
            {
                zone_normal = (struct zone *)((uint32_t)zone_DMA + zone_DMA->zonePhysicalSize); // put the normal zone structure right after all the DMA zone - related data
                zone_normal->totalSize = 0;
                zone_normal->poolStart = NULL;
                zone_normal->poolEnd = NULL;
                zone_normal->nextZone = NULL;
                zone_normal->zonePhysicalSize = sizeof(struct zone);

                zone_DMA->nextZone = zone_normal;
            }

            // create a new pool and add it to the existing list of NORMAL pools
            currentPool = (struct pool *)((uint32_t)zone_normal + zone_normal->zonePhysicalSize);
            currentPool->start = (uint32_t *)section->base_low;
            currentPool->poolSize = section->length_low;
            currentPool->nextPool = NULL;
            if (zone_normal->poolStart == NULL || zone_normal->poolEnd == NULL)
            {
                // this is the first NORMAL pool
                zone_normal->poolStart = currentPool;
            }
            else
            {
                // just add to list
                zone_normal->poolEnd->nextPool = currentPool;
            }

            zone_normal->poolEnd = currentPool;
            zone_normal->totalSize += currentPool->poolSize;
            zone_normal->zonePhysicalSize += sizeof(struct pool);

            // TODO: Build a buddy map for the size of the current pool and add the size of the entire map to zonePhysicalSize

            section = (struct mmap_entry_t *)((uint32_t)section + (uint32_t)section->size + sizeof(section->size));
        }
        else
        {
            // create a new DMA pool and add it to the existing list of DMA pools
            currentPool = (struct pool *)((uint32_t)zone_DMA + zone_DMA->zonePhysicalSize);
            currentPool->start = (uint32_t *)section->base_low;
            currentPool->nextPool = NULL;
            if (zone_DMA->poolStart == NULL || zone_DMA->poolEnd == NULL)
            {
                // this is the first DMA pool
                zone_DMA->poolStart = currentPool;
            }
            else
            {
                // Add pool to the list
                zone_DMA->poolEnd->nextPool = currentPool;
            }
            zone_DMA->poolEnd = currentPool; // update poolEnd
            zone_DMA->zonePhysicalSize += sizeof(struct pool);

            // Make a decision on how much of the current section is to be added as DMA pool
            if (section->length_low < (DMA_TOTAL_SIZE - zone_DMA->totalSize) && ((uint32_t)currentPool->start + section->length_low - 1) <= DMA_MAX_ADDRESS)
            {
                // adding the entire section as a pool
                currentPool->poolSize = section->length_low;
                zone_DMA->totalSize += section->length_low;
                // TODO: Build buddies for that and add the extra size to zonePhysocalSize

                // move to next section
                section = (struct mmap_entry_t *)((uint32_t)section + (uint32_t)section->size + sizeof(section->size));
                continue;
            }
            else if ((DMA_MAX_ADDRESS - (uint32_t)currentPool->start + 1) < (DMA_TOTAL_SIZE - zone_DMA->totalSize))
            {
                // adding a partial section for the 16MB condition
                currentPool->poolSize = DMA_MAX_ADDRESS - (uint32_t)currentPool->start + 1;
            }
            else
            {
                // adding a partial section for the 256KB condition
                currentPool->poolSize = DMA_TOTAL_SIZE - zone_DMA->totalSize;
            }

            // TODO: Build buddies for currentPool->poolSize and add the extra size to zonePhysocalSize

            // Update zone and section details - to create a new section with the remaining size
            zone_DMA->totalSize += currentPool->poolSize; // Increase zone size
            section->base_low += currentPool->poolSize;   // Advance start of current section
            section->length_low -= currentPool->poolSize; // Reduce size of current section
        }
    }

    printf("DMA ");
    printZoneInfo(zone_DMA);
    printf("\nNormal");
    printZoneInfo(zone_normal);
}

// debugging
void printZoneInfo(struct zone *zone)
{
    printf("Zone info: \n");
    printf(" totalSize: %x\n", zone->totalSize);
    printf(" physicalSize: %x\n", zone->zonePhysicalSize);
    struct pool *p = zone->poolStart;
    while (p != NULL)
    {
        printf(" Pool details: %x\n", p);
        printf("\tPoolStart : %x", p->start);
        printf("\tPoolSize : %x bytes", p->poolSize);
        p = p->nextPool;
        printf("\n");
    }
}