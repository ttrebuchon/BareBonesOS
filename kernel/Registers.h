#ifndef INCLUDED_REGISTERS_H
#define INCLUDED_REGISTERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} Registers_t;

#ifdef __cplusplus
}
#endif

#endif