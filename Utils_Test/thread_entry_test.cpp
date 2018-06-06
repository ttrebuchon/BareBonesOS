#ifdef TESTING
#include "Tests.hh"
#include <kernel/scheduling/context.h>
#include <kernel/Utility.hh>
#include <Utils/list>
#include <Utils/utility>
#include <Utils/type_traits>
#include <Utils/tuple>
#include <kernel/scheduling/thread.h>
#include <unistd.h>
#include <Utils/vector>


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


namespace Kernel
{
	extern "C" void scheduler_hook();
}

extern "C" bool signals_running();

const int timer = ITIMER_VIRTUAL;
const int sig = SIGUSR1;

sigset_t signal_toggle_set;
timer_t signal_timer;
struct itimerspec signal_timer_spec;

/*::stack_t sigstack
{
	.ss_sp = nullptr,
	.ss_flags = SS_DISABLE,
	.ss_size = ((4*SIGSTKSZ > 100*8192) ? 4*SIGSTKSZ : 100*8192),
};*/

::stack_t sigstack;

/*void block_signal()
{
	sigprocmask(SIG_BLOCK, &signal_toggle_set, nullptr);
}

void resume_signal()
{
	sigprocmask(SIG_UNBLOCK, &signal_toggle_set, nullptr);
}*/

static int cyvt = 0;



void thread_switch_inter(int val)
{
	std::cout << 'a';
	Kernel::scheduler_hook();
}

void thread_switch_timer(union sigval)
{
	std::cout << 'a';
	Kernel::scheduler_hook();
}


