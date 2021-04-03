#ifndef PORTS_H
#define PORTS_H

// PIC ports
#define PIC_MASTER_COMMAND_PORT 0x20
#define PIC_MASTER_DATA_PORT (PIC_MASTER_COMMAND_PORT) + 1
#define PIC_SLAVE_COMMAND_PORT 0xA0
#define PIC_SLAVE_DATA_PORT (PIC_SLAVE_COMMAND_PORT) + 1
#define PIC_EOI 0x20

// KEYBOARD ports
#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60

#include <stdint.h>

static inline uint8_t inb(uint16_t portAddr)
{
    uint8_t ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(portAddr));
    return ret;
}

static inline void outb(uint16_t portAddr, uint8_t data)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(data), "Nd"(portAddr));
}

#endif