#include "SpinMutex.hh"


namespace Support::threading
{
	spin_mutex::spin_mutex() : _lcount(0)
	{
		
	}
	
	
	void spin_mutex::lock()
	{
		int i = 0;
		while (!_lcount.compare_exchange_weak(i, 1))
		{
			i = 0;
			asm volatile ("NOP");
			asm volatile ("NOP");
		}
		
		//while (_lcount.exchange(1) == 1) ;
	}
	
	bool spin_mutex::try_lock() noexcept
	{
		int i = 0;
		return _lcount.compare_exchange_weak(i, 1);
	}
	
	void spin_mutex::unlock() noexcept
	{
		int i = 1;
		if (!_lcount.compare_exchange_strong(i, 0))
		{
			assert(i == 1);
			assert(false);
		}
	}
}