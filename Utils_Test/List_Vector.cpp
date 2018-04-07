#include <Utils/vector>
#include <Utils/list>
#include "Tests.hh"
#include <vector>
#include <list>


template <template <class...> class collection, class T, class F>
static void do_tests(const int n, F);


template <class T = int, class F = void(*)()>
void run_for(const int n, F f = F());



TEST(list_vector)
{
	auto barReset = []()
	{
		Bar_t::Reset();
		Bar_t::Check();
	};
	
	run_for<long long>(10000);
	//run_for(100000, 7);
	//run_for(100000, 10);
	
	Bar_t::SetPrinting(false);
	//run_for<int, Bar_t>(10000, 3, barReset);
	Bar_t::SetPrinting(true);
	
	enum E
	{
		E1,
		E2,
		E3
	};
	
	run_for<E>(100);
}








template <class T, class F>
void run_for(const int n, F f)
{
	
	std::clog << "Testing std::vector..." << std::endl;
	do_tests<std::vector, T, F>(n, f);
	
	std::clog << "Testing Utils::vector..." << std::endl;
	do_tests<Utils::vector, T, F>(n, f);
	
	std::clog << "Testing std::list..." << std::endl;
	do_tests<std::list, T, F>(n, f);
	
	std::clog << "Testing Utils::list..." << std::endl;
	do_tests<Utils::list, T, F>(n, f);
}






template <template <class...> class collection, class T, class F>
static void do_tests(const int n, F f)
{
	
	typedef collection<T> M;
	
	
	if (f)
	{
		f();
	}
	std::clog << "--------------\n" << std::endl;
	{
		M m;
		
		for (int i = 0; i < n; ++i)
		{
			m.push_back(T(i));
			MASSERTEQ(m.size(), i+1, i);
			//MASSERTEQ(m[positions.at(i)], T(i), i, positions.at(i));
		}
		
		ASSERTEQ(m.size(), n);
		
		M m2 = m;
		
		ASSERTEQ(m.size(), n);
		ASSERTEQ(m2.size(), n);
		
		{
			int i = 0;
			for (auto& e : m2)
			{
				e = T(100-i);
				++i;
			}
		}
		
		ASSERTEQ(m.size(), n);
		ASSERTEQ(m2.size(), n);
		
		{
			int i = 0;
			for (auto& e : m)
			{
				ASSERTEQ(e, T(i));
				++i;
			}
		}
		
		ASSERTEQ(m.size(), n);
		ASSERTEQ(m2.size(), n);
		
		
		{
			int i = 0;
			for (auto& e : m2)
			{
				ASSERTEQ(e, T(100-i));
				++i;
			}
		}
		
		ASSERTEQ(m.size(), n);
		ASSERTEQ(m2.size(), n);
	}
}