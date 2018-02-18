#ifndef INCLUDED_INTERRUPTS_H
#define INCLUDED_INTERRUPTS_H


#include <Common.h>
#include "ISR.h"
#include "IRQ.h"

#ifdef __cplusplus
namespace Kernel { namespace Interrupts {
extern "C" {
#endif

void cli();
void sti();
void register_interrupt_handler(uint8_t n, isr_t handler);



#ifdef __cplusplus
} } }
#endif

#endif