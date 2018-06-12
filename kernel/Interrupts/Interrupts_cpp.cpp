#include "Interrupts.h"
#include <Utils/atomic>
#include <kernel/Utility.hh>

namespace Kernel { namespace Interrupts {
extern "C" {
	
	#ifndef MULTI_CORE
	static volatile /*Utils::atomic_int*/ int cli_count(0);
	#endif
	
	
	
	void cli()
	{
		__DO_CLI__;
		__sync_synchronize();
		#ifdef MULTI_CORE
		++current_processor->cli_count;
		#else
		++cli_count;
		#endif
		__sync_synchronize();
		__DO_CLI__;
	}
	
	void sti()
	{
		#ifdef MULTI_CORE
		auto& cli_count = current_processor->cli_count;
		#endif
		if (/*cli_count.fetch_sub(1)*/--cli_count <= 0)
		{
			cli_count = 0;
			__sync_synchronize();
			__DO_STI__;
		}
	}
	
	int block_interrupt_counter()
	{
		#ifdef MULTI_CORE
		return current_processor->cli_count;
		#else
		return cli_count;
		#endif
	}
}
}
}