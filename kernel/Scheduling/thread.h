#ifndef INCLUDED_SCHEDULING_THREAD_H
#define INCLUDED_SCHEDULING_THREAD_H

#include <Common.h>
#include "context.h"
#include "API.h"

#ifdef __cplusplus

#include <Utils/type_traits>
#include <Utils/tuple>

#endif

START_NS(Kernel)
C_CODE



typedef struct thread_data thread_data_t;

typedef struct thread_entry
{
	void* data;
	void(*enter)(void*);
	void(*destroy)(struct thread_entry*);
	
} thread_entry_t;

typedef struct
{
	thread_entry_t* entry;
	context_t* context;
	pthread_t id;
	thread_data_t* data;
} __thread_t;



int thread_create(pthread_t*, void(*start)(void*), void* arg);

__attribute__((__noreturn__))
void thread_exit(void* retval);

int thread_join(pthread_t, void** retval);

void init_kernel_threads();

int thread_count();

C_END


#ifdef __cplusplus

template <class Fn, class... Args>
int thread_create(pthread_t* thread, Fn fn, Args... args)
{
	struct Thread_Args
	{
		
		typename Utils::decay<Fn>::type fn;
		Utils::tuple<typename Utils::decay<Args>::type...> args;
		
		Thread_Args(Fn fn, Args... args) : fn(Utils::forward<Fn>(fn)), args(Utils::forward<Args>(args)...)
		{
			
		}
	};
	
	auto args_pack = new Thread_Args(Utils::forward<Fn>(fn), Utils::forward<Args>(args)...);
	
	void(*unpacker)(void*) = [](void* _args)
	{
		Thread_Args* args = (Thread_Args*)_args;
		args->fn();
		delete args;
	};
	
	return thread_create(thread, unpacker, (void*)args_pack);
}





#endif


END_NS


#endif