#ifndef INCLUDED_MUTEX_HH
#define INCLUDED_MUTEX_HH

#include <Common.h>
#include "atomic.hpp"
#include "bits/lock_tags.hh"
#include <Support/threading/mutex.hh>
#include "chrono_fwd.hh"



namespace Utils
{
	
	
	
	class mutex
	{
		protected:
		Support::threading::spin_yield_mutex __mutex;
		
		public:
		mutex() : __mutex() {}
		mutex(const mutex&) = delete;
		
		mutex& operator=(const mutex&) = delete;
		
		__attribute__((__always_inline__))
		void lock()
		{
			__mutex.lock();
		}
		
		__attribute__((__always_inline__))
		bool try_lock()
		{
			return __mutex.try_lock();
		}
		__attribute__((__always_inline__))
		void unlock()
		{
			__mutex.unlock();
		}
	};
	
	
	/*template <class M>
	class unique_lock
	{
		public:
		typedef M mutex_type;
		
		private:
		mutex_type* _m;
		bool _owns;
		
		public:
		
		
		unique_lock() noexcept;
		unique_lock(unique_lock&&) noexcept;
		explicit unique_lock(mutex_type&);
		unique_lock(mutex_type&, try_to_lock_t);
		unique_lock(mutex_type&, adopt_lock_t);
		unique_lock(mutex_type&, defer_lock_t) noexcept;
		
		template <class Rep, class Period>
		unique_lock(mutex_type&, const Utils::chrono::duration<Rep, Period>& timeout);
		
		template <class Clock, class Dur>
		unique_lock(mutex_type&, const Utils::chrono::time_point<Clock, Dur>& timeout_point);
		
		~unique_lock();
		
		
		__attribute__((always_inline))
		bool owns_lock() const noexcept
		{ return _owns; }
		
	};*/
}

#endif