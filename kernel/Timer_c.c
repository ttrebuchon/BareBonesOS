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

uint32_t sleep(uint32_t ms)
{
	cli();
	sleep_for = (ms*current_freq)/1000;
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