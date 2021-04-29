/*
For crtend.o
*/

.section .init
    # contents of init section of crtend.o go here
    pop %ebp # pop the base pointer of the calling function back into ebp
    ret # return to the address of the calling function


.section .fini
    # contents of .fini section of crtend.o go here
    pop %ebp
    ret