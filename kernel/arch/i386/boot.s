# Multiboot Constants
.set ALIGN,	1
.set MEMINFO,	1<<1
.set FLAGS, 	ALIGN|MEMINFO
.set MAGIC,	0x1BADB002
.set CHECKSUM,	-(MAGIC+FLAGS)

# Multiboot Header
# Must be on a 4-byte boundary and be within the first 8KB
.section multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Stack Setup
# Must be 16-byte aligned and should be 16KB long
.section .bss
.align 16
.stack_bottom:
.skip 16384
.stack_top:

# Kernel Entry
.section .text
.global _start
.type _start, @function
_start:
	movl	$stack_top, %esp

	call	_init

	call 	kmain

	cli
	hlt
	jmp	$

.size _start, . - _start
