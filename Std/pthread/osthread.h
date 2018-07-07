#ifndef INCLUDED_PTHREAD_OSTHREAD_H
#define INCLUDED_PTHREAD_OSTHREAD_H

#include <Common.h>
#include <kernel/Scheduling/API.h>
#include "osthread_types.h"

#define OSTHREAD_ONCE_INIT 0

# define OSTHREAD_MUTEX_INITIALIZER \
  { { { 0 } }, OSTHREAD_MUTEX_NOT_INIT }

/* Conditional variable handling.  */
#define OSTHREAD_COND_INITIALIZER { { 0, 0, 0, 0, 0, (void *) 0, 0, 0 } }

extern "C" {
	
	void* osthread_getspecific(osthread_key_t);
	int osthread_setspecific(osthread_key_t, const void*);
	
	int osthread_once(osthread_once_t*, void (*)(void));
	
	int osthread_mutex_lock(osthread_mutex_t*);
	
	int osthread_mutex_unlock(osthread_mutex_t*);
	
	int osthread_key_create(osthread_key_t*, void (*)(void*));
	
	int osthread_cond_signal(osthread_cond_t*);
	
	int osthread_cond_broadcast(osthread_cond_t*);
	
	int osthread_cond_wait(osthread_cond_t*, osthread_mutex_t*);
	
	int osthread_mutex_init(osthread_mutex_t*, const osthread_mutexattr_t*);
	int osthread_mutex_destroy(osthread_mutex_t*);
	
	
	int osthread_mutexattr_gettype(const osthread_mutexattr_t* attr, int* type);
	int osthread_mutexattr_settype(osthread_mutexattr_t* attr, int type);
}
#endif