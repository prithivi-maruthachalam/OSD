ENTRY(_start)

SECTIONS
{
    . = 1M; /* Convention - to be used by BIOS and other stuff? */
  
    /* 4K for each section - ring a bell? - page sizes */
    .text BLOCK(4K) : ALIGN(4K) /* BLOCK and ALIGN are equivalent : backword compatibility : advantage ALIGN */
    {
        /* We put the mutiboot header first */
        *(.multiboot) 
        *(.text)
    }

    /* For read-only data */
    .rodata BLOCK(4K) : ALIGN(4K)
    {   
        *(.rodata)
    }

    /* Initialised Read-Write data */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* For unitialized Read-Write data */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(.COMMON) /* Basically unitialized GLOBAL variables. They only exist during the
        linking stage. Ultimately, they get initialised to 0. The linker then decides to move these to an 
        appropriate section (.bss or .data) */
        *(.bss) /* We set up the stack as unitialized Read-Write data */
    }
}
