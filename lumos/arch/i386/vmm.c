#include <lumos/vmm.h>
#include <stdio.h>
#include <stdbool.h>
#include <lumos/pte.h>
#include <lumos/pde.h>
#include <string.h>
#include <lumos/pmm.h>

page_dir_t *_cur_directory = NULL;

void init_vmm(){
    // allocate default page table for identity mapping
    // allocate default page table for higher half mapping
    logf("starting virtual memory manager\n");

    page_table_t* table_id = (page_table_t*) 0xC0126D35;
    if(!table_id){
        return;
    }    
    logf("putting first table at %x\n", table_id);
    
    memset((void *)table_id, 0, 4096);
}


// // internal functions
// bool vmm_alloc_page(pte_t *entry);
// void vmm_free_page(pte_t *entry);
// inline bool vmm_switch_page_dir(page_dir_t *dir);
// inline page_dir_t* vmm_get_page_dir();
// void vmm_flush_tlb_entry(virtual_addr_t addr);
// void vmm_map_page(void *phys_addr, void* virt_addr);

// void init_vmm(){
//     logf("\n\nstarting virtual memory manager\n");

//     // todo: disable pagin while we're here

//     // allocate the default page table
//     page_table_t* table = (page_table_t*) pmm_alloc();
//     if(!table){
//         return;
//     }

//     // allocate 3GB higher half page table
//     page_table_t* table2 = (page_table_t*) pmm_alloc();
//     if(!table2){
//         return;
//     }

//     // clear page table
//     memset(table, 0, sizeof(table));

//     // identity map first 4MB
//     for(int i = 0, frame = 0x0, virt=0x00000000; i < 1024 ;i++, frame += 4096, virt += 4096 ){
//         // create a new page
//         pte_t page = 0;
//         pte_set_attrib(&page, I86_PTE_PRESENT);
//         pte_set_frame_addr(&page, frame);

//         table2->entries[INDEX_PAGE_TABLE(virt)] = page;
//     }

//     // map > 1MB to higher half (currently here)
//     for(int i = 0, frame = 0x100000, virt=0xc0000000; i < 1024 ;i++, frame += 4096, virt += 4096 ){
//         // create a new page
//         pte_t page = 0;
//         pte_set_attrib(&page, I86_PTE_PRESENT);
//         pte_set_frame_addr(&page, frame);

//         table->entries[INDEX_PAGE_TABLE(virt)] = page;
//     }

//     // create default page dir
//     page_dir_t* dir = (page_dir_t *)pmm_alloc();
//     if(!dir){
//         return;
//     }

//     // clear directory and make it the current one
//     memset(dir, 0, sizeof(dir));

//     // add higher half table to dir
//     pde_t *entry = vmm_pdir_lookup_entry(dir, (virtual_addr_t)0xc0000000);
//     pde_set_attrib(entry, I86_PDE_PRESENT);
//     pde_set_attrib(entry, I86_PDE_WRITABLE);
//     pde_set_addr(entry, (physical_addr_t)table);

//     // add identity table to dir
//     pde_t *entry2 = vmm_pdir_lookup_entry(dir, (virtual_addr_t)0x00000000);
//     pde_set_attrib(entry2, I86_PDE_PRESENT);
  
// /* to allocate a pmm block and map it into an entry */
// bool vmm_alloc_page(pte_t *entry) {
//     // allocate a block from the physical memory manager
//     void *block = pmm_alloc();
//     if(!block){
//         return false;
//     }

//     // map it to the entry
//     pte_set_frame_addr(entry, (physical_addr_t)block);
//     pte_set_attrib(entry, I86_PTE_PRESENT);

//     return true;
// }

// /* free a block from the physical manager and remove it from its pte */
// void vmm_free_page(pte_t *entry){
//     void *block = (void *)pte_get_frame_addr(*entry);
//     if(block){
//         pmm_free(block);
//     }

//     pte_unset_attrib(entry, I86_PTE_PRESENT);
// }

// /* switch current page directory */
// inline bool vmm_switch_page_dir(page_dir_t* dir){
//     if(!dir){
//         return false;
//    }

//    _cur_directory = dir;
//    // todo: call functions to load pbdr
//    return true;
// }

