#include "PortIO.h"


#ifdef __cplusplus
extern "C"
#endif
unsigned char port_byte_in(const unsigned short port)
{
    unsigned char data;
    __asm__("in %%dx, %%al " : "=a" (data) : "d" (port));
    return data;
}

#ifdef __cplusplus
extern "C"
#endif
void port_byte_out(const unsigned short port, const unsigned char data)
{
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}

#ifdef __cplusplus
extern "C"
#endif
unsigned short port_word_in(const unsigned short port)
{
    unsigned short data;
    __asm__("in %%dx, %%ax" : "=a" (data) : "d" (port));
    return data;
}

#ifdef __cplusplus
extern "C"
#endif
void port_word_out(const unsigned short port, const unsigned short data)
{
    __asm__("out %%ax, %%dx" : :"a" (data), "d" (port));
}

