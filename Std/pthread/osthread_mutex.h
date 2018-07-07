#ifndef INCLUDED_PTHREAD_OSTHREAD_MUTEX_H
#define INCLUDED_PTHREAD_OSTHREAD_MUTEX_H

#include <Common.h>

C_CODE


#define OSTHREAD_MUTEX_NOT_INIT 0
#define OSTHREAD_MUTEX_SPIN 1
#define OSTHREAD_MUTEX_DONATE 2
#define OSTHREAD_MUTEX_YIELD_SPIN 3
#define OSTHREAD_MUTEX_WAIT 4

#define OSTHREAD_SIZEOF_LOCK 128
#define OSTHREAD_ALIGNOF_LOCK 8




typedef struct
{
	
	
	union
	{
		char __size[OSTHREAD_SIZEOF_LOCK];
		
		struct
		{
			int _lock;
		} __spin;
		
		struct
		{
			
		} __donate;
		
		struct
		{
			
		} __yield_spin;
		
		struct
		{
			
		} __wait;
		
		
		
	} __attribute__((__aligned__(OSTHREAD_ALIGNOF_LOCK)));
	
	uword_t type;
	
} osthread_mutex_t;

#ifdef __cplusplus

//static_assert(sizeof(osthread_mutex_t) - sizeof(uword_t) == 128);

static_assert(sizeof(osthread_mutex_t) - sizeof(uword_t) - OSTHREAD_ALIGNOF_LOCK <= OSTHREAD_SIZEOF_LOCK);
#endif



#define OSTHREAD_MUTEX_NORMAL 0xA0
#define OSTHREAD_MUTEX_ERRORCHECK 0xA1
#define OSTHREAD_MUTEX_RECURSIVE 0xA2

#define OSTHREAD_MUTEX_DEFAULT OSTHREAD_MUTEX_NORMAL


C_END


#endif