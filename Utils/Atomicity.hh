#ifndef INCLUDED_ATOMICITY_HH
#define INCLUDED_ATOMICITY_HH

#include "detail/Atomic_word.hh"

namespace Utils
{
	static inline __Atomic_word __exchange_and_add(volatile __Atomic_word* __mem, int __val)
	{
		return __sync_fetch_and_add(__mem, __val);
	}
	
	
	static inline __Atomic_word __exchange_and_add_single(__Atomic_word* __mem, int __val)
	{
		__Atomic_word __result = *__mem;
		*__mem += __val;
		return __result;
	}
	
	
	static inline __Atomic_word __attribute__((unused)) __exchange_and_add_dispatch(__Atomic_word* __mem, int __val)
	{
		return __exchange_and_add_single(__mem, __val);
	}
	
	static inline void __atomic_add_single(__Atomic_word* mem, int val)
	{
		*mem += val;
	}
	
	static inline void __attribute__((unused)) __atomic_add_dispatch(__Atomic_word* mem, int val)
	{
		#ifdef GTHREADS
		if (__gthread_active_p())
		{
			__atomic_add(mem, val);
		}
		else
		{
			__atomic_add_single(mem, val);
		}
		#else
		__atomic_add_single(mem, val);
		#endif
		
	}
}

#endif