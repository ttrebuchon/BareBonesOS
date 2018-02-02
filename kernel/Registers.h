#ifndef INCLUDED_REGISTERS_HH
#define INCLUDED_REGISTERS_HH

#include <stdint.h>

#ifdef __cplusplus
namespace Kernel {
#endif

typedef struct Registers
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