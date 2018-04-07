#ifndef INCLUDED_ISR_H
#define INCLUDED_ISR_H

#include <Common.h>
#include <kernel/Registers.h>

// #ifdef __cplusplus
// namespace Kernel { namespace Interrupts {
// #endif

typedef void(*isr_t)(Registers_t);
EXTERN_C void register_interrupt_handler(uint8_t, isr_t);

// #ifdef __cplusplus
// } }
// #endif

#endif