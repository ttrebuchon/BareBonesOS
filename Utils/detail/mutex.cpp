#include "mutex.hh"

namespace Utils
{
	mutex::mutex() : _lcount(0)
	{
		
	}
	
	
	void mutex::lock()
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
	
	bool mutex::try_lock()
	{
		int i = 0;
		return _lcount.compare_exchange_weak(i, 1);
	}
	
	void mutex::unlock()
	{
		_lcount.exchange(0);
	}
}