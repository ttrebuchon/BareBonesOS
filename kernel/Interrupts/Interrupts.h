#ifndef INCLUDED_INTERRUPTS_H
#define INCLUDED_INTERRUPTS_H


#include <kernel/c_cpp.h>

#ifdef __cplusplus
namespace Kernel { namespace Interrupts {
#endif

EXTERN_C void cli();
EXTERN_C void sti();





#ifdef __cplusplus
} }
#endif

#endif