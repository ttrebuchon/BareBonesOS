#include "thread.h"
#include <Std/stdlib.h>
#include <Utils/map>
#include <Utils/list>
#include <iostream>

namespace Kernel
{
extern "C" {
	
	typedef struct thread_data
	{
		Utils::map<int, void*> values;
		void* ret;
	}
	thread_data_t;
	
	
	
	
	
	static pthread_t tid_counter = 0;
	
	Utils::map<pthread_t, __thread_t*>* get_threadmap_for_current_process();
	
	bool register_thread_current(__thread_t*);
	
	
	void thread_entry_func(__thread_t*);
	static void remove_thread_from_list(__thread_t*);
	static pthread_t get_inc_tid();
	__attribute__((__noreturn__))
	static void deregister_exit_thread_current(void* ret);
	
	int thread_create(pthread_t* thread, void(*start)(void*), void* arg)
	{
		const size_t init_stack_size = 16*1024;
		assert(thread);
		auto __thr = new __thread_t;
		auto context = new context_t;
		auto entry = new thread_entry_t;
		
		__thr->context = context;
		__thr->entry = entry;
		__thr->data = nullptr;
		__thr->id = get_inc_tid();
		
		*thread = __thr->id;
		
		
		entry->data = arg;
		entry->enter = start;
		entry->destroy =
		[](auto e) -> void
		{
			delete e;
		};
		
		
		auto stack = malloc(init_stack_size);
		assert((addr_t)stack % 16 == 0);
		
		context->stack.base = stack;
		constexpr ptrdiff_t off = sizeof(void*) + (16 - (sizeof(void*) % 16));
		static_assert(sizeof(void*) == 8);
		
		context->stack.sp = (void*)(((addr_t)stack) + init_stack_size - off);
		context->stack.fp = context->stack.sp;
		
		context->ip = (void*)&thread_entry_func;
		assert(context->ip != nullptr);
		
		context->registers.x0 = (addr_t)(void*)__thr;
		context->registers.x1 = (addr_t)(void*)__thr;
		
		
		register_thread_current(__thr);
		
		return 0;
	}
	
	void thread_exit(void* retval)
	{
		deregister_exit_thread_current(retval);
	}
	
	void thread_entry_func(__thread_t* thread)
	{
		assert(thread);
		
		auto entry = thread->entry;
		
		entry->enter(entry->data);
		
		thread_exit(nullptr);
		
		/*if (entry->destroy)
		{
			entry->destroy(entry);
			entry = nullptr;
			thread->entry = nullptr;
		}
		
		remove_thread_from_list(thread);*/
		
	}
	
	static pthread_t get_inc_tid()
	{
		return tid_counter++;
	}
	
	
	
	
	#ifdef TESTING
	
	static Utils::map<pthread_t, __thread_t*> __thread_map;
	static Utils::list<__thread_t*> runlist;
	static typename Utils::list<__thread_t*>::iterator thread_it = runlist.begin();
	
	bool register_thread_current(__thread_t* thread)
	{
		assert(thread);
		__thread_map[thread->id] = thread;
		runlist.push_back(thread);
		return true;
	}
	
	
	void init_kernel_threads()
	{
		auto thread = new __thread_t;
		auto context = new context_t;
		
		thread->context = context;
		thread->entry = nullptr;
		thread->data = nullptr;
		thread->id = get_inc_tid();
		
		__thread_map[thread->id] = thread;
		runlist.push_back(thread);
		thread_it = runlist.begin();
	}
	
	int thread_count()
	{
		return runlist.size();
	}
	
	static void deregister_exit_thread_current(void* ret)
	{
		auto id = (*thread_it)->id;
		thread_it = runlist.erase(thread_it);
		if (thread_it == runlist.end())
		{
			thread_it = runlist.begin();
		}
		
		auto thread = __thread_map[id];
		
		if (!thread->data)
		{
			thread->data = new thread_data_t;
		}
		
		thread->data->ret = ret;
		
		
		if (thread->entry)
		{
			if (thread->entry->destroy)
			{
				thread->entry->destroy(thread->entry);
				thread->entry = nullptr;
			}
		}
		
		load_context((*thread_it)->context);
	}
	
	#endif
}
}

#ifdef TESTING

extern "C" int sched_yield()
{
	using namespace Kernel;
	assert(thread_it != runlist.end());
	auto old = *thread_it;
	++thread_it;
	if (thread_it == runlist.end())
	{
		thread_it = runlist.begin();
	}
	
	auto next = *thread_it;
	if (next == old)
	{
		return -1;
	}
	
	if (save_context(old->context) == 1)
	{
		return 0;
	}
	
	load_context(next->context);
}

#endif