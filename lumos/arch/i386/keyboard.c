#include <lumos/keyboard.h>
#include <lumos/ports.h>
#include <lumos/idt.h>
#include <lumos/isr.h>
#include <lumos/timer.h>
#include <stdint.h>
#include <stdio.h>

uint8_t scancode;
uint8_t lastScancode = 0;

struct status
{
    uint8_t control;
    uint8_t shift;
    uint8_t capsLock;
    uint8_t numLock;
    uint8_t scrollLock;
    uint8_t alt;
} status2;

// Status bit structure
/*
0 - control
1 - shift
2 - capsLock
3 - numLock
4 - scrollLock
5 - alt
*/
uint8_t status = 0;

unsigned char kbdus[128] =
    {
        0, 0, // Esc key
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
        0,                                                                      /* 29   - Control */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
        '\'', '`', 0, /* Left shift */
        '\\', 'z', 'x', 'c', 'v', 'b', 'n',
        'm', ',', '.', '/', 0, /* Right shift */
        '*',
        0,                            /* Alt */
        ' ',                          /* Space bar */
        0,                            /* Caps lock */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* <F1 ... F10 */
        0,                            /* 69 - Num lock*/
        0,                            /* Scroll Lock */
        0,                            /* Home key */
        0,                            /* Up Arrow */
        0,                            /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0, /* All other keys are undefined */
};

unsigned char kbdus_shift[128] =
    {
        0, 0, // Esc key
        '!', '@', '#', '$', '%', '%', '^', '*', '(', ')', '_', '+', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
        0,                                                                      /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
        '\"', '~', 0, /* Left shift */
        '|', 'Z', 'X', 'C', 'V', 'B', 'N',
        'M', '<', '>', '?', 0, /* Right shift */
        '*',
        0,                            /* Alt */
        ' ',                          /* Space bar */
        0,                            /* Caps lock */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* <F1 ... F10 */
        0,                            /* 69 - Num lock*/
        0,                            /* Scroll Lock */
        0,                            /* Home key */
        0,                            /* Up Arrow */
        0,                            /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0, /* All other keys are undefined */
};

// void keyboard_handler(struct registers_state regs);

void keyboard_handler(struct registers_state regs)
{
    scancode = inb(KBD_CTL_DATA_PORT);
    uint8_t temp = ((status & SHIFT) == 0) ? kbdus[scancode] : kbdus_shift[scancode];

    if (((scancode & 0x80) == 0) && temp)
    {
        if (((status & CAPS_LOCK) != 0) && ((65 <= temp && temp <= 90) || (97 <= temp && temp <= 122)))
        {
            if ((status & SHIFT) == 0)
            {
                printf("%c", kbdus_shift[scancode]);
            }
            else
            {
                printf("%c", kbdus[scancode]);
            }
        }
        else
        {
            printf("%c", temp);
        }
    }
    else
    {
        switch (scancode)
        {
        // Control
        case 0x1D:
            status |= CTRL;
            break;
        case 0x9D:
            status &= ~CTRL;
            break;

        // Shift
        case 0x2A:
            status |= SHIFT;
            break;
        case 0xAA:
            status &= ~SHIFT;
            break;
        case 0x36:
            status |= SHIFT;
            break;
        case 0xB6:
            status &= ~SHIFT;
            break;

        // Alt
        case 0x38:
            status |= ALT;
            break;
        case 0xB8:
            status &= ~ALT;
            break;

        // Caps lock
        case 0x3A:
            status ^= CAPS_LOCK;
            break;

        // numlock
        case 0x45:
            status ^= NUM_LOCK;
            break;

        // Scroll lock
        case 0x46:
            status ^= SCROLL_LOCK;
            break;

        case 0x3B:
            // printf("F1 pressed\n");
            break;

        case 0x3C:
            // printf("F2 pressed\n");
            break;

        case 0x3D:
            // printf("F3 pressed\n");
            break;

        case 0x3E:
            // printf("F4 pressed\n");
            break;

        case 0x3F:
            // printf("F5 pressed\n");
            break;

        case 0x40:
            // printf("F6 pressed\n");
            break;
        case 0x41:
            // printf("F7 pressed\n");
            break;
        case 0x42:
            // printf("F8 pressed\n");
            break;
        case 0x43:
            // printf("F9 pressed\n");
            break;
        case 0x44:
            // printf("F10 pressed\n");
            break;
        case 0x57:
            // printf("F11 pressed\n");
            break;
        case 0x58:
            // printf("F12 pressed\n");
            break;
        default:
            break;
        }
    }
    lastScancode = scancode;
    UNUSED(regs);
}

void init_keyboard()
{
    register_interrupt_handler(33, (uint32_t)keyboard_handler);

    // reading any scancodes left from boot
    while ((inb(KBD_CTL_COMMAND_PORT) & OP_BUF_STATUS) == 1)
    {
        inb(KBD_CTL_DATA_PORT);
    }

    status = 0;

    outb(PIC_MASTER_DATA_PORT, 0xFC);
    outb(PIC_SLAVE_DATA_PORT, 0xFF);
}