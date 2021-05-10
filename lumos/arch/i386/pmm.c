#include <lumos/pmm.h>
#include <lumos/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define CEIL(x, y) ((int)x / y) + (x % y != 0) ? 1 : 0;
#define getBitOffset(start, target, blockSize) ((uint32_t)target - (uint32_t)start) / blockSize
#define VIRT(x) x + VIRTUAL_KERNEL_OFFSET

uintptr_t kernel_end = (uintptr_t)&_kernel_end;
uintptr_t kernel_start = (uintptr_t)&_kernel_start;
uintptr_t VIRTUAL_KERNEL_OFFSET = (uintptr_t)&VIRTUAL_KERNEL_OFFSET_LD;

// TODO: Implement a serial port interface for logging -  osdev

// head of the lnked list of zones
struct zone *zone_DMA = (struct zone *)&_kernel_end;
struct zone *zone_normal = NULL;

// debugging
void printZoneInfo(struct zone *zone);
void printBuddy(uint32_t *map, uint32_t mapWordCount);

// utils
void makeBuddies(struct pool *pool);
void set_bit(uint32_t *mapStart, uint32_t offset);                             // set a single bit
void set_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd);   // set a section of bits
void unset_bit(uint32_t *mapStart, uint32_t offset);                           // unset a single bit
void unset_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd); // unset a section of bits

