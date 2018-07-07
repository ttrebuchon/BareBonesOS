#include "../osthread.h"


extern "C" {
	
	int osthread_mutexattr_gettype(const osthread_mutexattr_t* attr, int* type)
	{
		assert(attr);
		if (type)
		{
			*type = attr->__kind;
		}
		return 0;
	}
	
	int osthread_mutexattr_settype(osthread_mutexattr_t* attr, int type)
	{
		if (type < OSTHREAD_MUTEX_NORMAL || type > OSTHREAD_MUTEX_RECURSIVE)
		{
			return EINVAL;
		}
		
		assert(attr);
		attr->__kind = type;
		
		return 0;
	}
	
}