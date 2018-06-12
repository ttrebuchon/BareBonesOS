#include "Timer.h"
#include "Interrupts.h"
#include "Registers.h"
#include <drivers/PortIO.h>
#include <drivers/VGA.hh>
#include <kernel/Debug.h>

#define DEFAULT_FREQ 1193180

extern "C" {

volatile uint32_t sleep_for = 0;
static volatile uint32_t current_freq = DEFAULT_FREQ;
volatile uint32_t ticks = 0;

void task_switch();
extern void __sleep();
void scheduler_hook();

#ifndef TESTING
static
#endif
void timer_callback(Registers_t regs)
{
	++ticks;
	sleep_for > 0 ? --sleep_for : 0;
	if (ticks % 100 == 0)
	{
		task_switch();
	}
	else if (ticks % 10 == 0)
	{
		scheduler_hook();
	}
	
}



void init_timer(uint32_t freq)
{
	register_interrupt_handler(IRQ0, &timer_callback);
	
	uint32_t div = DEFAULT_FREQ / freq;

	assert(div > 0);
	
	port_byte_out(0x43, 0x36);
	
	uint8_t high = (uint8_t)((div >> 8) & 0xFF);
	uint8_t low = (uint8_t)(div & 0xFF);
	
	port_byte_out(0x40, low);
	port_byte_out(0x40, high);
	current_freq = freq;
}

unsigned int sleep(unsigned int secs)
{
	uint32_t last;
	int cycles = 0;
	//cli();
	sleep_for = secs*current_freq;
	last = sleep_for;
	while (sleep_for != 0)
	{
		//sti();
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		#ifndef __aarch64__
		asm volatile ("PAUSE");
		asm volatile ("PAUSE");
		asm volatile ("PAUSE");
		asm volatile ("PAUSE");
		#endif
		//cli();
		// if (++cycles == 10000000)
		// {
		// 	cycles = 0;
		// 	assert(sleep_for < last);
		// 	last = sleep_for;
		// }
	}
	//sti();
	return 0;
}

int usleep(useconds_t microseconds)
{
	//cli();
	sleep_for = (microseconds*current_freq)/1000000;
	while (sleep_for != 0)
	{
		//sti();
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		#ifndef __aarch64__
		asm volatile ("PAUSE");
		#endif
		//cli();
	}
	//sti();
	return 0;
}

}