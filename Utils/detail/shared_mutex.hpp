#ifndef INCLUDED_SHARED_MUTEX_HPP
#define INCLUDED_SHARED_MUTEX_HPP

#include "shared_mutex.hh"

namespace Utils
{
	template <class Clock, class Dur>
	bool shared_timed_mutex::try_lock_until(const chrono::time_point<Clock, Dur>& t)
	{
		unique_lock<mutex> lk(_mut);
		while (writers > 0)
		{
			cv_status status = wwait.wait_until(lk, t);
			if (status == cv_status::timeout)
			{
				return false;
			}
			__sync_synchronize();
		}
		++writers;
		
		while (readers > 0)
		{
			cv_status status = rwait.wait_until(lk, t);
			if (status == cv_status::timeout)
			{
				--writers;
				wwait.notify_all();
				return false;
			}
			__sync_synchronize();
		}
		
		
		return true;
	}
	
	
	
	
	template <class Clock, class Dur>
	bool shared_timed_mutex::try_lock_shared_until(const chrono::time_point<Clock, Dur>& t)
	{
		unique_lock<mutex> lk(_mut);
		cv_status status;
		while (writers > 0)
		{
			status = wwait.wait(lk, t);
			if (status == cv_status::timeout)
			{
				return false;
			}
			__sync_synchronize();
		}
		++waiting_readers;
		while (readers >= max_readers)
		{
			status = rwait.wait_until(lk, t);
			if (status == cv_status::timeout)
			{
				--waiting_readers;
				return false;
			}
			__sync_synchronize();
		}
		--waiting_readers;
		++readers;
		return true;
	}
}

#endif