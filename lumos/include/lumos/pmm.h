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
    uint32_t totalSize;
    uint32_t zonePhysicalSize;
    struct pool *poolStart;
    struct pool *poolEnd;
    struct zone *nextZone;
} __attribute__((packed));

struct pool
{
    uint32_t poolSize;
    uint32_t *start;
    uint32_t poolPhysicalSize;
    struct pool *nextPool;
} __attribute__((packed));

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern uint32_t VIRTUAL_KERNEL_OFFSET_LD;

#endif