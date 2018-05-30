#include "Tests.hh"
#include <kernel/scheduling/context.h>
#include <kernel/Utility.hh>
#include <Utils/list>
#include <Utils/utility>
#include <Utils/type_traits>
#include <Utils/tuple>
#include <kernel/scheduling/thread.h>
#include <unistd.h>


using Utils::list;

/*static list<Kernel::context_t*> jobs;
static typename list<Kernel::context_t*>::iterator job_it = jobs.begin();
static volatile long tid_count = 0;


template <class T, class R = void>
void entry_unpack(void* _data, const void* _fn)
{
	typedef R(*fn_type)(T*);
	fn_type fn = (fn_type)_fn;
	T* data = (T*)_data;
	fn(data);
}

extern "C" {
	
struct thread_init_t
{
	void(*destroy)(thread_init_t*);
	void* data;
	void* fn;
	Kernel::context_t thread_context;
};
	
struct thread_entry_t
{
	thread_init_t* init;
	void(*enter)(thread_init_t*);
	Kernel::context_t return_context;
};

thread_local int ggghhww;

__attribute__((__noreturn__))
void thread_exit()
{
	assert(job_it != jobs.end());
	auto context = *job_it;
	assert(context);
	//delete context;
	job_it = jobs.erase(job_it);
	if (job_it == jobs.end())
	{
		job_it = jobs.begin();
	}
	assert(job_it != jobs.end());
	load_context(*job_it);
}

}

template <class T, class R = void>
void thread_entry_unpack(thread_init_t* init)
{
	typedef R(*fn_type)(T*);
	((fn_type)init->fn)((T*)init->data);
}

extern "C" 
void thread_entry_func(thread_entry_t* entry)
{
	assert(entry);
	thread_init_t* init = entry->init;
	assert(init);
	assert(entry->enter);
	entry->enter(init);
	if (init->destroy)
	{
		void(*destroy)(thread_init_t*) = init->destroy;
		destroy(init);
	}
	
	std::cout << "Exiting thread." << std::endl;
	thread_exit();
	//Kernel::load_context(&entry->return_context);
	
	__builtin_unreachable();
	
}

extern "C" {
	
void thread_yield()
{
	assert(job_it != jobs.end());
	Kernel::context_t* oContext = *job_it;
	assert(oContext);
	assert(*job_it == oContext);
	assert(*job_it != (void*)1);
	std::cout << "Switching from " << (void*)oContext << std::endl;
	
	++job_it;
	if (job_it == jobs.end())
	{
		job_it = jobs.begin();
	}
	
	Kernel::context_t* nContext = *job_it;
	assert(nContext);
	
	if (oContext == nContext)
	{
		std::cout << "No additional contexts to switch to.\n" << std::endl;
		return;
	}
	
	std::cout << "Switching to " << (void*)nContext << std::endl;
	
	if (save_context(oContext) == 1)
	{
		std::cout << "Resumed " << oContext << std::endl;
		return;
	}
	
	std::cout << "Switching..." << std::endl;
	
	load_context(nContext);
}

void init_threads()
{
	auto thisContext = new Kernel::context_t;
	jobs.push_back(thisContext);
	job_it = jobs.begin();
	save_context(thisContext);
	tid_count++;
	std::cout << "Main context: " << thisContext << std::endl;
}
	
}




void test_do_stuff(int* i)
{
	std::cout << __func__ << std::endl;
	std::cout << "i: " << i << std::endl;
	assert(i);
	std::cout << __func__ << " : *i = " << *i << std::endl;
	ggghhww = 22;
	
	for (int i = 0; i < 5; ++i)
	{
		thread_yield();
	}
}



template <class T, class R = void>
bool setup_thread(thread_entry_t* entry, T* data, R(*func)(T*))
{
	assert(entry);
	typedef R(*fn_type)(T*);
	const size_t stack_size = 16*1000;
	auto nstack = malloc(stack_size);
	assert(((addr_t)nstack) % 16 == 0);
	ptrdiff_t off = sizeof(T*) + sizeof(fn_type);
	assert(off % 16 == 0);
	T** arg1_addr = (T**)(void*)(((addr_t)nstack) + stack_size - sizeof(T*));
	fn_type* arg2_addr = (fn_type*)(void*)(((addr_t)nstack) + stack_size - off);
	*arg1_addr = data;
	*arg2_addr = func;
	
	thread_init_t* init = new thread_init_t;
	entry->init = init;
	init->data = data;
	init->fn = (void*)func;
	init->thread_context.ip = (void*)&thread_entry_func;
	init->thread_context.registers.x0 = (addr_t)entry;
	entry->enter = &thread_entry_unpack<T, R>;
	entry->init->thread_context.stack.base = nstack;
	
	init->destroy = [](auto init)
	{
		delete init;
	};
	
	init->thread_context.stack.sp = (void*)((addr_t)nstack + stack_size);
	init->thread_context.stack.fp = nullptr;
	
	if (save_context(&entry->return_context) == 1)
	{
		return true;
	}
	
	jobs.push_back(&init->thread_context);
	if (job_it == jobs.end())
	{
		job_it = jobs.begin();
	}
	
	//thread_yield();
	
	return true;
}

template <class Fn, class... Args>
long start_thread(Fn fn, Args... args)
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
	
	void(*unpacker)(Thread_Args*) = [](Thread_Args* args)
	{
		args->fn();
	};
	
	auto id = tid_count++;
	auto entry = new thread_entry_t;
	setup_thread(entry, args_pack, unpacker);
	
	std::cout << __func__ << " : setup complete" << std::endl;
	
	return id;
}*/



