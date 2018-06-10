#include "COM.h"
#include <drivers/PortIO.h>

extern "C"
{
    void COM_init(uint16_t port)
    {
        port_byte_out(port + COM_REG_INT, 0);
        port_byte_out(port + COM_REG_LINE_CONTROL, COM_DLAB_BIT);
        port_byte_out(port + COM_REG_DIV_LSB, COM_MY_DEFAULT_DIV_LOW);
        port_byte_out(port + COM_REG_DIV_MSB, COM_MY_DEFAULT_DIV_HIGH);
        port_byte_out(port + COM_REG_LINE_CONTROL, COM_LINE_PROTOCOL);
        port_byte_out(port + COM_REG_CONTROL, COM_ENABLE_FIFO);
        port_byte_out(port + COM_REG_MODEM_CONTROL, 0x0B);
    }


    int COM_is_transmit_empty(uint16_t port)
    {
        return (port_byte_in(port + COM_REG_LINE_STATUS) & COM_STATUS_TRANSMIT_EMPTY);
    }


    void COM_write(uint16_t port, char c)
    {
        while (!COM_is_transmit_empty(port)) ;

        port_byte_out(port + COM_REG_DATA, c);
    }

    void COM_write_string(uint16_t port, const char* str)
    {
        assert(str);

        for (const char* it = str; *it != 0; ++it)
        {
            COM_write(port, *it);
        }
    }

    void COM_write_line(uint16_t port, const char* str)
    {
        if (str)
        {
            COM_write_string(port, str);
        }
        

        COM_write(port, '\n');
    }
}