#include "irq_guard.hh"
#include "Interrupts.h"

namespace Kernel { namespace Interrupts
{
	
	
	irq_guard::irq_guard() noexcept : guarded(false)
	{
		cli();
		guarded = true;
	}
	
	irq_guard::irq_guard(irq_guard&& o) noexcept : guarded(o.guarded)
	{
		o.guarded = false;
	}
	
	irq_guard::~irq_guard() noexcept
	{
		if (guarded)
		{
			guarded = false;
			sti();
			
		}
	}
}
}