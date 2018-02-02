#include "IRQ.h"
#include "ISR.h"
#include <kernel/Registers.h>
#include <drivers/Port_IO.h>

isr_t interrupt_handlers[256];

void irq_handler(struct Registers_t regs)
{
    
    if (regs.int_no >= 40)
    {
    	port_byte_out(0xA0, 0x20);
    }
    
    port_byte_out(0x20, 0x20);
    
    if (interrupt_handlers[regs.int_no] != 0)
   {
       isr_t handler = interrupt_handlers[regs.int_no];
       handler(regs);
   }
}

void register_interrupt_handler(uint8_t n, isr_t handler)
{
	interrupt_handlers[n] = handler;
}