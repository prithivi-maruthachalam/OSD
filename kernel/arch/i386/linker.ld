ENTRY(_start)

SECTIONS
{
    . = 1M; /* Convention - to be used by BIOS and other stuff? */
    _kernel_start = .;

    /* 4K for each section - ring a bell? - page sizes */
    .multiboot ALIGN(4K) : AT(1M)
    {
        /* We put the mutiboot header first */
        *(.multiboot)
    }
    .text.initBoot ALIGN(4K) :
    {
        *(.text.initBoot) 
    }

    .text ALIGN(4K) : 
    {
        *(.text) 
    }

    .rodata ALIGN(4K) : 
    {   
        *(.rodata)
    }

    .data ALIGN(4K) :
    {
        *(.data)
    }

    .bss ALIGN(4K) : 
    {
        *(.COMMON) /* Basically unitialized GLOBAL variables. They only exist during the
        linking stage. Ultimately, they get initialised to 0. The linker then decides to move these to an 
        appropriate section (.bss or .data) */
        *(.bss) /* We set up the stack as unitialized Read-Write data */
    }

    _kernel_end = .;
}
