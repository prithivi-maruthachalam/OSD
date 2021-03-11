/* 
Sets up the multiboot header for GRUB2 to interface with
Sets up the stack
Calls the init function in crti
Calls the kernel main
*/

# constants for the multiboot header
.set ALIGN, 1 << 0   # align things on page boundaries
.set MEMINFO, 1 << 1 # need memory map
.set MAGIC, 0x1BADB002
.set FLAGS, ALIGN | MEMINFO
.set CHECKSUM, -(MAGIC + FLAGS)

# the actual multiboot header
.section .multiboot # we'll tell the linker where to put this
.align 4 # 4 byte aligned for 32-bit arch
.long MAGIC
.long FLAGS
.long CHECKSUM



# C needs a stack
.section .bss # section for unitialized data
.align 16 # 16 byte alignment - gives 1024 stack elements when 16KB is allocated
stack_bottom: # label for stack bottom
.skip 16384 # 16KB for the stack
stack_top: # guess, stupid

# The actual kernel entry
.section .text
.global _start # needs to be gloabal so that the linker can find it
.type _start, @function # tells the assembler that _start is a function
.start: 
    /*
        NOTE: We don't need to keep track of the stack base(%ebp)
        It's only a needed in functions called from here, cause things can get lost when there are
        lot of thing to be stored(local variables and so on)
    */
    movl $stack_top, %esp # move the address of stack top into the stack pointer | stack grows downward so forget about stack_bottom
    call _init # init comes first in order to set up crtbegin.o - perform initialisations for C libraries
    ; NOTE: call pushes the return address to the stack. Don't worry about that
    call kernel_main # guess again, stupid

    # it shouldn't get here at all
    cli # certain interrupts will start it up again, so we'll disable those
1:  hlt
    jmp 1b # loop back
