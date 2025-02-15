#ifndef VMM_H
#define VMM_H

#include<stdint.h>
#include "pte.h"
#include "pde.h"

#define INDEX_PAGE_DIR(x) (((x) >> 22) & 0x3FF)
#define INDEX_PAGE_TABLE(x) (((x) >> 12) & 0x3FF)
#define GET_PHYSICAL_ADDR(x) (*x & ~0xFFF)

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIR 1024

#define PAGE_SIZE 4096  // 4KB pages

typedef uint32_t virtual_addr_t;

// page table struct
typedef struct page_table {
    pte_t entries[PAGES_PER_TABLE];
} page_table_t;

// page dir struct
typedef struct page_dir {
    pde_t entries[TABLES_PER_DIR];
} page_dir_t;

/* API FUNCTIONS */
void init_vmm();

static inline uint32_t read_cr3(){
    uint32_t value;
    asm volatile("mov rax, cr3" : "=a"(value));
    return value;
}

static inline void write_cr3(uint32_t value){
    asm volatile("mov cr3, rax" : : "a"(value));
}

#endif