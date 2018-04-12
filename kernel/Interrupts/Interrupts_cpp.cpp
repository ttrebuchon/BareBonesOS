#include "Interrupts.h"
#include <Utils/atomic>
#include <kernel/Utility.hh>

namespace Kernel { namespace Interrupts {
extern "C" {
	
	static volatile Utils::atomic_int cli_count(0);
	
	volatile void cli() noexcept
	{
		++cli_count;
		__DO_CLI__;
	}
	
	volatile void sti() noexcept
	{
		if (cli_count.fetch_sub(1) == 0)
		{
			__DO_STI__;
		}
	}
	
	
}
}
}