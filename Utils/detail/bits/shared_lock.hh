#ifndef INCLUDED_BITS_SHARED_LOCK_HH
#define INCLUDED_BITS_SHARED_LOCK_HH

#include "decay.hh"
#include <Utils/utility>

namespace Utils
{
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
		
		void lock()
		{
			assert(_mutex);
			assert(!owned);
			_mutex->lock_shared();
			owned = true;
		}
		
		bool try_lock()
		{
			assert(_mutex);
			assert(!owned);
			return ((owned = _mutex->try_lock_shared()));
		}
		
		template <class Rep, class Period>
		bool try_lock_for(const chrono::duration<Rep, Period>& dt)
		{
			assert(_mutex);
			assert(!owned);
			return ((owned = _mutex->try_lock_shared_for(dt)));
		}
		
		template <class Rep, class Period>
		bool try_lock_until(const chrono::time_point<Rep, Period>& t)
		{
			assert(_mutex);
			assert(!owned);
			return ((owned = _mutex->try_lock_shared_until(t)));
		}
		
		void unlock()
		{
			assert(_mutex);
			assert(owned);
			_mutex->unlock_shared();
			owned = false;
		}
		
		void swap(shared_lock& other) noexcept
		{
			swap(_mutex, other._mutex);
			swap(owned, other.owned);
		}
		
		mutex_type* release() noexcept
		{
			
			auto old = _mutex;
			_mutex = nullptr;
			return old;
		}
		
		mutex_type* mutex() const noexcept;
		bool owns_lock() const noexcept
		{
			return _mutex && owned;
		}
		
		__attribute__((__always_inline__))
		explicit operator bool() const noexcept
		{
			return owns_lock();
		}
		
		
	};
}

#endif