void init_pmm(multiboot_info_t *mbtStructure)
{
    struct pool *currentPool;
    struct pool *previousPool = NULL;

    // make sure we have a valid memory map - 6th bit of flags indicates whether the mmap_addr & mmp_length fields are valid
    if (!(mbtStructure->flags & MBT_FLAG_IS_MMAP))
    {
        logf("[PMM] : No memory map available\n");
        abort();
    }

    // Initialize the DMA zone descriptor
    zone_DMA->zoneType = 0;
    zone_DMA->totalSize = 0;
    zone_DMA->freeMem = 0;
    zone_DMA->poolStart = NULL;
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

        /* Add the current section as a pool to the NORMAL zone or the DMA zone. Additions to the
        DMA zone can be partial or complete base on the 256KB max size and the 16MB address limit */

        // If DMA zone is done, add the entire section as a NORMAL zone pool
        if (section->base_low > DMA_MAX_ADDRESS || zone_DMA->totalSize >= DMA_TOTAL_SIZE)
        {
            // check if this is the first time the normal zone is being considered
            if (zone_normal == NULL)
            {
                zone_normal = (struct zone *)((uint32_t)zone_DMA + zone_DMA->zonePhysicalSize); // put the normal zone structure right after all the DMA zone - related data
                zone_normal->zoneType = 1;
                zone_normal->totalSize = 0;
                zone_normal->freeMem = 0;
                zone_normal->poolStart = NULL;
                zone_normal->zonePhysicalSize = sizeof(struct zone);
            }

            // create a new pool and add it to the existing list of NORMAL pools
            currentPool = (struct pool *)((uint32_t)zone_normal + zone_normal->zonePhysicalSize);
            currentPool->start = section->base_low;
            currentPool->poolSize = section->length_low;
            currentPool->freeMem = section->length_low;
            currentPool->nextPool = NULL;
            currentPool->poolBuddiesTop = NULL;
            currentPool->poolPhysicalSize = sizeof(struct pool);
            if (zone_normal->poolStart == NULL)
            {
                zone_normal->poolStart = currentPool; // this is the first NORMAL pool
            }
            else
            {
                previousPool->nextPool = currentPool;
            }

            zone_normal->totalSize += section->length_low;
            zone_normal->freeMem += section->length_low;

            makeBuddies(currentPool);

            zone_normal->zonePhysicalSize += currentPool->poolPhysicalSize;
            previousPool = currentPool;
            section = (struct mmap_entry_t *)((uint32_t)section + (uint32_t)section->size + sizeof(section->size));
        }
        // else, we've not given up on DMA yet, add the current section as a DMA pool either entirely or partially
        else
        {
            // create a new DMA pool and add it to the existing list of DMA pools
            currentPool = (struct pool *)((uint32_t)zone_DMA + zone_DMA->zonePhysicalSize);
            currentPool->start = section->base_low;
            currentPool->nextPool = NULL;
            currentPool->poolBuddiesTop = NULL;
            currentPool->poolPhysicalSize = sizeof(struct pool);
            if (zone_DMA->poolStart == NULL)
            {
                zone_DMA->poolStart = currentPool; // this is the first DMA pool
            }
            else
            {
                // Add pool to the list
                previousPool->nextPool = currentPool;
            }

            // Make a decision on how much of the current section is to be added as DMA pool
            if (section->length_low < (DMA_TOTAL_SIZE - zone_DMA->totalSize) && (currentPool->start + section->length_low - 1) <= DMA_MAX_ADDRESS)
            {
                // adding the entire section as a pool
                currentPool->poolSize = section->length_low;
                currentPool->freeMem = section->length_low;
                zone_DMA->totalSize += section->length_low;
                zone_DMA->freeMem += section->length_low;

                makeBuddies(currentPool);
                zone_DMA->zonePhysicalSize += currentPool->poolPhysicalSize;

                previousPool = currentPool;
                // move to next section
                section = (struct mmap_entry_t *)((uint32_t)section + section->size + sizeof(section->size));
                continue;
            }
            else if ((DMA_MAX_ADDRESS - currentPool->start + 1) < (DMA_TOTAL_SIZE - zone_DMA->totalSize))
            {
                // adding a partial section for the 16MB condition
                currentPool->poolSize = DMA_MAX_ADDRESS - currentPool->start + 1;
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

            makeBuddies(currentPool);
            zone_DMA->zonePhysicalSize += currentPool->poolPhysicalSize;

            previousPool = currentPool;
        }
    }

    // reserve space for ther kernel
    uint32_t resSize = zone_DMA->zonePhysicalSize + zone_normal->zonePhysicalSize + kernel_end - kernel_start;
    uint32_t resStart = (uint32_t)kernel_start - VIRTUAL_KERNEL_OFFSET;
    printf("resStart: %x\tresEnd: %x\n", resStart, resStart + resSize - 1);
    struct buddy *currentBuddy;
    uint32_t startOffset, lastStart;
    uint32_t endOffSet, lastEnd;
    currentPool = zone_normal->poolStart;
    while (currentPool != NULL)
    {
        if (currentPool->start <= resStart && resStart < (uint32_t)currentPool->start + currentPool->poolSize)
        {
            printf("Pool @ %x\tStart : %x\tSize:%x\n", currentPool, currentPool->start, currentPool->poolSize);
            currentBuddy = currentPool->poolBuddiesTop;
            while (currentBuddy != NULL)
            {
                printf("\nOrder: %d\tFreeBlocks: %x\tMaxFree: %x\n", currentBuddy->buddyOrder, currentBuddy->freeBlocks, currentBuddy->maxFreeBlocks);

                startOffset = getBitOffset(currentPool->start, resStart, (currentBuddy->buddyOrder * BLOCK_SIZE));
                endOffSet = getBitOffset(currentPool->start, (resStart + resSize - 1), (currentBuddy->buddyOrder * BLOCK_SIZE));

                unset_bits(currentBuddy->bitMap, startOffset, endOffSet);

                if (currentBuddy->buddyOrder < MAX_BLOCK_ORDER)
                {
                    printf("\tAdding %d to freeblocks\n", (startOffset - (2 * lastStart)) + ((lastEnd * 2) + 1 - endOffSet));
                    currentBuddy->freeBlocks += (startOffset - (2 * lastStart)) + ((lastEnd * 2) + 1 - endOffSet);
                    lastStart *= lastStart;
                    lastEnd = (lastEnd * 2) + 1;
                }
                else
                {
                    lastStart = startOffset;
                    lastEnd = endOffSet;
                }

                currentBuddy = currentBuddy->nextBuddy;

                // debugging
                printf("\tStart Block: %x\tEnd Block: %x\n", startOffset, endOffSet);
            }
            break;
        }
        currentPool = currentPool->nextPool;
    }

    logf("DMA ");
    printZoneInfo(zone_DMA);
    logf("Normal");
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
    // set bit by bit until we reach the start of a word
    for (i = offsetStart; i <= offsetEnd && i % 32 != 0; i++)
        set_bit(mapStart, i);

    // set word by word, until a word would be too big for offsetEnd
    for (; i + 32 <= offsetEnd; i = i + 32)
        mapStart[i / 32] = 0xFFFFFFFF;

    // set bit by bit until the end
    for (; i <= offsetEnd; i++)
        set_bit(mapStart, i);
}

