#include "Tests.hh"
#include <kernel/Memory/Allocators/choice_allocator.hh>
#include <kernel/scheduling/thread.h>
#include <thread>
#include <Utils/condition_variable>

extern "C" {

static void* fooghv(void** ptr) __attribute__((__nonnull__(1)));

static void* fooghv(void** ptr)
{
	return (void*)((addr_t)*ptr + 1);
}

}

TEST(choice_alloc)
{
	using namespace Kernel::Memory;
	
	{
		Utils::condition_variable cv;
		bool woken = false;
		
		auto wake_func = [&cv, &woken]()
		{
			using namespace std::chrono_literals;
			do
			{
				std::this_thread::sleep_for(50ms);
				cv.notify_one();
				sched_yield();
				__sync_synchronize();
			}
			while (!woken);
			
		};
		
		auto broadcast_func = [&cv, &woken]()
		{
			using namespace std::chrono_literals;
			do
			{
				std::this_thread::sleep_for(50ms);
				cv.notify_all();
				sched_yield();
				__sync_synchronize();
			}
			while (!woken);
		};
		
		{
			Utils::mutex mut;
			{
				Utils::unique_lock<Utils::mutex> lock(mut);
				pthread_t tid;
				
				woken = false;
				__sync_synchronize();
				Kernel::thread_create(&tid, wake_func);
				
				cv.wait(lock);
				woken = true;
				__sync_synchronize();
				QA::out << "Woke by signal. Joining..." << std::endl;
				Kernel::thread_join(tid, nullptr);
				QA::out << "Joined." << QA::br;
				
				
				
				woken = false;
				__sync_synchronize();
				Kernel::thread_create(&tid, wake_func);
				
				cv.wait(lock);
				woken = true;
				__sync_synchronize();
				QA::out << "Woke by signal. Joining..." << std::endl;
				Kernel::thread_join(tid, nullptr);
				QA::out << "Joined." << QA::br;
				
				
				
				woken = false;
				__sync_synchronize();
				Kernel::thread_create(&tid, broadcast_func);
				cv.wait(lock);
				woken = true;
				__sync_synchronize();
				QA::out << "Woke by broadcast. Joining..." << std::endl;
				Kernel::thread_join(tid, nullptr);
				QA::out << "Joined." << QA::br;
			}
			
			const int tcount = 10000;
			
			{
				int counter = 0;
				pthread_t threads[tcount];
				auto waiter_func = [&counter](Utils::condition_variable* cv, Utils::mutex* mut)
				{
					{
						Utils::unique_lock<Utils::mutex> lock(*mut);
					cv->wait(lock);
					__sync_synchronize();
					++counter;
					cv->notify_one();
					}
					
				};
				
				Utils::condition_variable cv;
				
				for (int i = 0; i < tcount; ++i)
				{
					
					Kernel::thread_create(&threads[i], waiter_func, &cv, &mut);
				}
				
				sched_yield();
				QA::out << "Notifying first..." << std::endl;
				cv.notify_one();
				for (int i = 0; i < tcount; ++i)
				{
					Kernel::thread_join(threads[i], nullptr);
				}
				assert(counter == tcount);
				
			}
			
			
			
			{
				int counter = 0;
				pthread_t threads[tcount];
				auto waiter_func = [&counter](Utils::condition_variable* cv, Utils::mutex* mut)
				{
					{
						Utils::unique_lock<Utils::mutex> lock(*mut);
					cv->wait(lock);
					__sync_synchronize();
					++counter;
					}
					
				};
				
				Utils::condition_variable cv;
				
				QA::out << "Creating threads..." << std::endl;
				
				for (int i = 0; i < tcount; ++i)
				{
					
					Kernel::thread_create(&threads[i], waiter_func, &cv, &mut);
				}
				sched_yield();
				QA::out << "Notifying all..." << std::endl;
				cv.notify_all();
				QA::out << "Joining all..." << std::endl;
				
				for (int i = 0; i < tcount; ++i)
				{
					Kernel::thread_join(threads[i], nullptr);
				}
				assert(counter == tcount);
				
				QA::out << "Joined." << QA::br;
			}
		}
		choice_allocator<int, Utils::allocator<void>> alloc;
		
		
		auto iptr_4 = alloc.allocate(4);
		
		for (int i = 0; i < 4; ++i)
		{
			alloc.construct(iptr_4 + i);
		}
		
		for (int i = 0; i < 4; ++i)
		{
			alloc.destroy(iptr_4 + i);
		}
		
		alloc.deallocate(iptr_4, 4);
		
		//fooghv(NULL);
	}
}