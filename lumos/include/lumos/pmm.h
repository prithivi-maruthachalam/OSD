#ifndef PMM_H
#define PMM_H

#include <lumos/multiboot.h>
#include <stdint.h>

#define BLOCK_SIZE 4096   // 4 KB in bytes

// kernel location pointers - from the linker
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern uint32_t VIRTUAL_KERNEL_OFFSET_LD;

// GRUB Multiboot info
#define MBT_FLAG_IS_MMAP 0x40   // 6th bit of flags in the mbt
struct mmap_entry_t             // GRUB memory map entry structure
{
    uint32_t size;
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
};

// internal structures
struct pm_zone_t
{
    uint32_t start;
    uint32_t length;
    uint32_t free;
    uint32_t bitmapWords;
    uint32_t *bitmap;
    struct pm_zone_t *next_zone;
} __attribute__((packed));

// API Functions
void init_pmm(multiboot_info_t *mbtStructure);  // initialize the physciall memory manager from the GRUB memory map


#endif
// #include <lumos/multiboot.h>
// #include <stdint.h>

// // MISC
// #define BLOCK_SIZE 4096   // 4 KB in bytes
// #define MAX_BLOCK_ORDER 8 // ORDER * BLOCK_SIZE will be the maximum possible allocation

// // Macro to take an order and return the size of a block of that order in bytes
// #define ORDER_TO_SIZE_IN_BYTES(order) ((1 << order) * BLOCK_SIZE)

// // GRUB Multiboot info
// #define MBT_FLAG_IS_MMAP 0x40 // 6th bit of flags in the mbt

// // DMA constants
// #define DMA_MAX_ADDRESS 0xFFFFFF // Highest possible DMA address
// #define DMA_TOTAL_BYTES 0x40000  // 256KB in bytes
// #define DMA_TOTAL_BLOCKS 0x40    // 256 KB in blocks

// void init_pmm(multiboot_info_t *mbtStructure);
// void *pmm_alloc(uint32_t request);
// void pmm_free(uintptr_t address, uint32_t size);

// struct mmap_entry_t
// {
//     uint32_t size;
//     uint32_t base_low;
//     uint32_t base_high;
//     uint32_t length_low;
//     uint32_t length_high;
//     uint32_t type;
// };

// // Structure to create a linked list of zones
// struct zone
// {
//     uint8_t zoneType;
//     uint32_t freeBlocks;
//     uint32_t zonePhysicalSize;
//     struct pool *poolStart;
// } __attribute__((packed));

// struct pool
// {
//     uint32_t freeBlocks;
//     uint32_t start; // starting address of the memory associated with this pool
//     uint32_t poolPhysicalSize;
//     struct buddy *poolBuddiesTop;
//     struct buddy *poolBuddiesBottom;
//     struct pool *nextPool;
// } __attribute__((packed));

// struct buddy
// {
//     uint8_t buddyOrder;     // the order of the buddy in powers of 2
//     uint32_t mapWordCount;  // number of 32-bit words in the bitmap - for iteration
//     uint32_t freeBlocks;    // number of free blocks(paint)
//     uint32_t maxFreeBlocks; // max available allocations for this bitmap
//     uint32_t *bitMap;       // pointer to the bitmap for the current buddy
//     struct buddy *nextBuddy;
//     struct buddy *prevBuddy;
// } __attribute__((packed));

// extern uint32_t _kernel_start;
// extern uint32_t _kernel_end;
// extern uint32_t VIRTUAL_KERNEL_OFFSET_LD;

// #endif