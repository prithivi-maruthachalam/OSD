.section .text
.global load_idt
.type load_idt, @function
load_idt:
    movl 4(%esp), %eax
    lidt (%eax)
    sti
    int $49
    ret


// # for ISRs
// .extern isr_handler

.global isr49
isr49:
    movl $0x43, 0xC03FF000
    hlt
    iret

// .macro ISR_NOERRCODE num
//     .global isr\num
//     isr\num :
//         push 0
//         push \num
//         jmp isr_stub
// .endm

// .macro ISR_ERRCODE num
//     .global isr\num
//     isr\num :
//         push \num
//         jmp isr_stub
// .endm

// ISR_NOERRCODE 0
// ISR_NOERRCODE 1
// ISR_NOERRCODE 2
// ISR_NOERRCODE 3
// ISR_NOERRCODE 4
// ISR_NOERRCODE 5
// ISR_NOERRCODE 6
// ISR_NOERRCODE 7
// ISR_ERRCODE 8
// ISR_NOERRCODE 9
// ISR_ERRCODE 10
// ISR_ERRCODE 11
// ISR_ERRCODE 12
// ISR_ERRCODE 13
// ISR_ERRCODE 14
// ISR_NOERRCODE 15
// ISR_NOERRCODE 16
// ISR_ERRCODE 17
// ISR_NOERRCODE 18
// ISR_NOERRCODE 19
// ISR_NOERRCODE 20
// ISR_NOERRCODE 21
// ISR_NOERRCODE 22
// ISR_NOERRCODE 23
// ISR_NOERRCODE 24
// ISR_NOERRCODE 25
// ISR_NOERRCODE 26
// ISR_NOERRCODE 27
// ISR_NOERRCODE 28
// ISR_NOERRCODE 29
// ISR_NOERRCODE 30
// ISR_NOERRCODE 31

// .type isr_stub, @function
// isr_stub:
//     # stack EFLAGS -> CS -> EIP -> ERR CODE / 0 -> INT NUM
//     pushal
//     call isr_handler
//     popal
//     add %esp, 8 # for the 2 value that we pushed
//     iret



// # for IRQs
// .extern irq_handler
// .macro IRQ_NOERRCODE num,i_num
//     .global irq\num
//     irq\num :
//         push 0
//         push \i_num
//         jmp irq_stub 
// .endm

// IRQ_NOERRCODE 0, 32
// IRQ_NOERRCODE 1, 33
// IRQ_NOERRCODE 2, 34
// IRQ_NOERRCODE 3,35
// IRQ_NOERRCODE 4,36
// IRQ_NOERRCODE 5,37
// IRQ_NOERRCODE 6,38
// IRQ_NOERRCODE 7,39
// IRQ_NOERRCODE 8,40
// IRQ_NOERRCODE 9,41
// IRQ_NOERRCODE 10,42
// IRQ_NOERRCODE 11,43
// IRQ_NOERRCODE 12,44
// IRQ_NOERRCODE 13,45
// IRQ_NOERRCODE 14,46
// IRQ_NOERRCODE 15,47

// .type irq_stub, @function
// irq_stub:
//     # stack EFLAGS -> CS -> EIP -> ERR CODE / 0 -> INT NUM
//     pushal
//     call irq_handler
//     popal
//     add %esp, 8 # for the 2 value that we pushed
//     iret



