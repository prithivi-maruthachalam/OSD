/* 
    BitMap based Physcial Memoru Manager. Uses a first fit allocator for 
    Normal zone memory and a buddy allocator for DMA zone allocation
*/

#include <lumos/pmm.h>
#include <lumos/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <utils.h>

#define CEIL(x, y) ((x / y) + ((x % y) != 0))
#define MIN(x,y) (x < y) ? x : y;
#define MAX(x,y) (x > y) ? x : y;
#define GET_OFFSET(start, target, blockSize) ((uint32_t)target - (uint32_t)start) / blockSize
#define GET_ADDRESS(start, offset, blockSize) (offset * blockSize) + start;

// debugging functions
void printZone(struct pm_zone_t *zone);
void printZones(struct pm_zone_t *zone);
void printBitmap(uint32_t* mapstart, uint32_t mapWords);

// bitmap functions
void set_bit(uint32_t *mapStart, uint32_t offset);                             // set a single bit
void set_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd);   // set a section of bits
void unset_bit(uint32_t *mapStart, uint32_t offset);                           // unset a single bit
void unset_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd); // unset a section of bits
bool test_bit(uint32_t *mapStart, uint32_t offset);

// linker 
uintptr_t kernel_end = (uintptr_t)&_kernel_end;
uintptr_t kernel_start = (uintptr_t)&_kernel_start;
uintptr_t VIRTUAL_KERNEL_OFFSET = (uintptr_t)&VIRTUAL_KERNEL_OFFSET_LD;

// pmm memory zone structures
struct pm_zone_t *memory_zones = (struct pm_zone_t *)&_kernel_end;
struct pm_zone_t *current_zone = NULL;
uint32_t bitmapWords;

void init_pmm(multiboot_info_t *mbtStructure)
{
    logf("[pmm]: init_pmm()\n");

    // make sure we have a valid memory map - 6th bit of flags indicates whether the mmap_addr & mmp_length fields are valid
    if (!(mbtStructure->flags & MBT_FLAG_IS_MMAP))
    {
        logf("[PMM] : No memory map available\n");
        abort();
    }

    uint32_t pmm_structures_size = 0;

    // go through the memory map from the multiboot info structure
    struct mmap_entry_t *section = (struct mmap_entry_t *)(mbtStructure->mmap_addr + VIRTUAL_KERNEL_OFFSET);
    while (section < (struct mmap_entry_t *)(mbtStructure->mmap_addr + mbtStructure->mmap_length + VIRTUAL_KERNEL_OFFSET))
    {
        // Skip reserved sections
        if ((section->base_low == 0 && section->base_high) || section->type != 1)
        {
            section = (struct mmap_entry_t *)((uint32_t)section + (uint32_t)section->size + sizeof(section->size));
            continue;
        }

        // log memory map information
        logf("(*) base: %x\tlen: %x bytes\ttype: %d\n", section->base_low, section->length_low, section->type);

        // linked list stuf
        if(!current_zone){
            current_zone = (struct pm_zone_t *)&_kernel_end;
        } else {
            current_zone->next_zone = (struct pm_zone_t *)((uint32_t)current_zone + sizeof(struct pm_zone_t) + (current_zone->bitmapWords * 4));
            current_zone = current_zone->next_zone;
        }

        current_zone->start = section->base_low;    // start address
        current_zone->length = section->length_low / BLOCK_SIZE;    // number of blocks
        current_zone->free = current_zone->length;  // all blocks are free for now
        current_zone->bitmapWords = CEIL(current_zone->length, WORD);   // number of bytes in the bitmap
        current_zone->bitmap = (uint32_t *) ((uint32_t)current_zone + sizeof(struct pm_zone_t));
        current_zone->next_zone = NULL;

        // unset all the free page bits and set the bits that are used as padding         
        unset_bits(current_zone->bitmap, 0, current_zone->length - 1);
        set_bits(current_zone->bitmap, current_zone->length, (current_zone->bitmapWords * 32) - 1);

        pmm_structures_size += sizeof(struct pm_zone_t) + (current_zone->bitmapWords * 4);

        section = (struct mmap_entry_t *)((uint32_t)section + (uint32_t)section->size + sizeof(section->size));
    }

    /* RESERVING KERNEL PAGES */
    logf("PMM strucutres take %d bytes\n", pmm_structures_size);
    logf("\nReserving kernel blocks\n");
    uint32_t resStart = (uint32_t)kernel_start - VIRTUAL_KERNEL_OFFSET;
    uint32_t resEnd = (uint32_t)kernel_end - VIRTUAL_KERNEL_OFFSET - 1 + pmm_structures_size;
    uint32_t zoneEnd, startbit, endbit;

    logf("kernel start : %x\tkernel end: %x\n", resStart, resEnd);

    // for each zone
    current_zone = memory_zones;
    while(current_zone){
        // current zone end address
        zoneEnd = (uint32_t)current_zone->start + (current_zone->length * BLOCK_SIZE) - 1;
        logf("zone start: %x\tzone end: %x\n", current_zone->start, zoneEnd);

        if((resStart <= current_zone->start && resEnd >= current_zone->start) || 
            (resStart >= current_zone->start && resStart <= zoneEnd)){
            
            logf("kernel and zone overlap\n");

            // get bit offsets (offsets are inclusive)
            startbit = MAX(current_zone->start, resStart);
            endbit = MIN(zoneEnd, resEnd);
            startbit = GET_OFFSET(current_zone->start, startbit, BLOCK_SIZE); 
            endbit = GET_OFFSET(current_zone->start, endbit, BLOCK_SIZE);

            logf("kstart bit: %d\tkend bit: %d\n", startbit, endbit);

            set_bits(current_zone->bitmap, startbit, endbit);
        }

        logf("\n");
        current_zone = current_zone->next_zone;
    }

    printZones(memory_zones);
}

