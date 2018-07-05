#ifndef INCLUDED_SUPPORT_THREADING_SPINYIELDMUTEX_HH
#define INCLUDED_SUPPORT_THREADING_SPINYIELDMUTEX_HH

#include <Common.h>
#include <Utils/detail/atomic.hpp>

namespace Support::threading
{
	class spin_yield_mutex
	{
		protected:
		Utils::atomic<int> _lcount;
		
		public:
		spin_yield_mutex();
		spin_yield_mutex(const spin_yield_mutex&) = delete;
		
		spin_yield_mutex& operator=(const spin_yield_mutex&) = delete;
		
		
		void lock();
		bool try_lock() noexcept;
		void unlock() noexcept;
	};
}

#endif