void setup_signaling()
{
	sigemptyset(&signal_toggle_set);
	sigaddset(&signal_toggle_set, sig);
	
	if (sigprocmask(SIG_UNBLOCK, &signal_toggle_set, nullptr) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	//block_signal();
	
	
	if (sigaltstack(nullptr, &sigstack) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	sigstack.ss_flags &= SS_DISABLE;
	
	if (sigaltstack(&sigstack, nullptr) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	
	struct sigevent event;
	
	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_notify_function = thread_switch_timer;
	event.sigev_signo = sig;
	
	if (timer_create(CLOCK_PROCESS_CPUTIME_ID, &event, &signal_timer) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	signal_timer_spec.it_interval.tv_sec = 0;
	signal_timer_spec.it_interval.tv_nsec = 1000000;
	
	signal_timer_spec.it_value.tv_sec = 0;
	signal_timer_spec.it_value.tv_nsec = 1000002;
	
	
	if (timer_settime(signal_timer, 0, &signal_timer_spec, nullptr) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	
	
	struct sigaction oldAct, newAct;
	newAct.sa_handler = &thread_switch_inter;
	
	auto oldHndl = signal(sig, &thread_switch_inter);
	assert(oldHndl != SIG_ERR);
	
	
	if (sigaction(sig, &newAct, &oldAct) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	oldHndl = signal(sig, &thread_switch_inter);
	assert(oldHndl != SIG_ERR);
	
	
	
	/*struct itimerval newInterval, oldInterval;
	
	
	if (getitimer(timer, &newInterval) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	
	newInterval.it_interval.tv_sec = 0;
	newInterval.it_interval.tv_usec = 1000;
	assert((long)newInterval.it_interval.tv_usec == 1000);
	newInterval.it_value.tv_usec = 1001;
	newInterval.it_value.tv_sec = 0;
	
	if (setitimer(timer, &newInterval, &oldInterval) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}*/
	
	
	//resume_signal();
	
	std::cout << "Setup complete." << std::endl;
}



void disable_signaling()
{
	//Kernel::Interrupts::irq_guard lock;
	//block_signal();
	
	
	if (timer_delete(signal_timer) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	/*constexpr int timer = ITIMER_VIRTUAL;
	constexpr int sig = SIGVTALRM;
	struct itimerval newInterval, oldInterval;
	
	
	if (getitimer(timer, &newInterval) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}
	
	
	newInterval.it_interval.tv_sec = 0;
	newInterval.it_interval.tv_usec = 0;
	newInterval.it_value.tv_usec = 0;
	newInterval.it_value.tv_sec = 0;
	
	if (setitimer(timer, &newInterval, &oldInterval) != 0)
	{
		auto e = errno;
		std::cout << strerror(e) << std::endl;
		assert(false);
	}*/
	
	//resume_signal();
}




extern "C"
void* __emutls_get_address(struct __emutls_object*);

void test_threads();

TEST(thread_entry)
{
	Kernel::init_kernel_threads();
	assert(sched_yield() == -1);
	//setup_signaling();
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

//#define YIELD sched_yield()
#define YIELD

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
		YIELD;
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
	
	constexpr int Multiplier = 1;
	
	
	for (int i = 0; i < 20; ++i)
	{
		pthread_t* id = new pthread_t;
		test_invokable inv(&test_thread_counter, id);
		
		Kernel::thread_create(id, inv);
	}
	
	std::cout << "Letting all threads run!\n" << std::endl;
	
	while (Kernel::thread_count() > 1)
	{
		//std::cout << "\n\nMain yielding...\n\n" << std::endl;
		YIELD;
	}
	
	for (int j = 0; j < 20*Multiplier; ++j)
	{
		pthread_t id;
		Kernel::thread_create(&id, [j]() -> void
		{
			pthread_t id_to_join;
			for (int i = 0; i < 10*Multiplier; ++i)
			{
				pthread_t* id2 = new pthread_t;
				Kernel::thread_create(id2, [i, id2]() -> void
				{
					int m = 0;
					std::cout << "Inner Thread: " << *id2 << ": " << i << "\n";
					for (int k = 0; k < ((m % (i+1)) + 2); ++k)
					{
						m += k;
						YIELD;
						
						pthread_t* id4 = new pthread_t;
						test_invokable inv(&test_thread_counter, id4);
						Kernel::thread_create(id4, inv);
						
						
					}
					std::cout << *id2 << ": " << i << " -> " << m << "\n";
					if (i != 0)
					{
						
						delete id2;
					}
				});
				if (i == 0)
				{
					Kernel::thread_join(*id2, nullptr);
					delete id2;
				}
				
				if ((i+1) % ((j % 10)+1) == 0)
				{
					YIELD;
				}
			}
		});
	}
	
	int max_count = 0;
	
	std::cout << "Letting all threads run!\n" << std::endl;
	
	while (Kernel::thread_count() > 1)
	{
		if (Kernel::thread_count() > max_count)
		{
			max_count = Kernel::thread_count();
		}
		
		//std::cout << "\n\nMain yielding...\n\n" << std::endl;
		YIELD;
	}
	
	std::cout << "Most seen at once: " << max_count << std::endl;
	
	std::cout << std::string(20, '\n') << std::endl;
	
	
	Kernel::Interrupts::cli();
	for (int i = 0; i < 10; ++i)
	{
		auto tid = new pthread_t;
		test_invokable inv(&test_thread_counter, tid);
		Kernel::thread_create(tid, inv);
		auto tid2 = new pthread_t;
		Kernel::thread_create(tid2, [](pthread_t* t)
		{
			volatile int x = 0;
			long last = 0;
			while (true)
			{
				__sync_synchronize();
				++x;
				if (x % 100000 == 0)
				{
					std::cout << x << "\n";
				}
				//assert(signals_running());
				
				
				
				
				break;
			}
		}, tid2);
	}
	
	Kernel::Interrupts::sti();
	
	
	
	while (Kernel::thread_count() > 1)
	{
		std::cout << "Main waiting!!" << std::endl;
		assert(Kernel::Interrupts::block_interrupt_counter() == 0);
		sched_yield();
		//while (Kernel::thread_count() > 1) ;
	}
	
	
	int counter = 0;
	Utils::vector<pthread_t> ids;
	ids.reserve(100*Multiplier);
	
	for (int i = 0; i < 100*Multiplier; ++i)
	{
		pthread_t id;
		Kernel::thread_create(&id, [](int* p) -> void
		{
			__atomic_fetch_add(p, 1, __ATOMIC_SEQ_CST);
			
		}, &counter);
		ids.push_back(id);
	}
	
	for (int i = 0; i < ids.size(); ++i)
	{
		//while (Kernel::thread_count() > 1);
		Kernel::thread_join(ids[i], nullptr);
	}
	
	ids.clear();
	
	ASSERTEQ(counter, 100*Multiplier);
	assert(Kernel::thread_count() == 1);
	
}
#endif