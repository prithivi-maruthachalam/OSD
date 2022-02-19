#ifndef PTE_H
#define PTE_H

#include <stdint.h>
#include <stdbool.h>

enum PTE_FLAGS {
	I86_PTE_PRESENT			=	1,		    //0000000000000000000000000000001
	I86_PTE_WRITABLE		=	2,		    //0000000000000000000000000000010
	I86_PTE_USER			=	4,		    //0000000000000000000000000000100
	I86_PTE_WRITETHOUGH		=	8,		    //0000000000000000000000000001000
	I86_PTE_NOT_CACHEABLE	=	0x10,		//0000000000000000000000000010000
	I86_PTE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PTE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PTE_PAT			    =	0x80,		//0000000000000000000000010000000
	I86_PTE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_PTE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
   	I86_PTE_FRAME			=	0x7FFFF000 	//1111111111111111111000000000000
};

// page table entry data type
typedef uint32_t pte_t;

static inline void pte_set_attrib(pte_t* entry, uint32_t attribute){
    *entry |= attribute;
}

static inline void pte_unset_attrib(pte_t* entry, uint32_t attribute){
    *entry &= ~attribute;
}

static inline void pte_set_frame_addr(pte_t* entry, uint32_t physical_addr){
    *entry = (*entry & ~I86_PTE_FRAME) | physical_addr << 12;
}

static inline bool pte_is_present(pte_t entry){
    return entry & I86_PTE_PRESENT;
}

static inline bool pte_is_writable(pte_t entry){
    return entry & I86_PTE_WRITABLE;
}

static inline uint32_t pte_get_frame_addr(pte_t entry){
    return (uint32_t) (entry & I86_PTE_FRAME) >> 12;
}

#endif