extern "C"
void* __emutls_get_address(struct __emutls_object*);

void test_threads();

TEST(thread_entry)
{
	Kernel::init_kernel_threads();
	assert(sched_yield() == -1);
	test_threads();
	return;
	
	/*init_threads();
	test_threads();
	return;
	thread_entry_t tEnter;
	
	
	ggghhww = 21;
	
	
	int* i = new int(4);
	std::cout << "i: " << i << std::endl;
	setup_thread(&tEnter, i, &test_do_stuff);
	
	//thread_yield();
	
	for (int i = 0; i < 20; ++i)
	{
		thread_yield();
	}
	test_threads();
	return;
	
	if (save_context(&tEnter.return_context) == 1)
	{
		for (int i = 0; i < 100; ++i)
		{
			thread_yield();
		}
		
		return;
	}
	
	//ptrdiff_t off = 2*sizeof(unsigned int);
	
	void* x = 0;
	asm volatile ("adrp %0, :got:__emutls_v.ggghhww" : "=r"(x));
	void* off;
	asm volatile("ldr %0, [%1, #:got_lo12:__emutls_v.ggghhww]" : "=r"(off) : "r"(x));
	
	
	auto xobj = (struct __emutls_object*)off;
	auto xaddr = __emutls_get_address(xobj);
	assert(xaddr == &ggghhww);
	
	std::cout << "Entering thread." << std::endl;
	
	Kernel::load_context(&tEnter.init->thread_context);
	*/
	
}


static int test_thread_counter = 0;

class test_invokable
{
	private:
	int* ptr;
	long* tid;
	
	public:
	test_invokable(int* ptr, long* tid) : ptr(ptr), tid(tid)
	{
		
	}
	
	
	void operator()()
	{
		std::cout << "Thread " << *tid << ": " << (*ptr)++ << std::endl;
		sched_yield();
		std::cout << "Thread " << *tid << " (2): " << (*ptr)++ << std::endl;
	}
};


/*void test_threads()
{
	std::cout << std::string(20, '\n') << __func__ << "\n" << std::endl;
	
	
	for (int i = 0; i < 20; ++i)
	{
		long* id = new long;
		test_invokable inv(&test_thread_counter, id);
		*id = start_thread(inv);
	}
	
	while (jobs.size() > 1)
	{
		std::cout << "Main yielding..." << std::endl;
		thread_yield();
	}
}*/

void test_threads()
{
	std::cout << std::string(20, '\n') << __func__ << "\n" << std::endl;
	
	
	for (int i = 0; i < 20; ++i)
	{
		pthread_t* id = new pthread_t;
		test_invokable inv(&test_thread_counter, id);
		
		Kernel::thread_create(id, inv);
	}
	
	std::cout << "Letting all threads run!\n" << std::endl;
	
	while (Kernel::thread_count() > 1)
	{
		std::cout << "\n\nMain yielding...\n\n" << std::endl;
		sched_yield();
	}
	
	for (int j = 0; j < 20; ++j)
	{
		pthread_t id;
		Kernel::thread_create(&id, [j]() -> void
		{
			for (int i = 0; i < 10; ++i)
			{
				pthread_t* id2 = new pthread_t;
				Kernel::thread_create(id2, [i, id2]() -> void
				{
					int m = 0;
					std::cout << "Inner Thread: " << *id2 << ": " << i << "\n";
					for (int k = 0; k < ((m % (i+1)) + 2); ++k)
					{
						m += k;
						sched_yield();
						
						pthread_t* id4 = new pthread_t;
						test_invokable inv(&test_thread_counter, id4);
						Kernel::thread_create(id4, inv);
						
					}
					std::cout << *id2 << ": " << i << " -> " << m << "\n";
					delete id2;
				});
				
				if ((i+1) % ((j % 10)+1) == 0)
				{
					sched_yield();
				}
			}
		});
	}
	
	int max_count = 0;
	
	while (Kernel::thread_count() > 1)
	{
		if (Kernel::thread_count() > max_count)
		{
			max_count = Kernel::thread_count();
		}
		
		std::cout << "\n\nMain yielding...\n\n" << std::endl;
		sched_yield();
	}
	
	std::cout << "Most seen at once: " << max_count << std::endl;
	
}