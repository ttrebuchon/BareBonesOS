#ifndef INCLUDED_CONDITION_VARIABLE_HPP
#define INCLUDED_CONDITION_VARIABLE_HPP

#include "condition_variable.hh"
#include <Support/threading/LowLock.hh>
#include <kernel/Scheduling/thread.h>
#include <kernel/Processor.h>
#include <Utils/detail/locks.hh>
#include <Utils/mutex>



namespace Utils
{
	
	template <class M>
	generic_condition_variable<M>::generic_condition_variable() : __lock(0), __futex(0), __total_seq(0), __wakeup_seq(0), __woken_seq(0), __mutex(nullptr), __nwaiters(0), __broadcast_seq(0)
	{
		
	}
	
	
	template <class M>
	generic_condition_variable<M>::~generic_condition_variable()
	{
		Support::threading::low_lock(__lock);
		return;
	}
	
	
	
	template <class M>
	void generic_condition_variable<M>::notify_one() throw()
	{
		using namespace Support::threading;
		
		low_lock(__lock);
		
		if (__total_seq > __wakeup_seq)
		{
			++__wakeup_seq;
			++__futex;
			
			// TODO: Signal scheduler
			
		}
		
		low_unlock(__lock);
	}
	
	template <class M>
	void generic_condition_variable<M>::notify_all() throw()
	{
		using namespace Support::threading;
		
		low_lock(__lock);
		
		if (__total_seq > __wakeup_seq)
		{
			__wakeup_seq = __total_seq;
			__woken_seq = __total_seq;
			__futex = 2*__total_seq;
			++__broadcast_seq;
			
			// TODO: Signal scheduler
			
		}
		
		low_unlock(__lock);
	}
	
	
	template <class M>
	void generic_condition_variable<M>::wait(unique_lock<mutex_type>& mut) throw()
	{
		using namespace Support::threading;
		
		low_lock(__lock);
		auto thread = Kernel::current_processor->current_thread;
		assert(thread);
		
		assert(thread);
		
		mut.unlock();
		++__total_seq;
		++__futex;
		__nwaiters += (1 << NCOND_SHIFT);
		
		
		if ((void*)__mutex != (void*)~0l)
		{
			__mutex = mut.mutex();
		}
		
		
		auto broadcast_count = __broadcast_seq;
		auto val = __wakeup_seq;
		auto seq = val;
		
		do
		{
			auto futex_val = __futex;
			
			low_unlock(__lock);
			
			
			// Wait stuff here
			//assert(NOT_IMPLEMENTED);
			// Crappy Debuf Implementation
			// TODO
			// TODO
			// DON'T LEAVE THIS
			// TODO
			// TODO
			#if !defined(DEBUG) || defined(NDEBUG)
			abort();
			#endif
			while (__futex == futex_val)
			{
				sched_yield();
				__sync_synchronize();
			}
			
			
			low_lock(__lock);
			
			if (__broadcast_seq != broadcast_count)
			{
				break;
			}
			
			val = __wakeup_seq;
		}
		while (val == seq || __woken_seq == val);
		
		if (__broadcast_seq == broadcast_count)
		{
			++__woken_seq;
		}
		
		
		__nwaiters -= (1 << NCOND_SHIFT);
		
		low_unlock(__lock);
		
		mut.lock();
		
	}
}

#endif