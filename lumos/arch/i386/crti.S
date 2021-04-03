/*
For crtbegin.o - init the C library!!!
GCC provides a crtbegin.o as a black box. It handles calling functions that have to be called before 
the kernel main during runtime. 
All we can do is provide a place in memeory for gcc to put the init and fini sections of crtbegin
*/

# for the start section - called from boot.S (crt0.o)
.section .init
.global _init
.type _init, @function
_init:
    /*We're setting up a stack frame for this function without messing up
    the stack that we set up in the caller, _start
    Note: this stack frame is essentially a subset of the origin stack(16KB)
    */
    # ASSEMBLY FUNCTION PROLOGUE
    push %ebp # saves the previous base pointer in the stack
    /* This is GOOD PRACTICE for recursive calls. We want to use the ebp register 
    to store the base pointer of this function. So we'll save it in the stack to be 
    popped back into %ebp
    */
    movl %esp, %ebp 
    /* puts the value of esp into ebp - make ebp point to the base of this 
    function's stack frame
    */
    # crtbegin.o - .init section



# fini is almost pointless
.section .fini
.global _fini
.type _fini, @function
_fini:
    push %ebp
    movl %esp, %ebp
    # crtbegin.o - .fini section
