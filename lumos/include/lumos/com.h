#ifndef COMM_H
#define COMM_H

#define COM1 0x3f8
#define COM_DATA 0
#define COM_INTERRUPT_ENABLE 1
#define COM_BAUD_DIV_LSB 0
#define COM_BAUD_DIV_MSB 1
#define COM_FIFO_CTL 2
#define COM_INT_ID 2
#define COM_LINE_CTL 3
#define COM_DLAB 3
#define COM_MODEM_CTL 4
#define COM_LINE_STATUS 5
#define COM_MODEM_STATUS 6
#define COM_SCRATCH 7

void init_com1();
void com1_send(char ch);

#endif