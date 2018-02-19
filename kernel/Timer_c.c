#include "Timer.h"
#include "Interrupts.h"
#include "Registers.h"
#include <drivers/PortIO.h>
#include <drivers/VGA.hh>

#define DEFAULT_FREQ 1193180

void task_switch();

uint32_t ticks = 0;

static void timer_callback(Registers_t regs)
{
	++ticks;
	task_switch();
}

void init_timer(uint32_t freq)
{
	register_interrupt_handler(IRQ0, &timer_callback);
	
	uint32_t div = DEFAULT_FREQ / freq;
	
	port_byte_out(0x43, 0x36);
	
	uint8_t high = (uint8_t)((div >> 8) & 0xFF);
	uint8_t low = (uint8_t)(div & 0xFF);
	
	port_byte_out(0x40, low);
	port_byte_out(0x40, high);
}