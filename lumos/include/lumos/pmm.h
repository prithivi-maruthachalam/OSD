#ifndef PMM_H
#define PMM_H

#include <lumos/multiboot.h>
#include <stdint.h>

#define BLOCK_SIZE 4096   // 4 KB in bytes
#define WORD 32

// kernel location pointers - from the linker
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern uint32_t VIRTUAL_KERNEL_OFFSET_LD;

typedef uint32_t physical_addr_t;

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
    uint32_t end;
    uint32_t length;
    uint32_t free;
    uint32_t lastOffset;
    uint32_t bitmapWords;
    uint32_t *bitmap;
    struct pm_zone_t *next_zone;
} __attribute__((packed));

// API Functions
void init_pmm(multiboot_info_t *mbtStructure);  // initialize the physciall memory manager from the GRUB memory map
void *pmm_alloc();
void pmm_free(void *free);

#endif