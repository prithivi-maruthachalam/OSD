#include <lumos/com.h>
#include <lumos/ports.h>
#include <lumos/isr.h>

void init_com1()
{
    outb(COM1 + COM_INTERRUPT_ENABLE, 0x00); // disable interrupts
    outb(COM1 + COM_DLAB, 0x80);             // set DLAB to - access baudrate registers
    outb(COM1 + COM_BAUD_DIV_LSB, 0x03);     // (lsb) 115200 / 3 = 38400
    outb(COM1 + COM_BAUD_DIV_MSB, 0x00);     // (msb)
    outb(COM1 + COM_LINE_CTL, 0x03);         // 8 bits, 1 stop bit, no parity also unset DLAB
    outb(COM1 + COM_FIFO_CTL, 0xC7);         // enable fifo, clear and then trigger at 14 bytes
    outb(COM1 + COM_MODEM_CTL, 0x0B);        // out2 | DTR and RTS
    outb(COM1 + COM_MODEM_CTL, 0x1E);        // loopback mode, both outputs, RTS

    // LoopBack test
    outb(COM1 + COM_DATA, 0x55);
    if (inb(COM1 + COM_DATA) == 0x55)
    {
        outb(COM1 + COM_MODEM_CTL, 0x0F); // IRQs enabled, out1, out2 enabled
    }
}

void com1_send(char ch)
{
    while ((inb(COM1 + COM_LINE_STATUS) & 0x20) == 0)
        ;
    outb(COM1 + COM_DATA, ch);
}