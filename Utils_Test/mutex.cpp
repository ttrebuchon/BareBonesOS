#include <Utils/mutex>
#include <mutex>
#include "Tests.hh"
#include <future>
#include <Support/threading/SpinMutex.hh>

template <class mutex, class T, class L, class LF, class F>
static void do_tests(const int n, const LF locker, const F f);

template <class mutex, class T, template <class...> class L, class LF, class F>
static void do_tests(const int n, const LF locker, const F f)
{
	do_tests<mutex, T, L<mutex>, LF, F>(n, locker, f);
}


template <class T = int, class F = void(*)()>
void run_for(const int n, F f = F());


TEST(mutex)
{
	run_for<Foo_t>(50);
}





template <class T, class F>
void run_for(const int n, F f)
{
	
	auto stdLock = [](auto&... muts) -> void
	{
		std::lock(muts...);
	};
	
	std::clog << "Testing std..." << std::endl;
	do_tests<std::mutex, T, std::lock_guard>(n, stdLock, f);
	
	std::clog << "Testing Utils..." << std::endl;
	do_tests<Support::threading::spin_mutex, T, std::lock_guard>(n, stdLock, f);
	
	
}




template <class mutex, class T, class Lock, class LF, class F>
static void do_tests(const int n, const LF locker, const F f)
{
	mutex m;
	mutex m2;
	locker(m, m2);
	m.unlock();
	//locker(m);
	{
		Lock lock(m);
	}
	m2.unlock();
	{
		locker(m, m2);
		Lock lock(m, std::adopt_lock);
		Lock lock2(m2, std::adopt_lock);
	}
	
	m.lock();
	m2.lock();
	m.unlock();
	m2.unlock();
	
	
	
	{
		volatile int x = 0;
		auto func = [&]()
		{
			Lock lock(m);
			++x;
			ASSERTEQ(x, 1);
		};
		
		m.lock();
		auto fut = std::async(std::launch::async, func);
		fut.wait_for(std::chrono::milliseconds(100));
		ASSERTEQ(x, 0);
		ASSERTEQ(x, 0);
		m.unlock();
		fut.wait_for(std::chrono::milliseconds(100));
		m.lock();
		ASSERTEQ(x, 1);
		m.unlock();
		fut.get();
	}
	
	
	
	{
		volatile int x = 1;
		auto func = [&]()
		{
			m.lock();
			if (x % 2 == 0)
			{
				++x;
			}
			else
			{
				--x;
			}
			auto y = x;
			ASSERT(!m.try_lock());
			m.unlock();
			Lock lock(m);
			if (y % 2 == 0)
			{
				++x;
			}
			else
			{
				--x;
			}
		};
		
		m.lock();
		std::vector<std::future<void>> futs(n);
		std::clog << "Starting threads (" << n << ")...\n";
		for (int i = 0; i < n; ++i)
		{
			futs[i] = std::async(std::launch::async, func);
		}
		std::clog << "Started.\n";
		ASSERTEQ(x, 1);
		m.unlock();
		for (auto& fut : futs)
		{
			fut.get();
		}
		m.lock();
		ASSERTEQ(x, 1);
		m.unlock();
	}
}