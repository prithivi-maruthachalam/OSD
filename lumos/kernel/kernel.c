#include <lumos/tty.h>
#include <lumos/gdt.h>
#include <lumos/idt.h>
#include <lumos/timer.h>
#include <lumos/keyboard.h>
#include <lumos/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define VIRTUAL_KERNEL_OFFSET 0xC0000000

struct mmap_entry_t
{
    uint32_t size;
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
};

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

void kernel_main(multiboot_info_t *mbt, uint32_t eax)
{
    uint32_t kernel_end = &_kernel_end;
    uint32_t kernel_start = &_kernel_start;

    terminal_init();
    gdt_init();
    printf("[kernel_main]: GDT initialized\n");
    idt_init();
    printf("[kernel_main]: IDT initialized\n");
    init_timer(50);
    printf("[kernel_main]: timer initialized\n");
    init_keyboard();
    printf("[kernel_main]: PS/2 keyboard driver initialized\n");

    printf("\nFound multiboot info at %x\n", mbt);
    struct mmap_entry_t *entry = (struct mmap_entry_t *)(mbt->mmap_addr + 0xC0000000);
    if (mbt->flags & 0x40)
    {
        printf("Found BIOS memory map at %x of size %d bytes\n", entry, mbt->mmap_length);
        while (entry < (struct mmap_entry_t *)(mbt->mmap_addr + mbt->mmap_length + VIRTUAL_KERNEL_OFFSET))
        {

            printf("base: %x\tlength:%x\ttype:%d\n", entry->base_low, entry->length_low, entry->type);
            entry = (struct mmap_entry_t *)((unsigned int)entry + entry->size + sizeof(entry->size));
        }
    }
    else
    {
        printf("GRUB has not given a BIOS memory map\n");
    }

    printf("Kernel starts at: %x\n", kernel_start);
    printf("Kernel ends at: %x\n", kernel_end);
    printf("Kernel size: %d bytes\n", kernel_end - kernel_start);

    for (;;)
        ;
}