#ifndef INCLUDED_SHARED_MUTEX_HPP
#define INCLUDED_SHARED_MUTEX_HPP

#include "shared_mutex.hh"

namespace Utils
{
	template <class Clock, class Dur>
	bool shared_timed_mutex::try_lock_until(const chrono::time_point<Clock, Dur>& t)
	{
		unique_lock<mutex> lk(_mut);
		if (state & _write_entered)
		{
			while (true)
			{
				cv_status status = g1.wait_until(lk, t);
				if ((state & _write_entered) == 0)
				{
					break;
				}
				
				if (status == cv_status::timeout)
				{
					return false;
				}
				
			}
		}
		
		state |= _write_entered;
		if (state & _n_readers)
		{
			while (true)
			{
				cv_status status = g2.wait_until(lk, t);
				if ((state & _n_readers) == 0)
				{
					break;
				}
				
				if (status == cv_status::timeout)
				{
					state &= ~_write_entered;
					return false;
				}
			}
		}
		return true;
	}
	
	
	
	
	template <class Clock, class Dur>
	bool shared_timed_mutex::try_lock_shared_until(const chrono::time_point<Clock, Dur>& t)
	{
		unique_lock<mutex> lk(_mut);
		if ((state & _write_entered) || (state & _n_readers) == _n_readers)
		{
			while (true)
			{
				cv_status status = g1.wait_until(lk, t);
				if (((state & _write_entered) == 0) && (state & _n_readers) < _n_readers)
				{
					break;
				}
				
				if (status == cv_status::timeout)
				{
					return false;
				}
				
			}
		}
		
		unsigned numReaders = (state & _n_readers) + 1;
		state &= ~_n_readers;
		state |= numReaders;
		return true;
	}
}

#endif