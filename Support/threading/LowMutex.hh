#ifndef INCLUDED_SUPPORT_THREADING_LOWMUTEX_HH
#define INCLUDED_SUPPORT_THREADING_LOWMUTEX_HH

#include "LowLock.hh"


namespace Support::threading
{
	class low_mutex
	{
		private:
		int_lock_t state;
		
		public:
		constexpr low_mutex() noexcept : state(0)
		{
			
		}
		
		low_mutex(const low_mutex&) = delete;
		~low_mutex() noexcept = default;
		
		
		low_mutex& operator=(const low_mutex&) = delete;
		
		
		void lock();
		bool try_lock() noexcept;
		void unlock() noexcept;
	};
}

#endif