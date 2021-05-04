#include <lumos/pmm.h>
#include <lumos/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

uintptr_t kernel_end = (uintptr_t)&_kernel_end;
uintptr_t kernel_start = (uintptr_t)&_kernel_start;
uintptr_t VIRTUAL_KERNEL_OFFSET = (uintptr_t)&VIRTUAL_KERNEL_OFFSET_LD;

// TODO: Implement a serial port interface for logging -  osdev

// head of the lnked list of zones
struct zone *zone_DMA = (struct zone *)&_kernel_end;
struct zone *zone_normal = NULL;

// debugging
void printZoneInfo(struct zone *zone);

// utils
void makeBuddies(struct pool *pool);
void set_bit(uint32_t *mapStart, uint32_t offset);                             // set a single bit
void set_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd);   // set a section of bits
void unset_bit(uint32_t *mapStart, uint32_t offset);                           // unset a single bit
void unset_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd); // unset a section of bits

void init_pmm(multiboot_info_t *mbtStructure)
{
    struct pool *currentPool; // Do I need this?

    // make sure we have a valid memory map - 6th bit of flags indicates whether the mmap_addr & mmp_length fields are valid
    if (!(mbtStructure->flags & MBT_FLAG_IS_MMAP))
    {
        logf("[PMM] : No memory map available\n");
        abort();
    }

    // Initialize the DMA zone descriptor
    zone_DMA->totalSize = 0;
    zone_DMA->freeMem = 0;
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

        logf("Base: %x\tLength:%x\ttype:%d\n", section->base_low, section->length_low, section->type);

        /* Add the current section as a pool to the NORMAL zone or the DMA zone. Additions to the
        DMA zone can be partial or complete base on the 256KB max size and the 16MB address limit */

        // If DMA zone is done, add the entire section as a NORMAL zone pool
        if (section->base_low > DMA_MAX_ADDRESS || zone_DMA->totalSize >= DMA_TOTAL_SIZE)
        {
            // check if this is the first time the normal zone is being considered
            if (zone_normal == NULL)
            {
                zone_normal = (struct zone *)((uint32_t)zone_DMA + zone_DMA->zonePhysicalSize); // put the normal zone structure right after all the DMA zone - related data
                zone_normal->totalSize = 0;
                zone_normal->freeMem = 0;
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
            currentPool->freeMem = section->length_low;
            currentPool->nextPool = NULL;
            currentPool->poolPhysicalSize = sizeof(struct pool);
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

            zone_normal->totalSize += section->length_low;
            zone_normal->freeMem += section->length_low;
            zone_normal->poolEnd = currentPool;

            // TODO: Build a buddy map for the size of the current pool and add the size of the entire map to zonePhysicalSize
            makeBuddies(currentPool);

            zone_normal->zonePhysicalSize += currentPool->poolPhysicalSize;
            section = (struct mmap_entry_t *)((uint32_t)section + (uint32_t)section->size + sizeof(section->size));
        }
        // else, we've not given up on DMA yet, add the current section as a DMA pool either entirely or partially
        else
        {
            // create a new DMA pool and add it to the existing list of DMA pools
            currentPool = (struct pool *)((uint32_t)zone_DMA + zone_DMA->zonePhysicalSize);
            currentPool->start = (uint32_t *)section->base_low;
            currentPool->nextPool = NULL;
            currentPool->poolPhysicalSize = sizeof(struct pool);
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

            // Make a decision on how much of the current section is to be added as DMA pool
            if (section->length_low < (DMA_TOTAL_SIZE - zone_DMA->totalSize) && ((uint32_t)currentPool->start + section->length_low - 1) <= DMA_MAX_ADDRESS)
            {
                // adding the entire section as a pool
                currentPool->poolSize = section->length_low;
                currentPool->freeMem = section->length_low;
                zone_DMA->totalSize += section->length_low;
                zone_DMA->freeMem += section->length_low;

                // TODO: Build buddies for that and add the extra size to zonePhysocalSize
                makeBuddies(currentPool);
                zone_DMA->zonePhysicalSize += currentPool->poolPhysicalSize;

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

            currentPool->freeMem = currentPool->poolSize; // init free mem for this pool

            // Update zone and section details - to create a new section with the remaining size
            zone_DMA->totalSize += currentPool->poolSize; // Increase zone size
            zone_DMA->freeMem += currentPool->poolSize;
            section->base_low += currentPool->poolSize;   // Advance start of current section
            section->length_low -= currentPool->poolSize; // Reduce size of current section

            // TODO: Build buddies for currentPool->poolSize and add the extra size to zonePhysocalSize
            makeBuddies(currentPool);
            zone_DMA->zonePhysicalSize += currentPool->poolPhysicalSize;
        }
    }

    logf("DMA ");
    printZoneInfo(zone_DMA);
    logf("\nNormal");
    printZoneInfo(zone_normal);
}

// utils
void set_bit(uint32_t *mapStart, uint32_t offset)
{
    mapStart[offset / 32] |= 1 << (offset % 32);
}

void set_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd)
{
    uint32_t i;
    for (i = offsetStart; i <= offsetEnd && i % 32 != 0; i++)
    {
        set_bit(mapStart, i);
    }
    for (; i + 32 <= offsetEnd; i = i + 32)
    {
        mapStart[i / 32] = 0xFFFFFFFF;
    }
    for (; i <= offsetEnd; i++)
    {
        set_bit(mapStart, i);
    }
}