// /* get current page directory */
// inline page_dir_t* vmm_get_page_dir(){
//     return _cur_directory;
// }

// /* flush tlb entry for a virtual address*/
// void vmm_flush_tlb_entry(virtual_addr_t addr){
//     asm volatile("cli");
//     asm volatile("invlpg %0" : "a"(addr));
//     asm volatile("sti");
// }

// /* map a virtual address to a virtual address*/
// void vmm_map_page(void *phys_addr, void* virt_addr){
//     // get current page dir
//     page_dir_t* page_dir = vmm_get_page_dir();

//     // get the page table corresponding to the virtual addr
//     pde_t *pd_entry = vmm_pdir_lookup_entry(page_dir, (virtual_addr_t)virt_addr);
    
//     // if the page table is not present, allocate it
  
// /* to allocate a pmm block and map it into an entry */
// bool vmm_alloc_page(pte_t *entry) {
//     // allocate a block from the physical memory manager
//     void *block = pmm_alloc();
//     if(!block){
//         return false;
//     }

//     // map it to the entry
//     pte_set_frame_addr(entry, (physical_addr_t)block);
//     pte_set_attrib(entry, I86_PTE_PRESENT);

//     return true;
// }

// /* free a block from the physical manager and remove it from its pte */
// void vmm_free_page(pte_t *entry){
//     void *block = (void *)pte_get_frame_addr(*entry);
//     if(block){
//         pmm_free(block);
//     }

//     pte_unset_attrib(entry, I86_PTE_PRESENT);
// }

// /* switch current page directory */
// inline bool vmm_switch_page_dir(page_dir_t* dir){
//     if(!dir){
//         return false;
//    }

//    _cur_directory = dir;
//    // todo: call functions to load pbdr
//    return true;
// }

// /* get current page directory */
// inline page_dir_t* vmm_get_page_dir(){
//     return _cur_directory;
// }

// /* flush tlb entry for a virtual address*/
// void vmm_flush_tlb_entry(virtual_addr_t addr){
//     asm volatile("cli");
//     asm volatile("invlpg %0" : "a"(addr));
//     asm volatile("sti");
// }

// /* map a virtual address to a virtual address*/
// void vmm_map_page(void *phys_addr, void* virt_addr){
//     // get current page dir
//     page_dir_t* page_dir = vmm_get_page_dir();

//     // get the page table corresponding to the virtual addr
//     pde_t *pd_entry = vmm_pdir_lookup_entry(page_dir, (virtual_addr_t)virt_addr);
    
//     // if the page table is not present, allocate it
//     if(!pde_is_present(*pd_entry)){
//         page_table_t* table = (page_table_t*) pmm_alloc();
//         if(!table){
//             return;
//         }

//         // clear table
//         memset(table, 0, sizeof(table));

//         // map in the table
//         pde_set_attrib(pd_entry, I86_PDE_PRESENT);
//         pde_set_attrib(pd_entry, I86_PDE_WRITABLE);
//         pde_set_addr(pd_entry, (physical_addr_t)table);
//     }

//     // get the table
//     page_table_t* table = (page_table_t *) GET_PHYSICAL_ADDR(pd_entry);

//     // get the page
//     pte_t *page = vmm_ptable_lookup_entry(table, virt_addr);

//     // map it in
//     pte_set_attrib(page, I86_PTE_PRESENT);
//     pte_set_frame_addr(page, (physical_addr_t)phys_addr);
// }

//   if(!pde_is_present(*pd_entry)){
//         page_table_t* table = (page_table_t*) pmm_alloc();
//         if(!table){
//             return;
//         }

//         // clear table
//         memset(table, 0, sizeof(table));

//         // map in the table
//         pde_set_attrib(pd_entry, I86_PDE_PRESENT);
//         pde_set_attrib(pd_entry, I86_PDE_WRITABLE);
//         pde_set_addr(pd_entry, (physical_addr_t)table);
//     }

//     // get the table
//     page_table_t* table = (page_table_t *) GET_PHYSICAL_ADDR(pd_entry);

//     // get the page
//     pte_t *page = vmm_ptable_lookup_entry(table, virt_addr);

//     // map it in
//     pte_set_attrib(page, I86_PTE_PRESENT);
//     pte_set_frame_addr(page, (physical_addr_t)phys_addr);
// }


