#include "Interrupts.h"
#include <Utils/atomic>
#include <kernel/Utility.hh>

namespace Kernel { namespace Interrupts {
extern "C" {
	
	static volatile /*Utils::atomic_int*/ int cli_count(0);
	
	void cli()
	{
		__DO_CLI__;
		__sync_synchronize();
		++cli_count;
		__sync_synchronize();
		__DO_CLI__;
	}
	
	void sti()
	{
		if (/*cli_count.fetch_sub(1)*/--cli_count <= 0)
		{
			cli_count = 0;
			__sync_synchronize();
			__DO_STI__;
		}
	}
	
	int block_interrupt_counter()
	{
		return cli_count;
	}
}
}
}