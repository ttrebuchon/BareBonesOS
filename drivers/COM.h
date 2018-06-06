#ifndef INCLUDED_DRIVERS_COM_H
#define INCLUDED_DRIVERS_COM_H

#include <Common.h>

#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8
#define COM3_PORT 0x3E8
#define COM4_PORT 0x2E8

#define COM_REG_DATA            0x0
#define COM_REG_INT             0x1     // 1 = Interrupts Enabled
#define COM_REG_DIV_LSB         0x0     // When DLAB is set to 1
#define COM_REG_DIV_MSB         0x1     // When DLAB is set to 1
#define COM_REG_CONTROL         0x2
#define COM_REG_LINE_CONTROL    0x3     // MSB of this reg is DLAB
#define COM_REG_MODEM_CONTROL   0x4
#define COM_REG_LINE_STATUS     0x5
#define COM_REG_MODEM_STATUS    0x6
#define COM_REG_SCRATCH         0x7

#define COM_STATUS_TRANSMIT_EMPTY   0x20
#define COM_STATUS_RECEIVE_AVAIL    0x01

#define COM_LINE_PROTOCOL       0x03
#define COM_DLAB_BIT            0x80
#define COM_MY_DEFAULT_DIV_LOW  0x03
#define COM_MY_DEFAULT_DIV_HIGH 0x00
#define COM_ENABLE_FIFO         0xC7


C_CODE


// typedef struct
// {

// }
// __attribute__((__packed__))
// COM_Channel_t;



// #ifdef __cplusplus
//     static_assert(sizeof(COM_Channel_t) == 8);
// #endif


int COM_is_transmit_empty(uint16_t port);
void COM_write(uint16_t port, char c);
void COM_init(uint16_t port);

C_END

#endif