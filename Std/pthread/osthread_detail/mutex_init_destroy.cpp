#include "../osthread.h"
#include "mutex_type_internal.hh"


#define err_enum(msg) \
	enum \
	{ \
		msg = 0x0, \
	}
	
	err_enum(KIND_OUT_OF_RANGE);

static int init_spin_mut(osthread_mutex_t* _mut, const osthread_mutexattr_t* attr)
{
	memset(_mut, 0, sizeof(osthread_mutex_t));
	
	auto mut = reinterpret_cast<osthread::mutex_intern_t*>(_mut);
	
	mut->type = OSTHREAD_MUTEX_SPIN;
	
	new (&mut->__lock.__spin) osthread::__Lock<Support::threading::spin_mutex>();
	reinterpret_cast<osthread::Lock*>(&mut->__lock)->init();
	
	return 0;
}


static int destroy_spin_mut(osthread_mutex_t* _mut)
{
	using namespace osthread;
	auto mut = reinterpret_cast<mutex_intern_t*>(_mut);
	
	assert(mut->type == OSTHREAD_MUTEX_SPIN);
	if (mut->type != OSTHREAD_MUTEX_SPIN)
	{
		return EINVAL;
	}
	
	reinterpret_cast<Lock*>(&mut->__lock)->~Lock();
	
	return 0;
}




extern "C" {
	
	
	
	
	int osthread_mutex_init(osthread_mutex_t* mut, const osthread_mutexattr_t* attr)
	{
		assert(mut);
		int kind = OSTHREAD_MUTEX_DEFAULT;
		int err;
		
		if (attr)
		{
			err = osthread_mutexattr_gettype(attr, &kind);
			if (err)
			{
				return err;
			}
		}
		
		assert(kind - 0xA0 <= 0x02);
		
		switch (kind)
		{
			case OSTHREAD_MUTEX_NORMAL:
			return init_spin_mut(mut, attr);
			
			case OSTHREAD_MUTEX_ERRORCHECK:
			assert(NOT_IMPLEMENTED);
			
			case OSTHREAD_MUTEX_RECURSIVE:
			assert(NOT_IMPLEMENTED);
			
			default:
			assert(KIND_OUT_OF_RANGE);
		}
	}
	
	int osthread_mutex_destroy(osthread_mutex_t* mut)
	{
		assert(mut);
		
		switch (mut->type)
		{
			case OSTHREAD_MUTEX_SPIN:
			return destroy_spin_mut(mut);
			
			case OSTHREAD_MUTEX_DONATE:
			assert(NOT_IMPLEMENTED);
			
			case OSTHREAD_MUTEX_YIELD_SPIN:
			assert(NOT_IMPLEMENTED);
			
			case OSTHREAD_MUTEX_WAIT:
			assert(NOT_IMPLEMENTED);
			
			default:
			assert(KIND_OUT_OF_RANGE);
		}
	}
}