// todo: store the last allocated offset in each zone. maybe start searching from there?
void *pmm_alloc(){
    logf("trying to allocate 1 block\n");

    // find the first zone with free blocks
    current_zone = memory_zones;
    uint8_t found = 0;
    while (current_zone)
    {
        if(current_zone->free != 0){
            found = 1;
            break;
        }

        current_zone = current_zone->next_zone;
    }
    
    if(!found){
        logf("no free zones\n");
        return NULL;
    }

    uint32_t i = 0;
    while (current_zone->bitmap[i / WORD] == 0xFFFFFFFF && i < current_zone->length){
        i += WORD;
    }

    if(i >= current_zone->length){
        logf("no free words in zone\n");
        return NULL;
    }

    logf("searching in word %d\n", i);
    uint32_t end = i + 32;
    while (i < end)
    {
        if(test_bit(current_zone->bitmap, i) == 0){
            break;
        }
        i++;
    }
    
    // set block to reserved
    set_bit(current_zone->bitmap, i);
    current_zone->free--;

    return (void *) GET_ADDRESS(current_zone->start, i, BLOCK_SIZE);

    // go through bit map
    // 

    return NULL;
}

/*
    BITMAP FUNCTIONS

    @param - uint32_t *mapStart : pointer to the start of the bitmap
    @param - offset (or other variations of it) : offset from the start of the bitmap
        representing a sepcific bit. Starting from 0. All these functions assume that 
        the offsets are includive.
*/
void set_bit(uint32_t *mapStart, uint32_t offset)
{
    mapStart[offset / WORD] |= 1 << (offset % WORD);
}

void set_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd)
{
    uint32_t i;
    // set bit by bit until we reach the start of a word
    for (i = offsetStart; i <= offsetEnd && i % WORD != 0; i++)
        set_bit(mapStart, i);

    // set word by word, until a word would be too big for offsetEnd
    for (; i + WORD <= offsetEnd; i = i + WORD)
        mapStart[i / WORD] = 0xFFFFFFFF;

    // set bit by bit until the end
    for (; i <= offsetEnd; i++)
        set_bit(mapStart, i);
}

void unset_bit(uint32_t *mapStart, uint32_t offset)
{
    mapStart[offset / WORD] &= ~(1 << (offset % WORD));
}

void unset_bits(uint32_t *mapStart, uint32_t offsetStart, uint32_t offsetEnd)
{
    uint32_t i;
    // unset bit by bit until we reach the start of a word
    for (i = offsetStart; i <= offsetEnd && i % WORD != 0; i++)
        unset_bit(mapStart, i);

    // set word by word, until a word would be too big for offsetEnd
    for (; i + WORD <= offsetEnd; i = i + WORD)
        mapStart[i / WORD] = 0;

    // set bit by bit until the end
    for (; i <= offsetEnd; i++)
        unset_bit(mapStart, i);
}

bool test_bit(uint32_t *mapStart, uint32_t offset){
    return (mapStart[offset / WORD] & (1 << (offset % WORD)));
}


// functions for debugging
void printZone(struct pm_zone_t *zone)
{
    logf("Zone info @ %x: \n", zone);
    logf("zone start: %x\n", zone->start);
    logf("zone free: %d blocks\n", zone->free);
    logf("zone length: %d\n", zone->length);
    logf("zone bitmap words: %d words : %d bytes\n", zone->bitmapWords, zone->bitmapWords * 4);
    logf("bitmap location: %x\n", zone->bitmap);
    logf("bitmap contents : "); printBitmap(zone->bitmap, zone->bitmapWords);
    logf("\n");
    // todo: print bitmap
}

void printBitmap(uint32_t* mapstart, uint32_t mapWords){
    uint32_t i = 0;
    for(; i < mapWords; i++){
        logf(" %x ", *(mapstart + i));

        if(i >= 5){
            logf("...");
            break;
        }
    }
}

void printZones(struct pm_zone_t *zone){
    logf("\nPhysical Memory Manger Zones\n------------------------------------\n");

    while(zone){
        printZone(zone);
        logf("\n");
        zone = zone->next_zone;
    }

    logf("------------------------------------\n");
}