#include <drivers/PortIO.h>

#ifndef IGNORE_PORT_IO
	#define _FAIL ASSERT(false)
#else
	#define _FAIL 
#endif



unsigned char port_byte_in(const unsigned short port)
{
	_FAIL;
    return 0;
}


void port_byte_out(const unsigned short port, const unsigned char data)
{
    _FAIL;
}


unsigned short port_word_in(const unsigned short port)
{
	_FAIL;
    return 0;
}


void port_word_out(const unsigned short port, const unsigned short data)
{
    _FAIL;
}

void port_long_out(const unsigned short port, const uint32_t data)
{
    _FAIL;
}

uint32_t port_long_in(const unsigned short port)
{
	_FAIL;
    return 0;
}

void insl(uint16_t port, void* addr, uint32_t cnt)
{
	_FAIL;
}