#ifndef INCLUDED_KERNEL_SCHEDULER_H
#define INCLUDED_KERNEL_SCHEDULER_H

#include <Common.h>

#ifdef __cplusplus
#include <Utils/list>
#include <Utils/map>
#include <Support/threading/SpinMutex.hh>
#include <Support/collections/concurrent_circular_list.hh>
#include <kernel/Memory/Allocators/cross_processor_allocator.hh>
#endif

START_NS(Kernel)

typedef struct __thread_struct __thread_t;
typedef struct thread_data thread_data_t;

#ifdef __cplusplus

namespace detail
{
	template <class T>
	using scheduler_list = Support::Collections::concurrent_circular_list<T, Support::threading::spin_mutex, Memory::cross_proc_allocator<T>>;
	
	
}

class Scheduler
{
	public:
	
	
	protected:
	Support::threading::spin_mutex spin_lock;
	//typedef Support::Collections::concurrent_circular_list<__thread_t*, Support::threading::spin_mutex, Memory::cross_proc_allocator<__thread_t*>>
	
	Utils::map<pthread_t, __thread_t*> __thread_map;
	/*Support::Collections::concurrent_circular_list<__thread_t*, Support::threading::spin_mutex, Memory::cross_proc_allocator<__thread_t*>>*/ detail::scheduler_list<__thread_t*> runlist;
	typename detail::scheduler_list<__thread_t*>::iterator thread_it;
	Utils::map<pthread_t, __thread_t*> waiting_threads;
	detail::scheduler_list<pthread_t> threads_to_reap;
	pthread_t pid_counter;
	
	public:
	Scheduler();
	//Memory::cross_proc_allocator<__thread> thread_alloc;
	
	
	__thread_t* current_thread() const noexcept;
	
	//__attribute__((__always_inline__))
	pthread_t current_thread_id() const noexcept;/*
	{
		auto t = current_thread();
		assert(t);
		return t->id;
	}*/
	
	int register_process(struct process*);
	
	
	
	static Scheduler* Current;
};

Scheduler* get_scheduler();


#endif




END_NS


#endif