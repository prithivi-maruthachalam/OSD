#include <lumos/pmm.h>
#include <lumos/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#define CEIL(x, y) ((int)x / y) + (x % y != 0) ? 1 : 0;
#define getBitOffset(start, target, blockSize) ((uint32_t)target - (uint32_t)start) / blockSize
#define VIRT(x) x + VIRTUAL_KERNEL_OFFSET

uintptr_t kernel_end = (uintptr_t)&_kernel_end;
uintptr_t kernel_start = (uintptr_t)&_kernel_start;
uintptr_t VIRTUAL_KERNEL_OFFSET = (uintptr_t)&VIRTUAL_KERNEL_OFFSET_LD;

/*
    The available physical memory is divided into two
    types of zones. DMA zone handles a mazimum of 256K 
    of the available physical memory space, upto a max address
    of 16M.
*/
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
bool test_bit(uint32_t *mapStart, uint32_t offset);                            // return the value of a bit
void reserve_kernel();                                                         // Mark the space used by the kernel and the pmm structures as reserved

/*
    Initialising the Physical Memory Manager
    ----------------------------------------
    This involves going through the memory map provided GRUB. For each free region,
    a decision is made to either add it to the linked list of NORMAL zone pools of memory or 
    to the DMA pools. A section will be added only partially to the set of DMA pools if
    adding the section increases the total size of the DMA zone above 256K or makes the addresses 
    of the DMA zone above 16M. For each pool, it sets up the pool's buddies using the makeBuddies function.
*/
void init_pmm(multiboot_info_t *mbtStructure)
{
    // temp pointers to work wit pools inside loops
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

    // go through the memory map from the multiboot info structure
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
        DMA zone can be partial or complete based on the 256KB max size and the 16MB address limit */

        // If we're done with the DMA, add the entire section as a NORMAL zone pool
        if (section->base_low > DMA_MAX_ADDRESS || zone_DMA->totalSize >= DMA_TOTAL_SIZE)
        {
            /*  If this is the first time the NORMAL zone is being considered, initialize the 
                NORMAL zone descriptor
             */
            if (zone_normal == NULL)
            {
                zone_normal = (struct zone *)((uint32_t)zone_DMA + zone_DMA->zonePhysicalSize); // put the normal zone structure right after all the DMA zone - related data
                zone_normal->zoneType = 1;
                zone_normal->totalSize = 0;
                zone_normal->freeMem = 0;
                zone_normal->poolStart = NULL;
                zone_normal->zonePhysicalSize = sizeof(struct zone);
            }

            // Create a new pool and add it to the existing list of NORMAL pools
            currentPool = (struct pool *)((uint32_t)zone_normal + zone_normal->zonePhysicalSize);
            currentPool->start = section->base_low;
            currentPool->poolSize = section->length_low;
            currentPool->freeMem = section->length_low;
            currentPool->nextPool = NULL;
            currentPool->poolBuddiesTop = NULL;
            currentPool->poolPhysicalSize = sizeof(struct pool);

            zone_normal->freeMem += section->length_low;
            zone_normal->totalSize += section->length_low;

            makeBuddies(currentPool);

            zone_normal->zonePhysicalSize += currentPool->poolPhysicalSize;

            if (zone_normal->poolStart == NULL)
                zone_normal->poolStart = currentPool; // this is the first NORMAL pool
            else
                previousPool->nextPool = currentPool; // update link from previous pool

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
                zone_DMA->poolStart = currentPool; // this is the first DMA pool
            else
                previousPool->nextPool = currentPool; // Add pool to the list

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

                section = (struct mmap_entry_t *)((uint32_t)section + section->size + sizeof(section->size));
                continue;
            }
            else if ((DMA_MAX_ADDRESS - currentPool->start + 1) < (DMA_TOTAL_SIZE - zone_DMA->totalSize))
            {
                // adding a partial section because of the 16MB condition
                currentPool->poolSize = DMA_MAX_ADDRESS - currentPool->start + 1;
            }
            else
            {
                // adding a partial section because the 256KB condition
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

    // logging pmm structures
    logf("DMA ");
    printZoneInfo(zone_DMA);
    logf("Normal");
    printZoneInfo(zone_normal);

    // Mark kernel space and pmm space as reserved
    reserve_kernel();

    // logging pmm structures
    logf("DMA ");
    printZoneInfo(zone_DMA);
    logf("Normal");
    printZoneInfo(zone_normal);
}

/* 
    Mark the space used by the kernel and the pmm structures as reserved.
    This creates some free blocks. Mark them and add the count of free 
    blocks to their respective buddies.
*/
void reserve_kernel()
{
    uint32_t resStart = (uint32_t)kernel_start - VIRTUAL_KERNEL_OFFSET;
    uint32_t resEnd = (uint32_t)kernel_end + zone_DMA->zonePhysicalSize + zone_normal->zonePhysicalSize - 1 - VIRTUAL_KERNEL_OFFSET;
    uint32_t startOffset = 0, eStart = 0, endOffSet = 0, eEnd = 0;

    uint32_t i;

    struct buddy *currentBuddy;
    struct pool *currentPool;
    currentPool = zone_normal->poolStart;

    logf("Kernel start: %x\tKernel End: %x\n", resStart, resEnd);
    while (currentPool != NULL)
    {
        // identify the pool that contains the kernel and pmm structures
        if (currentPool->start <= resStart && resStart < (uint32_t)currentPool->start + currentPool->poolSize)
        {
            logf("Pool @ %x\tStart : %x\tSize:%x\n", currentPool, currentPool->start, currentPool->poolSize);

            // Reserve the coresponding blocks in the highest order buddy
            currentBuddy = currentPool->poolBuddiesTop;
            eStart = getBitOffset(currentPool->start, resStart, (currentBuddy->buddyOrder * BLOCK_SIZE));
            eEnd = getBitOffset(currentPool->start, resEnd, (currentBuddy->buddyOrder * BLOCK_SIZE));
            set_bits(currentBuddy->bitMap, eStart, eEnd);
            currentBuddy->freeBlocks -= (eEnd - eStart + 1); // reduce the number of free blocks

            currentBuddy = currentBuddy->nextBuddy;

            while (currentBuddy != NULL)
            {
                logf("\nOrder: %d\tFreeBlocks: %x\tMaxFree: %x\n", currentBuddy->buddyOrder, currentBuddy->freeBlocks, currentBuddy->maxFreeBlocks);
                logf("\tStart Block: %x\tEnd Block: %x\n", startOffset, endOffSet);
                startOffset = getBitOffset(currentPool->start, resStart, (currentBuddy->buddyOrder * BLOCK_SIZE));
                endOffSet = getBitOffset(currentPool->start, resEnd, (currentBuddy->buddyOrder * BLOCK_SIZE));

                // If this overlaps a region that is already available, that region has to be reserved
                for (i = startOffset; i <= endOffSet; i++)
                {
                    if (test_bit(currentBuddy->bitMap, i) == 0)
                    {
                        logf("\tBit %d was found to be prviously free. Reserving\n", i);
                        currentBuddy->freeBlocks--;
                    }
                }

                /* 
                    When all the children of a parent arent being reserved,
                    we get some free blocks in the beginning and in the end.
                    Let's free them! #freealltheorphanedblocks
                */
                if (startOffset != (eStart * 2))
                {
                    unset_bits(currentBuddy->bitMap, (eStart * 2), (startOffset - 1));
                    currentBuddy->freeBlocks += startOffset - (eStart * 2);
                    logf("\tUnsetting %d to %d and adding %d free blocks\n", (eStart * 2), (startOffset - 1), startOffset - (eStart * 2));
                }

                if (endOffSet != (eEnd * 2) + 1)
                {
                    unset_bits(currentBuddy->bitMap, endOffSet + 1, (eEnd * 2) + 1);
                    currentBuddy->freeBlocks += (eEnd * 2) - endOffSet + 1;
                    logf("\tUnsetting %d to %d and adding %d free blocks\n", endOffSet + 1, (eEnd * 2) + 1, (eEnd * 2) - endOffSet + 1);
                }

                eStart *= 2;
                eEnd = (eEnd * 2) + 1;
                currentBuddy = currentBuddy->nextBuddy;
            }
            break;
        }
        currentPool = currentPool->nextPool;
    }

    logf("\n------------------------------------------------------\n\n");
}

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

bool test_bit(uint32_t *mapStart, uint32_t offset)
{
    return (mapStart[offset / 32] & (1 << (offset % 32)));
}

/*
    Creates structures and initializes bitmaps for the buddies of a given pool.  
    The blocks of the highest order are all set to available, while the lower 
    order ones have all their blocks set to reserved (but the ones that do not 
    belong to a larger parent block are set to available).
*/
void makeBuddies(struct pool *pool)
{
    struct buddy *currentBuddy;
    struct buddy *previousBuddy = NULL;
    printf("\n\n");
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

        // if maxFreeBlocks is equal to the actual number of free blocks, this is the highest order buddy
        if (currentBuddy->maxFreeBlocks == currentBuddy->freeBlocks)
        {
            memset(currentBuddy->bitMap, 0, currentBuddy->mapWordCount * 4); // set entire region to available
        }
        // else, this is a lower order buddy - an extra block at the end might be available
        else
        {
            memset(currentBuddy->bitMap, 0xFF, currentBuddy->mapWordCount * 4); // set entire region to reserved
            // check if a block at the end is actually available
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
    for (uint32_t i = 0; i < wordCount && i < 50; i++)
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
            logf("\t\tBitMap @ %x: ", b->bitMap);
            printBuddy(b->bitMap, b->mapWordCount);
            logf("\n");
            b = b->nextBuddy;
        }
        p = p->nextPool;
    }
    logf("---------------------------------------------\n\n");
}
