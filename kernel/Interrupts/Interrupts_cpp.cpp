#include "Interrupts.h"
#include <Utils/atomic>
#include <kernel/Utility.hh>

namespace Kernel { namespace Interrupts {
extern "C" {
	
	static volatile Utils::atomic_int cli_count(0);
	
	void cli()
	{
		++cli_count;
		__DO_CLI__;
	}
	
	void sti()
	{
		if (cli_count.fetch_sub(1) == 0)
		{
			__DO_STI__;
		}
	}
	
	
}
}
}