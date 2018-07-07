#include <pthread.h>
#include <Common.h>

#ifndef PTHREAD_IGNORE
	#define FAIL assert(NOT_IMPLEMENTED)
#else
	#define FAIL 
#endif

extern "C" {
	
	void* pthread_getspecific(pthread_key_t)
	{
		FAIL;
		return nullptr;
	}
	
	int pthread_setspecific(pthread_key_t, const void*)
	{
		FAIL;
		return -1;
	}
	
	int pthread_mutex_lock(pthread_mutex_t*)
	{
		FAIL;
		return -1;
	}
	
	int pthread_mutex_unlock(pthread_mutex_t*)
	{
		FAIL;
		return -1;
	}
	
	int pthread_cond_signal(pthread_cond_t*)
	{
		FAIL;
		return -1;
	}
	
	int pthread_cond_broadcast(pthread_cond_t*)
	{
		FAIL;
		return -1;
	}
	
	int pthread_cond_wait(pthread_cond_t*, pthread_mutex_t*)
	{
		FAIL;
		return -1;
	}
	
	
	int __sF;
}