#ifndef INCLUDED_PORT_IO_H
#define INCLUDED_PORT_IO_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif


unsigned char port_byte_in(const unsigned short port);

void port_byte_out(const unsigned short port, const unsigned char data);

unsigned short port_word_in(const unsigned short port);

void port_word_out(const unsigned short port, const unsigned short data);

void port_long_out(const unsigned short port, const uint32_t data);
uint32_t port_long_in(const unsigned short port);

void insl(uint16_t port, void* addr, uint32_t cnt);

void insm(unsigned short port, void* addr, uint32_t bytes);

#ifdef __cplusplus
}
#endif

#endif