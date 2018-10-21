#include "PortIO.h"

#ifndef TESTING


unsigned char port_byte_in(const unsigned short port)
{
    unsigned char data;
    __asm__("inb %%dx, %%al " : "=a" (data) : "d" (port));
    return data;
}


void port_byte_out(const unsigned short port, const unsigned char data)
{
    __asm__("outb %%al, %%dx" : :"a" (data), "d" (port));
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

void insl(uint16_t port, void* addr, uint32_t cnt)
{
    asm volatile ("cld; rep insl" : "=D" (addr), "=c" (cnt) : "d" (port), "0" (addr), "1" (cnt) : "memory", "cc");
}

void insm(unsigned short port, void* addr, uint32_t bytes)
{
	asm volatile("rep insb" : "+D"(addr), "+c"(bytes) : "d"(port) : "memory");
}

#endif