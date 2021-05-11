#ifndef PMM_H
#define PMM_H

#include <lumos/multiboot.h>
#include <stdint.h>

// MISC
#define BLOCK_SIZE 4096   // 4 KB in bytes
#define MAX_BLOCK_ORDER 8 // ORDER * BLOCK_SIZE will be the maximum possible allocation

// Macro to take an order and return the size of a block of that order in bytes
#define ORDER_TO_SIZE_IN_BYTES(order) ((1 << order) * BLOCK_SIZE)

// GRUB Multiboot info
#define MBT_FLAG_IS_MMAP 0x40 // 6th bit of flags in the mbt

// DMA constants
#define DMA_MAX_ADDRESS 0xFFFFFF // Highest possible DMA address
#define DMA_TOTAL_SIZE 0x40000   // 256 KB in bytes - total memory we're giving DMA

void init_pmm(multiboot_info_t *mbtStructure);

struct mmap_entry_t
{
    uint32_t size;
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
};

// Structure to create a linked list of zones
struct zone
{
    uint8_t zoneType;
    uint32_t totalSize;
    uint32_t freeMem;
    uint32_t zonePhysicalSize;
    struct pool *poolStart;
} __attribute__((packed));

struct pool
{
    uint32_t poolSize; // size of the memory associated with this pool
    uint32_t freeMem;
    uint32_t start; // starting address of the memory associated with this pool
    uint32_t poolPhysicalSize;
    struct buddy *poolBuddiesTop;
    struct pool *nextPool;
} __attribute__((packed));

struct buddy
{
    uint8_t buddyOrder;      // the order of the buddy in powers of 2
    uint32_t mapWordCount;   // number of 32-bit words in the bitmap - for iteration
    uint32_t freeBlocks;     // number of free blocks(paint)
    uint32_t maxFreeBlocks;  // max available allocations for this bitmap
    uint32_t *bitMap;        // pointer to the bitmap for the current buddy
    struct buddy *nextBuddy; // pointer to the next buddy

} __attribute__((packed));

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern uint32_t VIRTUAL_KERNEL_OFFSET_LD;

void *pmm_alloc(uint32_t size);
void pmm_free(uintptr_t address, uint32_t size);

#endif