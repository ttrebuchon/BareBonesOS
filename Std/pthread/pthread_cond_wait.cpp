#include <kernel/Scheduling/thread.h>
#include <kernel/Processor.h>
#include "pthread.hh"
#include "osthread.h"


#define NCOND_SHIFT 0

extern "C" {
	
	int osthread_cond_wait(osthread_cond_t* cond, osthread_mutex_t* mut)
	{
		int err;
		int pshared = (cond->__data.__mutex == (void*)~0l) ? 128 : 0;
		
		
		
		lll_lock(cond->__data.__lock);
		auto thread = Kernel::current_processor->current_thread;
		assert(thread);
		
		osthread_mutex_unlock(mut);
		++cond->__data.__total_seq;
		++cond->__data.__futex;
		cond->__data.__nwaiters += (1 << NCOND_SHIFT);
		
		if (cond->__data.__mutex != (void*)~0l)
		{
			cond->__data.__mutex = mut;
		}
		
		
		auto broadcast_count = cond->__data.__broadcast_seq;
		auto val = cond->__data.__wakeup_seq;
		auto seq = val;
		
		do
		{
			//auto futex_val = cond->__data.__futex;
			
			lll_unlock(cond->__data.__lock);
			
			
			// Wait stuff here
			assert(NOT_IMPLEMENTED);
			
			
			lll_lock(cond->__data.__lock);
			
			if (cond->__data.__broadcast_seq != broadcast_count)
			{
				break;
			}
			
			val = cond->__data.__wakeup_seq;
		}
		while (val == seq || cond->__data.__woken_seq == val);
		
		if (cond->__data.__broadcast_seq == broadcast_count)
		{
			++cond->__data.__woken_seq;
		}
		
		
		cond->__data.__nwaiters -= (1 << NCOND_SHIFT);
		
		lll_unlock(cond->__data.__lock);
		
		osthread_mutex_lock(mut);
		return 0;
	}
	
	
}