#include "shared_mutex.hh"
#include "shared_mutex.hpp"
#include "locks.hh"
#include "locks.hpp"

namespace Utils
{
	shared_timed_mutex::shared_timed_mutex() : _mut(), rwait(), wwait(), writers(0), readers(0)
	{
		
	}
	
	void shared_timed_mutex::lock()
	{
		unique_lock<mutex> lk(_mut);
		while (writers > 0)
		{
			wwait.wait(lk);
			__sync_synchronize();
		}
		
		++writers;
		while (readers > 0)
		{
			rwait.wait(lk);
			__sync_synchronize();
		}
	}
	
	bool shared_timed_mutex::try_lock()
	{
		unique_lock<mutex> lk(_mut, try_to_lock);
		if (!lk.owns_lock())
		{
			return false;
		}
		
		if (writers == 0 && readers == 0)
		{
			++writers;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	void shared_timed_mutex::unlock()
	{
		unique_lock<mutex> lk(_mut);
		assert(writers == 1 && readers == 0);
		--writers;
		wwait.notify_all();
	}
	
	void shared_timed_mutex::lock_shared()
	{
		unique_lock<mutex> lk(_mut);
		++waiting_readers;
		while (writers > 0)
		{
			wwait.wait(lk);
			__sync_synchronize();
		}
		
		while (readers >= max_readers)
		{
			rwait.wait(lk);
			__sync_synchronize();
		}
		++readers;
		--waiting_readers;
	}
	
	bool shared_timed_mutex::try_lock_shared()
	{
		unique_lock<mutex> lk(_mut, try_to_lock);
		if (lk.owns_lock())
		{
			if (writers == 0 && readers < max_readers)
			{
				++readers;
				return true;
			}
		}
		return false;
	}
	
	void shared_timed_mutex::unlock_shared()
	{
		unique_lock<mutex> lk(_mut);
		assert(readers > 0);
		--readers;
		if (waiting_readers > 0)
		{
			rwait.notify_all();
		}
		else
		{
			rwait.notify_one();
		}
	}
}