void unset_bit(uint32_t *mapStart, uint32_t offset)
{
    mapStart[offset / 32] &= ~(1 << (offset % 32));
}

void unset_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd)
{
    uint32_t i;
    // unset bit by bit until we reach the start of a word
    for (i = offsetStart; i <= offsetEnd && i % 32 != 0; i++)
        unset_bit(mapStart, i);

    // set word by word, until a word would be too big for offsetEnd
    for (; i + 32 <= offsetEnd; i = i + 32)
        mapStart[i / 32] = 0;

    // set bit by bit until the end
    for (; i <= offsetEnd; i++)
        unset_bit(mapStart, i);
}

void makeBuddies(struct pool *pool)
{
    struct buddy *currentBuddy;
    struct buddy *previousBuddy = NULL;

    for (uint8_t i = MAX_BLOCK_ORDER; i > 0; i = i >> 1)
    {
        currentBuddy = (struct buddy *)((uint32_t)pool + pool->poolPhysicalSize); // put the current buddy right after the previous structures
        currentBuddy->buddyOrder = i;                                             // buddy order in terms of powers of 2
        currentBuddy->maxFreeBlocks = pool->poolSize / (BLOCK_SIZE * i);          // max possible allocations for this order
        currentBuddy->freeBlocks = (previousBuddy == NULL) ? currentBuddy->maxFreeBlocks : currentBuddy->maxFreeBlocks - (previousBuddy->maxFreeBlocks * 2);
        currentBuddy->mapWordCount = (currentBuddy->maxFreeBlocks / 32) + (currentBuddy->maxFreeBlocks % 32 != 0);
        currentBuddy->bitMap = (uint32_t *)((uint32_t)pool + pool->poolPhysicalSize + sizeof(struct buddy));
        currentBuddy->nextBuddy = NULL;
        pool->poolPhysicalSize += sizeof(struct buddy) + (currentBuddy->mapWordCount * 4);

        if (currentBuddy->maxFreeBlocks == currentBuddy->freeBlocks)
        {
            memset(currentBuddy->bitMap, 0, currentBuddy->mapWordCount * 4); // set entire region to 0
        }
        else
        {
            memset(currentBuddy->bitMap, 0xFF, currentBuddy->mapWordCount * 4); // set entire region to 1
            if (currentBuddy->freeBlocks > 0)
                unset_bit(currentBuddy->bitMap, (currentBuddy->maxFreeBlocks - currentBuddy->freeBlocks)); // set that one last directly usable block to 0
        }

        // linked list stuff
        if (pool->poolBuddiesTop == NULL)
            pool->poolBuddiesTop = currentBuddy;
        else
            previousBuddy->nextBuddy = currentBuddy;
        previousBuddy = currentBuddy;
    }
}

// debugging
void printBuddy(uint32_t *map, uint32_t wordCount)
{
    if (wordCount > 50)
        return;
    for (uint32_t i = 0; i < wordCount; i++)
        logf(" %x | ", map[i]);
    logf("\n");
}

void printZoneInfo(struct zone *zone)
{
    logf("Zone info @ %x: \n", zone);
    logf("  totalSize: %x\n", zone->totalSize);
    logf("  freeMem: %x\n", zone->freeMem);
    logf("  physicalSize: %x\n\n", zone->zonePhysicalSize);
    struct pool *p = zone->poolStart;
    struct buddy *b;
    while (p != NULL)
    {
        logf("  Pool details: %x\n", p);
        logf("\tPoolStart : %x\n", p->start);
        logf("\tPoolSize : %x bytes\n", p->poolSize);
        logf("\tfreeMem : %x bytes\n", p->freeMem);
        b = p->poolBuddiesTop;
        while (b != NULL)
        {
            logf("\tBuddy of order %d @ %x:\n", b->buddyOrder, b);
            logf("\t\tMapWordCount : %d\n", b->mapWordCount);
            logf("\t\tMaxFreeBlocks: %d\n", b->maxFreeBlocks);
            logf("\t\tRealFreeBlocks: %d\n", b->freeBlocks);
            logf("\t\t");
            printBuddy(b->bitMap, b->mapWordCount);
            logf("\n");
            b = b->nextBuddy;
        }
        p = p->nextPool;
    }
    logf("---------------------------------------------\n\n");
}
