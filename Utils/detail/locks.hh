#ifndef INCLUDED_LOCKS_HH
#define INCLUDED_LOCKS_HH

#include <Common.h>
#include "bits/lock_tags.hh"
#include "chrono_fwd.hh"
#include <Utils/utility>

namespace Utils
{
	template <class M>
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
		
		
		void lock();
		bool try_lock();
		template <class Rep, class Period>
		bool try_lock_for(const chrono::duration<Rep, Period>& timeout);
		template <class Clock, class Duration>
		bool try_lock_until(const chrono::time_point<Clock, Duration>& timeout_time);
		void unlock();
		void swap(unique_lock& other) noexcept;
		mutex_type* release() noexcept;
		mutex_type* mutex() const noexcept;
		
		
		__attribute__((always_inline))
		bool owns_lock() const noexcept
		{ return _owns; }
		
		
		
		// Operators
		unique_lock& operator=(unique_lock&&);
		
		__attribute__((always_inline))
		explicit operator bool() const noexcept
		{ return owns_lock(); }
		
	};
	
	
	
	template <class M>
	void swap(unique_lock<M>& lhs, unique_lock<M>& rhs) noexcept
	{
		lhs.swap(rhs);
	}
	
	
	
	template <class Mutex>
	class lock_guard
	{
		public:
		typedef Mutex mutex_type;
		
		protected:
		mutex_type& _mutex;
		
		
		public:
		
		explicit lock_guard(mutex_type& m) : _mutex((m.lock(), m))
		{
			
		}
		
		lock_guard(mutex_type& m, adopt_lock_t) : _mutex(m)
		{
			
		}
		
		lock_guard(const lock_guard&) = delete;
		
		~lock_guard()
		{
			_mutex.unlock();
		}
		
		lock_guard& operator=(const lock_guard&) = delete;
	};
	
	
	
	
	
}

#include "bits/shared_lock.hh"

#endif
	