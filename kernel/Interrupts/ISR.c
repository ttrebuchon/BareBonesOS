#include "ISR.h"
#include <kernel/Registers.h>
#include <drivers/VGA.hh>

extern isr_t* interrupt_handlers;

void isr_handler(struct Registers_t regs)
{
    //TODO
    // c_vga_write("Interrupt caught!\n");
    // c_vga_write("Interrupt caught!\n");
    // c_vga_write("Interrupt caught!\n");
    // c_vga_write("Interrupt caught!\n");
    // c_vga_write("Interrupt caught!\n");
    // c_vga_write("Interrupt caught!\n");
    
    if (interrupt_handlers[regs.int_no] != 0)
    {
    	isr_t handler = interrupt_handlers[regs.int_no];
    	handler(regs);
    }
}