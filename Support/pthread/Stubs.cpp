#include <pthread.h>
#include <Common.h>

extern "C" {
	
	void* pthread_getspecific(pthread_key_t)
	{
		ASSERT(false);
	}
	
	int pthread_setspecific(pthread_key_t, const void*)
	{
		ASSERT(false);
	}
	
	int pthread_once(pthread_once_t*, void (*)(void))
	{
		ASSERT(false);
	}
	
	int pthread_mutex_lock(pthread_mutex_t*)
	{
		ASSERT(false);
	}
	
	int pthread_mutex_unlock(pthread_mutex_t*)
	{
		ASSERT(false);
	}
	
	int pthread_key_create(pthread_key_t*, void (*)(void*))
	{
		ASSERT(false);
	}
	
}