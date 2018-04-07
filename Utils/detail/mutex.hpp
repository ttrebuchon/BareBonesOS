#ifndef INCLUDED_MUTEX_HPP
#define INCLUDED_MUTEX_HPP

#include "mutex.hh"

namespace Utils
{
	template <class M>
	unique_lock<M>::unique_lock() noexcept : _m(nullptr), _owns(false) 
	{
		
	}
	
	template <class M>
	unique_lock<M>::unique_lock(unique_lock&& u) noexcept : _m(u._m), _owns(u._owns) 
	{
		u._m = nullptr;
		u._owns = false;
	}
	
	template <class M>
	unique_lock<M>::unique_lock(mutex_type& m) : _m(&m), _owns(true)
	{
		_m->lock();
	}
	
	template <class M>
	unique_lock<M>::unique_lock(mutex_type& m, try_to_lock_t) : _m(&m), _owns(m.try_lock())
	{
		
	}
	
	template <class M>
	unique_lock<M>::unique_lock(mutex_type& m, adopt_lock_t) : _m(&m), _owns(true)
	{
		
	}
	
	template <class M>
	unique_lock<M>::unique_lock(mutex_type& m, defer_lock_t) noexcept : _m(&m), _owns(false)
	{
		
	}
	
	template <class M>
	unique_lock<M>::~unique_lock()
	{
		if (_owns)
		{
			_m->unlock();
		}
	}
	
	
	
	
}

#endif