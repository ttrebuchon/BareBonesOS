#include "thread.h"
#include <Std/stdlib.h>
#include <Utils/map>
#include <Utils/list>

namespace Kernel
{
extern "C" {
	
	#define WAIT_LIST_ID 2
	
	#define THREAD_STATUS_RUNNING 1
	#define THREAD_STATUS_DONE 2
	#define THREAD_STATUS_WAITING 4
	
	
	typedef struct thread_data
	{
		Utils::map<int, void*> values;
		void* ret;
	}
	thread_data_t;
	
	
	void scheduler_hook();
	
	
	static pthread_t tid_counter = 0;
	
	Utils::map<pthread_t, __thread_t*>* get_threadmap_for_current_process();
	
	bool register_thread_current(__thread_t*);
	
	
	
	static void remove_thread_from_list(__thread_t*);
	static pthread_t get_inc_tid();
	__attribute__((__noreturn__))
	static void deregister_exit_thread_current(void* ret);
	static __thread_t* get_current_thread();
	static thread_data_t* get_create_thread_data(pthread_t);
	static void reap_threads();

	#ifdef __ENV_AARCH64__
	void thread_entry_func(__thread_t*);
	
	int thread_create(pthread_t* thread, void(*start)(void*), void* arg)
	{
		const size_t init_stack_size = 16*1024;//*1024;
		assert(thread);
		auto __thr = new __thread_t;
		auto context = new context_t;
		auto entry = new thread_entry_t;
		
		__thr->context = context;
		__thr->entry = entry;
		__thr->data = nullptr;
		__thr->id = get_inc_tid();
		__thr->status = THREAD_STATUS_RUNNING;
		
		if (thread)
		{
			*thread = __thr->id;
		}
		
		
		entry->data = arg;
		entry->enter = start;
		entry->destroy =
		[](auto e) -> void
		{
			delete e;
		};
		
		
		auto stack = malloc(init_stack_size);
		stack = (void*)((addr_t)stack + 16 - ((addr_t)stack % 16));
		assert((addr_t)stack % 16 == 0);
		
		context->stack.base = stack;
		constexpr ptrdiff_t off = (2*sizeof(void*) + (16 - ((2*sizeof(void*)) % 16)));
		// static_assert(sizeof(void*) == 8);
		
		__thr->stack_size = init_stack_size;
		context->stack.sp = (void*)(((addr_t)stack) + init_stack_size - off);
		context->stack.fp = context->stack.sp;

		assert((addr_t)context->stack.sp % 16 == 0);

		*(void**)((addr_t)stack + init_stack_size - sizeof(void*)) = __thr;
		*(void**)((addr_t)stack + init_stack_size - off + sizeof(void*)) = __thr;
		{
			void** arr = (void**)((addr_t)stack + init_stack_size - off);
			while (arr < (void**)((addr_t)stack + init_stack_size))
			{
				arr[0] = __thr;
				arr += 1;
			}
		}
		
		context->ip = (void*)&thread_entry_func;
		assert(context->ip != nullptr);
		
		context->registers.*register_pointers::r0 = (addr_t)(void*)__thr;
		context->registers.*register_pointers::r1 = (addr_t)(void*)__thr;
		// context->registers.x0 = (addr_t)(void*)__thr;
		// context->registers.x1 = (addr_t)(void*)__thr;
		
		
		register_thread_current(__thr);
		
		return 0;
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

	#elif defined(__ENV_x86__) || defined(__VS_CODE__)

	extern "C" void thread_entry_asm();

	void thread_entry_func();

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
		__thr->status = THREAD_STATUS_RUNNING;

		if (thread)
		{
			*thread = __thr->id;
		}

		
		
		
		
		
		entry->data = arg;
		entry->enter = start;
		entry->destroy =
		[](auto e) -> void
		{
			delete e;
		};


		{
			irq_guard lock;
			TRACE("Made it here.");
		}
		
		
		auto stack = malloc(init_stack_size);
		// {
		// 	irq_guard lock;
		// 	TRACE("Made it here.");
		// }
		// memset(stack, 4, init_stack_size);
		// {
		// 	irq_guard lock;
		// 	TRACE("Made it here.");
		// }
		{
			irq_guard lock;
			TRACE("Made it here.");
			std::cout << (void*)((addr_t)stack + init_stack_size) << std::endl; 
			memset(stack, 4, init_stack_size);
			TRACE("Made it here.");
		}
		addr_t stack_addr = (addr_t)stack;
		size_t stack_size = init_stack_size;
		if (stack_addr % 16 != 0)
		{
			addr_t nAddr = ((stack_addr / 16) + 1)*16;
			addr_t endAddr = stack_addr + stack_size;
			assert(endAddr % 16 != 0);
			endAddr = ((endAddr / 16) + 1)*16;
			stack_size = endAddr - nAddr;
			stack_addr = nAddr;
			stack = (void*)stack_addr;

		}
		
		context->stack.base = stack;
		constexpr ptrdiff_t off = sizeof(void*);
		static_assert(sizeof(void*) <= 16);
		assert((stack_addr + stack_size) % 16 == 0);
		__thr->stack_size = stack_size;


		stack = (void*)(stack_addr + stack_size - 16);
		stack_addr = (addr_t)stack;

		*(void**)stack = (void*)&thread_entry_func;

		assert(*(void**)stack == (void*)thread_entry_func);
		assert((void*)(*((addr_t*)stack)) == &thread_entry_func);
		
		context->stack.sp = stack;
		context->stack.fp = context->stack.sp;
		context->registers.esp = (register_t)stack;
		context->registers.ebp = (register_t)stack;

		assert((addr_t)context->stack.sp % 16 == 0);
		
		context->ip = (void*)&thread_entry_func;
		assert(context->ip != nullptr);
		
		 context->registers.*register_pointers::r0 = 3;
		
		
		register_thread_current(__thr);

		
		return 0;
	}

	void thread_entry_func()
	{
		__thread_t* thread = get_current_thread();
		assert(thread);
		

		// Have to re-enable interrupts here, as we might have been switched as the result of
		// an interrupt, and therefore interrupts are currently disabled and we will 
		// never switch back
		asm volatile ("STI");
		auto entry = thread->entry;
		
		entry->enter(entry->data);

		TRACE("Thread finished!");
		
		thread_exit(nullptr);
		
		/*if (entry->destroy)
		{
			entry->destroy(entry);
			entry = nullptr;
			thread->entry = nullptr;
		}
		
		remove_thread_from_list(thread);*/
		
	}

	#else

	#error Unknown architecture

	#endif

	
	void thread_exit(void* retval)
	{
		deregister_exit_thread_current(retval);
	}
	
	
	
	static pthread_t get_inc_tid()
	{
		return tid_counter++;
	}

	
	
	
	
	
	//#ifdef TESTING
	
	
	// TODO: Use locks instead of irq_guards
	static Utils::map<pthread_t, __thread_t*> __thread_map;
	static Utils::list<__thread_t*> runlist;
	static typename Utils::list<__thread_t*>::iterator thread_it = runlist.begin();
	static Utils::map<pthread_t, __thread_t*> waiting_threads;
	static Utils::list<pthread_t> threads_to_reap;
	
	
	bool register_thread_current(__thread_t* thread)
	{
		assert(thread);
		Interrupts::irq_guard lock;
		__thread_map[thread->id] = thread;
		runlist.push_back(thread);
		return true;
	}
	
	
	void init_kernel_threads()
	{
		Interrupts::irq_guard lock;
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
		pthread_t id;
		{
		Interrupts::irq_guard lock;
		id = (*thread_it)->id;
		
		
		auto thread = __thread_map[id];
		
		if (ret)
		{
			if (!thread->data)
			{
				thread->data = new thread_data_t;
			}
			
			thread->data->ret = ret;
		}
		
		
		if (thread->entry)
		{
			if (thread->entry->destroy)
			{
				thread->entry->destroy(thread->entry);
				thread->entry = nullptr;
			}
		}
		
		if (thread->data)
		{
			auto data = thread->data;
			if (data->values[WAIT_LIST_ID])
			{
				for (auto thread : *(Utils::list<__thread_t*>*)data->values[WAIT_LIST_ID])
				{
					assert(thread);
					auto id = thread->id;
					waiting_threads.erase(id);
					assert(waiting_threads.count(id) == 0);
					runlist.push_back(thread);
				}
				
				delete (Utils::list<__thread_t*>*)data->values[WAIT_LIST_ID];
				data->values[WAIT_LIST_ID] = nullptr;
			}
		}
		
		
		thread_it = runlist.erase(thread_it);
		if (thread_it == runlist.end())
		{
			thread_it = runlist.begin();
		}
		
		assert(thread_it != runlist.end());
		
		
		thread->status = THREAD_STATUS_DONE;
		threads_to_reap.push_back(id);
		
		assert(threads_to_reap.size() > 0);
		
		}
		
		
		
		load_context((*thread_it)->context);
	}
	
	int thread_join(pthread_t id, void** retval)
	{
		assert(id < tid_counter);
		__thread_t* current = nullptr;
		{
			Interrupts::irq_guard lock;
			
			__thread_t* to_join = __thread_map[id];
			assert(to_join);
			
			thread_data_t* to_join_data = nullptr;
			
			if (to_join->status == THREAD_STATUS_DONE)
			{
				if (retval)
				{
					to_join_data = to_join->data;
					if (to_join_data)
					{
						*retval = to_join_data->ret;
					}
					else
					{
						*retval = nullptr;
					}
				}
				return 0;
			}
			
			
			to_join_data = get_create_thread_data(id);
			assert(to_join_data);
			
			assert(runlist.size() > 1);
			
			current = get_current_thread();
			assert(current);
			
			assert(current != to_join);
			
			pthread_t this_id = current->id;
			waiting_threads[this_id] = current;
			thread_it = runlist.erase(thread_it);
			if (thread_it == runlist.end())
			{
				thread_it = runlist.begin();
			}
			assert(thread_it != runlist.end());
			
			
			
			
			auto wait_list = (Utils::list<__thread_t*>*)to_join_data->values[WAIT_LIST_ID];
			if (!wait_list)
			{
				to_join_data->values[WAIT_LIST_ID] = wait_list = new Utils::list<__thread_t*>();
			}
			
			wait_list->push_back(current);
			current->status = THREAD_STATUS_WAITING;
		}
		
		
		
		int save_result = save_context(current->context);
		
		if (save_result == 0)
		{
			load_context((*thread_it)->context);
		}
		
		{
			Interrupts::irq_guard lock;
			
			if (retval)
			{
				thread_data_t* to_join_data = get_create_thread_data(id);
				assert(to_join_data);
				*retval = to_join_data->ret;
			}
			
			current->status = THREAD_STATUS_RUNNING;
			
			return 0;
		}
		
		
		assert(NOT_IMPLEMENTED);
	}
	
	//#endif


	static __thread_t* get_current_thread()
	{
		assert(thread_it != runlist.end());
		return *thread_it;
	}
	
	static thread_data_t* get_create_thread_data(pthread_t id)
	{
		// TODO: Lock for __thread_map
		auto thread = __thread_map[id];
		assert(thread);
		{
			Interrupts::irq_guard lock;
			
			
			if (!thread->data)
			{
				thread->data = new thread_data_t;
			}
			
			return thread->data;
		}
	}

	void scheduler_hook()
	{
		// TRACE("Timer called scheduler!\n");
		if (runlist.size() != 0)
		{
			// TRACE("Scheduler ready!\n");
			sched_yield();
		}
	}
	
	
	static void reap_threads()
	{
		Interrupts::irq_guard lock;
		
		for (auto id : threads_to_reap)
		{
			assert(__thread_map.count(id) > 0);
			auto thread = __thread_map[id];
			assert(thread);
			assert(thread->id == id);
			assert(thread->status == THREAD_STATUS_DONE);
			assert(id > 0);
			assert(thread->stack_size > 0);
			assert(thread->context);
			free(thread->context->stack.base);
			delete thread->context;
			thread->context = nullptr;
			
			
		}
		
		threads_to_reap.clear();
	}
}
}

//#ifdef TESTING

static volatile int main_already_saved = 0;

extern "C" int sched_yield()
{
	using namespace Kernel;
	assert(thread_it != runlist.end());
	auto old = *thread_it;
	++thread_it;
	if (thread_it == runlist.end())
	{
		if (runlist.size() > __thread_map.size())
		{
			assert(threads_to_reap.size() > 0);
		}
		thread_it = runlist.begin();
		if (threads_to_reap.size() > 0)
		{
			reap_threads();
		}
	}
	else if (threads_to_reap.size() > 10)
	{
		reap_threads();
	}
	
	auto next = *thread_it;

	if (next == old)
	{
		return -1;
	}
	
	
	
	{
	Interrupts::irq_guard irq_lock;
	

	int save_result = save_context(old->context);


	if (save_result != 0)
	{
		return 0;
	}

	main_already_saved = 1;



	
	}
	load_destroy_context(next->context, nullptr);
}

//#endif