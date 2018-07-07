#ifndef INCLUDED_PTHREAD_OSTHREAD_DETAIL_MUTEX_TYPE_INTERNAL_HH
#define INCLUDED_PTHREAD_OSTHREAD_DETAIL_MUTEX_TYPE_INTERNAL_HH

#include "../osthread_mutex.h"

#include <Support/threading/SpinMutex.hh>



namespace osthread
{
	
	class Lock
	{
		protected:
		
		public:
		virtual void init() = 0;
		virtual void lock() = 0;
		virtual void unlock() = 0;
		virtual bool try_lock() = 0;
	};
	
	template <class M>
	class __Lock : public Lock
	{
		protected:
		M mut;
		
		public:
		virtual void init() {}
		virtual void lock()
		{
			mut.lock();
		}
		
		virtual void unlock()
		{
			mut.unlock();
		}
		
		virtual bool try_lock()
		{
			return mut.try_lock();
		}
	};
	
	struct mutex_intern_t
	{
		union
		{
			char __size[OSTHREAD_SIZEOF_LOCK];
			
			__Lock<Support::threading::spin_mutex> __spin;
			
			struct
			{
				
			} __donate;
			
			struct
			{
				
			} __yield_spin;
			
			struct
			{
				
			} __wait;
			
			
		}  __attribute__((__aligned__(OSTHREAD_ALIGNOF_LOCK))) __lock;
		
		uword_t type;
	};
	
	static_assert(alignof(mutex_intern_t) == alignof(osthread_mutex_t));
	static_assert(sizeof(mutex_intern_t) == sizeof(osthread_mutex_t));
	
	

}




#endif