//     // todo: enable paging using a function
// }

// /* to allocate a pmm block and map it into an entry */
// bool vmm_alloc_page(pte_t *entry) {
//     // allocate a block from the physical memory manager
//     void *block = pmm_alloc();
//     if(!block){
//         return false;
//     }

//     // map it to the entry
//     pte_set_frame_addr(entry, (physical_addr_t)block);
//     pte_set_attrib(entry, I86_PTE_PRESENT);

//     return true;
// }

// /* free a block from the physical manager and remove it from its pte */
// void vmm_free_page(pte_t *entry){
//     void *block = (void *)pte_get_frame_addr(*entry);
//     if(block){
//         pmm_free(block);
//     }

//     return _cur_directory;
// }

// /* flush tlb entry for a virtual address*/
// void vmm_flush_tlb_entry(virtual_addr_t addr){
//     asm volatile("cli");
//     asm volatile("invlpg %0" : "a"(addr));
//     asm volatile("sti");
// }

// /* map a virtual address to a virtual address*/
// void vmm_map_page(void *phys_addr, void* virt_addr){
//     // get current page dir
//     page_dir_t* page_dir = vmm_get_page_dir();

//     // get the page table corresponding to the virtual addr
//     pde_t *pd_entry = vmm_pdir_lookup_entry(page_dir, (virtual_addr_t)virt_addr);
    
//     // if the page table is not present, allocate it
//     if(!pde_is_present(*pd_entry)){
//         page_table_t* table = (page_table_t*) pmm_alloc();
//         if(!table){
//             return;
//         }

//         // clear table
//         memset(table, 0, sizeof(table));

//         // map in the table
//         pde_set_attrib(pd_entry, I86_PDE_PRESENT);
//         pde_set_attrib(pd_entry, I86_PDE_WRITABLE);
//         pde_set_addr(pd_entry, (physical_addr_t)table);
//     }

//     // get the table
//     page_table_t* table = (page_table_t *) GET_PHYSICAL_ADDR(pd_entry);

//     // get the page
//     pte_t *page = vmm_ptable_lookup_entry(table, virt_addr);

//     // map it in
//     pte_set_attrib(page, I86_PTE_PRESENT);
//     pte_set_frame_addr(page, (physical_addr_t)phys_addr);
// }

//     pte_unset_attrib(entry, I86_PTE_PRESENT);
// }

// /* switch current page directory */
// inline bool vmm_switch_page_dir(page_dir_t* dir){
//     if(!dir){
//         return false;
//    }

//    _cur_directory = dir;
//    // todo: call functions to load pbdr
//    return true;
// }

// /* get current page directory */
// inline page_dir_t* vmm_get_page_dir(){
//     return _cur_directory;
// }

// /* flush tlb entry for a virtual address*/
// void vmm_flush_tlb_entry(virtual_addr_t addr){
//     asm volatile("cli");
//     asm volatile("invlpg %0" : "a"(addr));
//     asm volatile("sti");
// }

// /* map a virtual address to a virtual address*/
// void vmm_map_page(void *phys_addr, void* virt_addr){
//     // get current page dir
//     page_dir_t* page_dir = vmm_get_page_dir();

//     // get the page table corresponding to the virtual addr
//     pde_t *pd_entry = vmm_pdir_lookup_entry(page_dir, (virtual_addr_t)virt_addr);
    
//     // if the page table is not present, allocate it
//     if(!pde_is_present(*pd_entry)){
//         page_table_t* table = (page_table_t*) pmm_alloc();
//         if(!table){
//             return;
//         }

//         // clear table
//         memset(table, 0, sizeof(table));

//         // map in the table
//         pde_set_attrib(pd_entry, I86_PDE_PRESENT);
//         pde_set_attrib(pd_entry, I86_PDE_WRITABLE);
//         pde_set_addr(pd_entry, (physical_addr_t)table);
//     }

//     // get the table
//     page_table_t* table = (page_table_t *) GET_PHYSICAL_ADDR(pd_entry);

//     // get the page
//     pte_t *page = vmm_ptable_lookup_entry(table, virt_addr);

//     // map it in
//     pte_set_attrib(page, I86_PTE_PRESENT);
//     pte_set_frame_addr(page, (physical_addr_t)phys_addr);
// }

