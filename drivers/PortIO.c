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

void port_long_out(const unsigned short port, const uint32_t data)
{
    __asm__("outl %%ax, %%dx" : :"a" (data), "d" (port));
    //asm volatile ("outl %%ax, %%dx" : : "r"(port), "r"(data));
}

uint32_t port_long_in(const unsigned short port)
{
    uint32_t data;
    //asm volatile ("inl %%dx, %%ax" : "=r"(data) : "r"(port));
    __asm__("inl %%dx, %%ax" : "=a" (data) : "d" (port));
    return data;
}
