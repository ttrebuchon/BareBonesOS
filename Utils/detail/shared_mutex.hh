#ifndef INCLUDED_SHARED_MUTEX_HH
#define INCLUDED_SHARED_MUTEX_HH

#include <Common.h>
#include "mutex.hh"
#include "condition_variable.hh"
#include <Utils/chrono>


namespace Utils
{
	class shared_timed_mutex
	{
		private:
		
		protected:
		mutex _mut;
		condition_variable rwait, wwait;
		unsigned int writers;
		unsigned int readers;
		unsigned int waiting_readers;
		
		/*static constexpr unsigned _write_entered = 1u << (sizeof(unsigned)*8 - 1);
		static constexpr unsigned _n_readers = ~_write_entered;*/
		
		constexpr static unsigned int max_readers = 10;
		
		public:
		
		shared_timed_mutex();
		shared_timed_mutex(const shared_timed_mutex&) = delete;
		
		__attribute__((__always_inline__))
		~shared_timed_mutex() = default;
		
		
		
		// Methods
		void lock();
		bool try_lock();
		template <class Rep, class Period>
		bool try_lock_for(const chrono::duration<Rep, Period>& t)
		{
			return try_lock_until(chrono::steady_clock::now() + t);
		}
		template <class Clock, class Dur>
		bool try_lock_until(const chrono::time_point<Clock, Dur>& t);
		void unlock();
		
		void lock_shared();
		bool try_lock_shared();
		template <class Rep, class Period>
		bool try_lock_shared_for(const chrono::duration<Rep, Period>& t)
		{
			return try_lock_shared_until(chrono::steady_clock::now() + t);
		}
		template <class Clock, class Dur>
		bool try_lock_shared_until(const chrono::time_point<Clock, Dur>& t);
		void unlock_shared();
		
		
		// Operators
		shared_timed_mutex& operator=(const shared_timed_mutex&) = delete;
	};
	
}

#endif