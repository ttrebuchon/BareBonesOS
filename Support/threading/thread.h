#ifndef INCLUDED_SUPPORT_THREADING_THREAD_H
#define INCLUDED_SUPPORT_THREADING_THREAD_H

#include <Common.h>
#include "Types.h"

C_CODE
START_NS(Support)
START_NS(threading)


void* thread_getspecific(thread_key_t);
int thread_setspecific(thread_key_t, const void*);

int thread_once(thread_once_t*, void (*)(void));

int thread_mutex_lock(thread_mutex_t*);

int thread_mutex_unlock(thread_mutex_t*);

int thread_key_create(thread_key_t*, void (*)(void*));

int thread_cond_signal(thread_cond_t*);

int thread_cond_broadcast(thread_cond_t*);

int thread_cond_wait(thread_cond_t*, thread_mutex_t*);


END_NS // (threading)
END_NS // (Support)
C_END


#endif