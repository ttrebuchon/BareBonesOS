#ifndef INCLUDED_SUPPORT_THREADING_LOWLOCK_HH
#define INCLUDED_SUPPORT_THREADING_LOWLOCK_HH

typedef int int_lock_t;

namespace Support::threading
{

template <class T>
static inline void __attribute__((__always_inline__)) low_lock(T& val, bool yield = true)
{
	T tmp = 0;
	while (!__atomic_compare_exchange_n(&val, &tmp, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
	{
		if (yield)
		{
			sched_yield();
		}
		tmp = 0;
	}
}

template <class T>
static inline void __attribute__((__always_inline__)) low_unlock(T& val)
{
	__atomic_exchange_n(&val, 0, __ATOMIC_RELEASE);
}

}

#endif