#ifndef INCLUDED_LOCKS_HPP
#define INCLUDED_LOCKS_HPP

#include "locks.hh"

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
			_owns = false;
		}
	}
	
	
	
	
	template <class M>
	void unique_lock<M>::lock()
	{
		assert(_m);
		_m->lock();
		_owns = true;
	}
	
	template <class M>
	bool unique_lock<M>::try_lock()
	{
		assert(_m);
		return _owns = _m->try_lock();
	}
	
	template <class M>
	template <class Rep, class Period>
	bool unique_lock<M>::try_lock_for(const chrono::duration<Rep, Period>& timeout)
	{
		assert(_m);
		return _owns = _m->try_lock_for(timeout);
	}
	
	template <class M>
	template <class Clock, class Duration>
	bool unique_lock<M>::try_lock_until(const chrono::time_point<Clock, Duration>& timeout_time)
	{
		assert(_m);
		return _owns = _m->try_lock_until(timeout_time);
	}
	
	template <class M>
	void unique_lock<M>::unlock()
	{
		assert(_m);
		_m->unlock();
		_owns = false;
	}
	
	template <class M>
	void unique_lock<M>::swap(unique_lock& other) noexcept
	{
		swap(_m, other._m);
		swap(_owns, other._owns);
	}
	
	template <class M>
	auto unique_lock<M>::release() noexcept -> mutex_type*
	{
		auto old = _m;
		_m = nullptr;
		_owns = false;
		return old;
	}
	
	template <class M>
	auto unique_lock<M>::mutex() const noexcept -> mutex_type*
	{
		return _m;
	}
	
}

#endif