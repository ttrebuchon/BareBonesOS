#ifndef INCLUDED_PORT_IO_H
#define INCLUDED_PORT_IO_H

#ifdef __cplusplus
extern "C" {
#endif


unsigned char port_byte_in(const unsigned short port);

void port_byte_out(const unsigned short port, const unsigned char data);

unsigned short port_word_in(const unsigned short port);

void port_word_out(const unsigned short port, const unsigned short data);

#ifdef __cplusplus
}
#endif

#endif