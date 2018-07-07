#include "../osthread.h"
#include "mutex_type_internal.hh"

static int osthread_spin_mutex_lock(osthread_mutex_t* _mut)
{
	using namespace osthread;
	auto mut = reinterpret_cast<mutex_intern_t*>(_mut);
	assert(mut);
	reinterpret_cast<Lock*>(&mut->__lock)->lock();
	return 0;
}

static int osthread_donate_mutex_lock(osthread_mutex_t* mut)
{
	assert(NOT_IMPLEMENTED);
}


static int osthread_spin_mutex_unlock(osthread_mutex_t* _mut)
{
	using namespace osthread;
	auto mut = reinterpret_cast<mutex_intern_t*>(_mut);
	assert(mut);
	reinterpret_cast<Lock*>(&mut->__lock)->unlock();
	return 0;
}

static int osthread_donate_mutex_unlock(osthread_mutex_t* mut)
{
	assert(NOT_IMPLEMENTED);
}


enum
{
	OSTHREAD_MUTEX_NOT_INITIALIZED = 0,
};


extern "C" {
	
	int osthread_mutex_lock(osthread_mutex_t* mut)
	{
		assert(mut);
		assert(mut->type != OSTHREAD_MUTEX_NOT_INITIALIZED);
		if (mut->type == OSTHREAD_MUTEX_NOT_INITIALIZED)
		{
			return EINVAL;
		}
		
		using namespace osthread;
		reinterpret_cast<Lock*>(&reinterpret_cast<mutex_intern_t*>(mut)->__lock)->lock();
		
		return 0;
		
		switch (mut->type)
		{
			case 0:
			assert(OSTHREAD_MUTEX_NOT_INITIALIZED);
			__builtin_unreachable();
			
			
			case OSTHREAD_MUTEX_SPIN:
			return osthread_spin_mutex_lock(mut);
			
			case OSTHREAD_MUTEX_DONATE:
			return osthread_donate_mutex_lock(mut);
			
			
			default:
			assert(false);
			
		}
	}
	
	
	int osthread_mutex_unlock(osthread_mutex_t* mut)
	{
		assert(mut);
		assert(mut->type != OSTHREAD_MUTEX_NOT_INITIALIZED);
		if (mut->type == OSTHREAD_MUTEX_NOT_INITIALIZED)
		{
			return EINVAL;
		}
		
		using namespace osthread;
		reinterpret_cast<Lock*>(&reinterpret_cast<mutex_intern_t*>(mut)->__lock)->unlock();
		
		return 0;
		
		
		switch (mut->type)
		{
			case OSTHREAD_MUTEX_SPIN:
			return osthread_spin_mutex_unlock(mut);
			
			case OSTHREAD_MUTEX_DONATE:
			return osthread_donate_mutex_unlock(mut);
			
			
			default:
			assert(false);
			
		}
	}
	
}