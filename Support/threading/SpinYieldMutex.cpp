#include "SpinYieldMutex.hh"
#include <kernel/Scheduling/API.h>

namespace Support::threading
{
	spin_yield_mutex::spin_yield_mutex() : _lcount(0)
	{
		
	}
	
	
	void spin_yield_mutex::lock()
	{
		int i = 0;
		while (!_lcount.compare_exchange_weak(i, 1))
		{
			i = 0;
			__sync_synchronize();
			sched_yield();
		}
	}
	
	bool spin_yield_mutex::try_lock() noexcept
	{
		int i = 0;
		return _lcount.compare_exchange_weak(i, 1);
	}
	
	void spin_yield_mutex::unlock() noexcept
	{
		int i = 1;
		if (!_lcount.compare_exchange_strong(i, 0))
		{
			assert(i == 1);
			assert(false);
		}
	}
}