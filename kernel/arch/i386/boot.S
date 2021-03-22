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

.set VIRTUAL_KERNEL_OFFSET, 0xC0000000;
# the actual multiboot header
# aw and progbits are required to put it above the text section in the elf
# progbits inform that this is a section that contains initialised data and instructions
.section .multiboot, "ad", @progbits # we'll tell the linker where to put this
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


# allocate space for paging structure
.section .bss, "aw", @nobits
    .align 4096
boot_page_directory:
    .skip 4096
boot_page_table1:
    .skip 4096

# The actual kernel entry
.section .text.initBoot
.global _start # needs to be gloabal so that the linker can find it
.type _start, @function # tells the assembler that _start is a function
_start: 
    movl $(boot_page_table1 - VIRTUAL_KERNEL_OFFSET), %edi    # page table entry address
    movl $0, %esi   # physical memeory
    movl $1023, %ecx # loop instruction handeles this by itself
    
1:
    cmpl $_kernel_start, %esi
    jl 2f
    cmpl $(_kernel_end - VIRTUAL_KERNEL_OFFSET), %esi 
    jge 3f  # finished allocating bytes for kernel, let's leave

    # esi is greater than or equal to kernel start
    # allocate pages
    movl %esi, %edx
    orl $0x003, %edx
    movl %edx, (%edi) 

2:
    addl $4096, %esi    # each page is 4KB
    addl $4, %edi         # each entry is 4 bytes
    loop 1b    # this also decerments ecx

3: # map video memory
    movl $(0x000B8000 | 0x003), boot_page_table1 - VIRTUAL_KERNEL_OFFSET + (1023 * 4)

    movl $(boot_page_table1 + 0x003 - VIRTUAL_KERNEL_OFFSET), boot_page_directory - VIRTUAL_KERNEL_OFFSET
    movl $(boot_page_table1 + 0x003 - VIRTUAL_KERNEL_OFFSET), boot_page_directory - VIRTUAL_KERNEL_OFFSET + (768 * 4)

    # set the control registers
    # set cr3 to address of boot_page_directory
    
    movl $(boot_page_directory - VIRTUAL_KERNEL_OFFSET), %ecx
    movl %ecx, %cr3

    # enable paging in cr0
    movl %cr0, %ecx
    orl $0x80000001, %ecx
    movl %ecx, %cr0 
    

    lea 4f, %ecx
    jmp *%ecx;


.section .text
4:
    movl $stack_top, %esp # move the address of stack top into the stack pointer | stack grows downward so forget about stack_bottom
    call _init # init comes first in order to set up crtbegin.o - perform initialisations for C libraries
    # NOTE: call pushes the return address to the stack. Don't worry about that
    call kernel_main # guess again, stupid

    # it shouldn't get here at all
    cli # certain interrupts will start it up again, so we'll disable those
1:  hlt
    jmp 1b # loop back
