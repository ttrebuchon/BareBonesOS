#include "Timer.h"
#include "Interrupts.h"
#include "Registers.h"
#include <drivers/PortIO.h>
#include <drivers/VGA.hh>
#include <kernel/Debug.h>

#define DEFAULT_FREQ 1193180

volatile uint32_t sleep_for = 0;
static volatile uint32_t current_freq = DEFAULT_FREQ;
volatile uint32_t ticks = 0;

void task_switch();
extern void __sleep();

static void timer_callback(Registers_t regs)
{
	++ticks;
	sleep_for > 0 ? --sleep_for : 0;
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
	current_freq = freq;
}

unsigned int sleep(unsigned int secs)
{
	cli();
	sleep_for = secs*current_freq;
	while (sleep_for != 0)
	{
		sti();
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
		cli();
	}
	sti();
	return 0;
}

int usleep(useconds_t microseconds)
{
	cli();
	sleep_for = microseconds*current_freq/1000000;
	while (sleep_for != 0)
	{
		sti();
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
		cli();
	}
	sti();
	return 0;
}

time_t time(time_t* ptr)
{
	// TODO
	ASSERT(0);
}

