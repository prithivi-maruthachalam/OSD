# function to load GDT created in C
.section .text
.global load_gdt
.type load_gdt, @function
load_gdt:
    movl 4(%esp), %eax
    lgdt (%eax)

    movw $0x10, %ax
    movw %ax, %es
    movw %ax, %ds
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss 

    mov $0x28, %eax
    ltr %ax

    # this sets the CS register to the kernel code descriptor in GDT
    jmp $0x08,$flush

flush:
    ret
