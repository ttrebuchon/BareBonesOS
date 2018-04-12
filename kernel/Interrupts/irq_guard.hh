#ifndef INCLUDED_IRQ_GUARD_HH
#define INCLUDED_IRQ_GUARD_HH

namespace Kernel { namespace Interrupts
{
	class irq_guard
	{
		private:
		bool guarded;
		
		public:
		irq_guard() noexcept;
		irq_guard(const irq_guard&) = delete;
		irq_guard(irq_guard&&) noexcept;
		
		~irq_guard() noexcept;
	};
}

using Interrupts::irq_guard;
}

#endif