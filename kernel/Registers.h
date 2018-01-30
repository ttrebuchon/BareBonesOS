#ifndef INCLUDED_REGISTERS_HH
#define INCLUDED_REGISTERS_HH

#include <stdint.h>

#ifdef __cplusplus
namespace Kernel {
#endif

struct Registers_t
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

#ifdef __cplusplus
}
#endif

#endif