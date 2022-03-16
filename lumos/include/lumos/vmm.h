#ifndef VMM_H
#define VMM_H

#include<stdint.h>

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

/* get pointer to page table entry from virtual address */
static inline pte_t* vmm_ptable_lookup_entry(page_table_t *table, virtual_addr_t addr){
    if(table){
        return &(table->entries[INDEX_PAGE_TABLE(addr)]);
    }
}

/* get pointer to page dir entry from virtual address */
static inline pde_t* vmm_pdir_lookup_entry(page_dir_t *dir, virtual_addr_t addr){
    if(dir){
        return &(dir->entries[INDEX_PAGE_DIR(addr)]);
    }
}

#endif