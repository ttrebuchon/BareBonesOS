#ifndef INCLUDED_MUTEX_HH
#define INCLUDED_MUTEX_HH

#include <Common.h>
#include "atomic.hpp"

namespace Utils
{
	
	class mutex
	{
		protected:
		atomic<int> _lcount;
		
		public:
		mutex();
		mutex(const mutex&) = delete;
		
		mutex& operator=(const mutex&) = delete;
		
		
		void lock();
		bool try_lock();
		void unlock();
	};
	
}

#endif