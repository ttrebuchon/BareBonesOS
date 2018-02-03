#include "IRQ.h"
#include "ISR.h"
#include <kernel/Registers.h>
#include <drivers/PortIO.h>
#include <drivers/VGA.hh>

isr_t interrupt_handlers[256];

void irq_handler(Registers_t regs)
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

void init_IRQ()
{
    for (int i = 0; i < 256; ++i)
    {
        interrupt_handlers[i] = 0x0;
    }

    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);
    port_byte_out(0x21, 0x20);
}