void unset_bit(uint32_t *mapStart, uint32_t offset)
{
    mapStart[offset / 32] &= ~(1 << (offset % 32));
}

void unset_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd)
{
    uint32_t i;
    for (i = offsetStart; i <= offsetEnd && i % 32 != 0; i++)
    {
        unset_bit(mapStart, i);
    }
    for (; i + 32 <= offsetEnd; i = i + 32)
    {
        mapStart[i / 32] = 0;
    }
    for (; i <= offsetEnd; i++)
    {
        unset_bit(mapStart, i);
    }
}

void makeBuddies(struct pool *pool)
{
    uint8_t i;
    struct buddy *currentBuddy;
    uint32_t baseBlockSize = 0;
    uint32_t startBit, endBit; // to be used as bitmap indexes (bitwise)

    logf("\nPool @ %x | Size : %x\n", pool, pool->poolSize);

    // This will be multiplied with powers of two for the blockCounts
    baseBlockSize = pool->poolSize / (BLOCK_SIZE * MAX_BLOCK_ORDER) + (pool->poolSize % (BLOCK_SIZE * MAX_BLOCK_ORDER) != 0);

    for (i = 1; i <= MAX_BLOCK_ORDER; i = i << 1)
    {
        // Initialize the first buddy - right after the end of the pool structure
        currentBuddy = (struct buddy *)((uint32_t)pool + pool->poolPhysicalSize);
        currentBuddy->buddyOrder = MAX_BLOCK_ORDER / i;
        currentBuddy->blockCount = baseBlockSize * i;
        currentBuddy->mapWordCount = (currentBuddy->blockCount / 32 + (currentBuddy->blockCount % 32 != 0));
        currentBuddy->bitMap = (uint32_t *)((uint32_t)pool + pool->poolPhysicalSize + sizeof(struct buddy));
        pool->poolPhysicalSize += sizeof(struct buddy) + (currentBuddy->mapWordCount * 4); // add size of first bitmap and buddy structure to physicalSize

        // Bit offsets calculation
        startBit = 0;
        endBit = (pool->poolSize / (BLOCK_SIZE * currentBuddy->buddyOrder)) - 1;

        logf("\n\tInitialised buddy @ %x :\n", currentBuddy);
        logf("\t\tOrder: %d\n", currentBuddy->buddyOrder);
        logf("\t\tBlockCount : %x\n", currentBuddy->blockCount);
        logf("\t\tTakes up %x bytes\n", currentBuddy->mapWordCount * 4);
        logf("\t\tStartBit: %d\tEndBit: %d\n", startBit, endBit);
    }
}

// debugging
void printZoneInfo(struct zone *zone)
{
    logf("Zone info @ %x: \n", zone);
    logf("  totalSize: %x\n", zone->totalSize);
    logf("  freeMem: %x\n", zone->freeMem);
    logf("  physicalSize: %x\n", zone->zonePhysicalSize);
    struct pool *p = zone->poolStart;
    while (p != NULL)
    {
        logf(" Pool details: %x\n", p);
        logf("\tPoolStart : %x", p->start);
        logf("\tPoolSize : %x bytes", p->poolSize);
        logf("\tfreeMem : %x bytes", p->freeMem);
        p = p->nextPool;
        logf("\n");
    }
}