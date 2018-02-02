#include "PortIO.h"



unsigned char port_byte_in(const unsigned short port)
{
    unsigned char data;
    __asm__("in %%dx, %%al " : "=a" (data) : "d" (port));
    return data;
}


void port_byte_out(const unsigned short port, const unsigned char data)
{
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}


unsigned short port_word_in(const unsigned short port)
{
    unsigned short data;
    __asm__("in %%dx, %%ax" : "=a" (data) : "d" (port));
    return data;
}


void port_word_out(const unsigned short port, const unsigned short data)
{
    __asm__("out %%ax, %%dx" : :"a" (data), "d" (port));
}
