#ifndef PDE_H
#define PDE_H

#include <stdint.h>
#include <stdbool.h>

enum PDE_FLAGS {
	I86_PDE_PRESENT			=	1,		    //0000000000000000000000000000001
	I86_PDE_WRITABLE		=	2,		    //0000000000000000000000000000010
	I86_PDE_USER			=	4,		    //0000000000000000000000000000100
	I86_PDE_PWT			    =	8,		    //0000000000000000000000000001000
	I86_PDE_PCD			    =	0x10,		//0000000000000000000000000010000
	I86_PDE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PDE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PDE_4MB			    =	0x80,		//0000000000000000000000010000000
	I86_PDE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_PDE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
   	I86_PDE_FRAME			=	0x7FFFF000 	//1111111111111111111000000000000
};

// page directory entry data type
typedef uint32_t pde_t;

static inline void pde_set_attrib(pde_t* entry, uint32_t attribute){
	*entry |= attribute;
}

static inline void pde_unset_attrib(pde_t* entry, uint32_t attribute){
	*entry &= ~attribute;
}

static inline void pde_set_addr(pde_t* entry, uint32_t physical_addr){
    *entry = (*entry & ~I86_PTE_FRAME) | physical_addr << 12;
}

static inline bool pde_is_present (pde_t entry){
	return entry & I86_PDE_PRESENT;
}

static inline bool pde_is_user (pde_t entry){
	return entry & I86_PDE_USER;
}

static inline bool pde_is_4mb (pde_t entry){
	return entry & I86_PDE_4MB;
}

static inline bool pde_is_writable (pde_t entry){
	return entry & I86_PDE_WRITABLE;
}

static inline uint32_t pde_get_addr(pde_t entry){
    return (uint32_t) (entry & I86_PDE_FRAME) >> 12;
}


#endif