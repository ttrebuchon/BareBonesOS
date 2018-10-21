#include "scheduler.h"
#include "thread.h"

namespace Kernel
{
	Scheduler* Scheduler::Current __attribute__((section("k_globals"))) = nullptr;
	
	
	Scheduler::Scheduler() : spin_lock(),
				__thread_map(),
				runlist(),
				thread_it(),
				waiting_threads(),
				threads_to_reap(),
				pid_counter(1)
	{
		
	}
	
	
	// Haven't implemented
	// current_thread() yet
	/*pthread_t Scheduler::current_thread_id() const noexcept
	{
		auto t = current_thread();
		assert(t);
		return t->id;
	}*/
	
	
	
	
	
	
	
}