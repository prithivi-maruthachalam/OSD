#include <lumos/keyboard.h>
#include <lumos/ports.h>
#include <lumos/idt.h>
#include <lumos/isr.h>
#include <stdint.h>
#include <stdio.h>

uint8_t scancode;
uint8_t keys_status = 0;

void keyboard_handler(struct registers_state regs)
{
    uint8_t scancode = inb(KBD_CTL_DATA_PORT);
    printf("%x\n", scancode);
    UNUSED(regs);
}

uint8_t kbrd_ctl_read_status()
{
    return inb(KBD_CTL_STATUS_PORT);
}

void kbrd_ctl_send_command(uint8_t commandByte)
{
    while (kbrd_ctl_read_status() & IP_BUF_STATUS)
        ;

    outb(KBD_CTL_COMMAND_PORT, commandByte);
}

void kbrd_enc_send_command(uint8_t commandByte)
{
    while (kbrd_ctl_read_status() & IP_BUF_STATUS)
        ;

    outb(KBD_ENC_COMMAND_PORT, commandByte);
}

void init_keyboard()
{
    outb(PIC_MASTER_DATA_PORT, 0x00);
    outb(PIC_SLAVE_DATA_PORT, 0x00);
    asm volatile("cli");

    keys_status = 0;
    // test current status of keyboard to make sure it is up

    inb(KBD_CTL_DATA_PORT); // Flush data register
    register_interrupt_handler(33, (uint32_t)keyboard_handler);

    asm volatile("sti");
}