#ifdef TESTING
#include <Utils/functional>
#include "Tests.hh"
#include <functional>
#include <Utils/Allocator.hh>


class Invokable
{
	public:
	long n;
	
	
	~Invokable()
	{
		std::clog << "~Invokable{" << n << "}()" << std::endl;
	}
	
	
	long operator()() const
	{
		std::clog << "Invokable{" << n << "}()" << std::endl;
		return n;
	}
	
	long operator()(long x)
	{
		long old = n;
		n = x;
		std::clog << "Invokable{n=" << n << "}()" << std::endl;
		return old;
	}
};




template <template <class...> class function, class AA, class T, class F>
void do_tests(const int n, F);

template <class T = int, class F = void(*)()>
void run_for(const int n, F f = F());

TEST(function)
{
	{
		typedef typename Utils::function<int(int, float)>::first_argument_type t1;
		
		typedef typename Utils::function<int(int, float)>::second_argument_type t2;
		
		typedef typename Utils::function<int(int)>::argument_type t3;
		
	}
	
	run_for(10000);
}


template <class T, class F>
void run_for(const int n, F f)
{
	std::cout << "Running std (" << n << ")..." << std::endl;
	do_tests<std::function, std::allocator_arg_t, T, F>(n, f);
	
	
	std::cout << "Running Utils (" << n << ")..." << std::endl;
	do_tests<Utils::function, Utils::allocator_arg_t, T, F>(n, f);
}


template <template <class...> class function, class AA, class T, class F>
void do_tests(const int n, F fn)
{
	std::clog << "--------------\n" << std::endl;
	{
		int x = 0;
		ASSERTEQ(x, 0);
		function<void(int)> f = [&x](int)
		{
			x = 4;
		};
		
		ASSERTEQ(x, 0);
		f(0);
		ASSERTEQ(x, 4);
	}
	
	std::clog << "--------------\n" << std::endl;
	{
		int x = 0;
		ASSERTEQ(x, 0);
		function<void(int)> f(AA(), std::allocator<int>(), [&x](int)
		{
			x = 4;
		});
		
		ASSERTEQ(x, 0);
		f(0);
		ASSERTEQ(x, 4);
	}
	
	
	std::clog << "--------------\n" << std::endl;
	{
		Foo_t in{6};
		ASSERTEQ(in.n, 6);
		function<long()> fc(in);
		
		#ifdef __cpp_rtti
		{
			Foo_t* in_ptr1 = fc.template target<Foo_t>();
			ASSERT(in_ptr1 != nullptr);
			ASSERTEQ(in_ptr1->n, 6);
			int* ptr = fc.template target<int>();
			ASSERTEQ(ptr, nullptr);
			std::clog << "Checked.\n";
		}
		#endif
		ASSERTEQ(fc(), 6);
		
		function<long(long)> f(in);
		ASSERTEQ(f(4), 6);
		ASSERTEQ(f(5), 4);
		
		function<long(long)> f2 = f;
		ASSERTEQ(f2(10), 5);
		ASSERTEQ(f(10), 5);
		
		ASSERTEQ(f2(0), 10);
		ASSERTEQ(f(9), 10);
		
		ASSERTEQ(f(0), 9);
		ASSERTEQ(f2(-1), 0);
	}
	
	
	
	std::clog << "--------------\n" << std::endl;
}
#endif