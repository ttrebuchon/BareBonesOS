#ifndef INCLUDED_LOCKS_HH
#define INCLUDED_LOCKS_HH

#include <Common.h>
#include "bits/lock_tags.hh"

namespace Utils
{
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
	
	
	
	namespace chrono
	{
		template <class, class>
		class duration;
		
		template <class, class>
		class time_point;
	}
	
	
	template <class Mutex>
	class shared_lock
	{
		public:
		typedef Mutex mutex_type;
		
		protected:
		mutex_type* _mutex;
		bool owned;
		
		
		public:
		
		shared_lock() noexcept = default;
		shared_lock(shared_lock&&) noexcept;
		
		
		explicit shared_lock(mutex_type& m) : _mutex((m.lock_shared(), &m)), owned(true)
		{
			
		}
		
		shared_lock(mutex_type& m, try_to_lock_t) : _mutex(&m), owned(m.try_lock_shared())
		{
			
		}
		
		shared_lock(mutex_type& m, defer_lock_t) noexcept : _mutex(&m), owned(false)
		{
			
		}
		
		shared_lock(mutex_type& m, adopt_lock_t) : _mutex(&m), owned(true)
		{
			
		}
		
		
		template <class Rep, class Period>
		shared_lock(mutex_type& m, const chrono::duration<Rep, Period>& timeout);/* : _mutex(&m), owned(m.try_lock_shared_for(timeout))
		{
			
		}*/
		
		template <class Rep, class Period>
		shared_lock(mutex_type& m, const chrono::time_point<Rep, Period>& timeout);/* : _mutex(&m), owned(m.try_lock_shared_until(timeout))
		{
			
		}*/
		
		
		~shared_lock()
		{
			if (owns_lock())
			{
				_mutex->unlock_shared();
				owned = false;
			}
		}
		
		shared_lock& operator=(shared_lock&&) noexcept;
		
		void lock();
		bool try_lock();
		template <class Rep, class Period>
		bool try_lock_for(const chrono::duration<Rep, Period>&);
		template <class Rep, class Period>
		bool try_lock_until(const chrono::time_point<Rep, Period>&);
		void unlock();
		template <class Mutex2>
		void swap(shared_lock<Mutex2>&) noexcept;
		mutex_type* release() noexcept;
		mutex_type* mutex() const noexcept;
		bool owns_lock() const noexcept
		{
			return _mutex && owned;
		}
		
		explicit operator bool() const noexcept;
		
		
	};
	
}

#endif
	