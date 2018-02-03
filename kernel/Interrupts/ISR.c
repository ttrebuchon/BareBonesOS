#include "ISR.h"
#include <kernel/Registers.h>
#include <drivers/VGA.hh>

extern isr_t interrupt_handlers[];

void isr_handler(Registers_t regs)
{
    if (interrupt_handlers[regs.int_no] != 0)
    {
    	isr_t handler = interrupt_handlers[regs.int_no];
    	handler(regs);
    }
}