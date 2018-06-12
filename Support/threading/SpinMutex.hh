#ifndef INCLUDED_SUPPORT_THREADING_SPINMUTEX_HH
#define INCLUDED_SUPPORT_THREADING_SPINMUTEX_HH

#include <Common.h>
#include <Utils/detail/atomic.hpp>

namespace Support::threading
{
	class spin_mutex
	{
		protected:
		Utils::atomic<int> _lcount;
		
		public:
		spin_mutex();
		spin_mutex(const spin_mutex&) = delete;
		
		spin_mutex& operator=(const spin_mutex&) = delete;
		
		
		void lock();
		bool try_lock() noexcept;
		void unlock() noexcept;
	};
}

#endif