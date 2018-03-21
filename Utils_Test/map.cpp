#include "Tests.hh"
#include <Utils/map>
//#include "Tests.hh"
#include <map>

template <template <class...> class Type, class T, class F>
static void do_tests(const int n, const std::vector<int>& positions, F);


template <class T = int, class F = void(*)()>
void run_for(const int n, const int shuffles, F f = F());


TEST(map)
{
	srand(time(NULL));
	
	auto barReset = []()
	{
		Bar_t::Reset();
		Bar_t::Check();
	};
	
	run_for(10000, 3);
	//run_for(100000, 7);
	//run_for(100000, 10);
	
	Bar_t::SetPrinting(false);
	run_for<Bar_t>(10000, 3, barReset);
	Bar_t::SetPrinting(true);
}




template <class T, class F>
void run_for(const int n, const int shuffles, F f)
{
	std::vector<int> positions(n);
	for (int i = 0; i < n; ++i)
	{
		positions[i] = i;
	}
	
	std::clog << "Testing std..." << std::endl;
	do_tests<std::map, T, F>(n, positions, f);
	
	std::clog << "Testing Utils..." << std::endl;
	do_tests<Utils::map, T, F>(n, positions, f);
	
	if (shuffles <= 0)
	{
		return;
	}
	
	std::clog << "Shuffling...\n";
	for (int i = 0; i < shuffles; ++i)
	{
		std::random_shuffle(positions.begin(), positions.end());
	}
	
	std::clog << "Testing std..." << std::endl;
	do_tests<std::map, T, F>(n, positions, f);
	
	std::clog << "Testing Utils..." << std::endl;
	do_tests<Utils::map, T, F>(n, positions, f);
}






template <template <class...> class map, class T, class F>
static void do_tests(const int n, const std::vector<int>& positions, F f)
{
	std::clog << "--------------\n" << std::endl;
	{
		map<int, T> m;
		m[0] = T(0);
		ASSERTEQ(m.size(), 1);
		ASSERTEQ(m[0], T(0));
		m[1] = T(4);
		ASSERTEQ(m.size(), 2);
		ASSERTEQ(m[1], T(4));
		m[0] = T(1);
		ASSERTEQ(m.size(), 2);
		ASSERTEQ(m[0], T(1));
		m[20] = T(-1);
		ASSERTEQ(m.size(), 3);
		ASSERTEQ(m[20], T(-1));
	}
	
	
	Bar_t::Reset();
	Bar_t::Check();
	std::clog << "--------------\n" << std::endl;
	{
		map<int, Bar_t> m;
		m[0] = Bar_t(0);
		ASSERTEQ(Bar_t::count, 1);
		ASSERTEQ(Bar_t::callers.size(), 0);
		ASSERT(Bar_t::Check(1, 0));
		
		m[1].n = 4;
		ASSERTEQ(m.size(), 2);
	}
	ASSERTEQ(Bar_t::count, 0);
	ASSERTEQ(Bar_t::callers.size(), 2);
	ASSERT(Bar_t::Check(0, 2));
	ASSERTEQ(Bar_t::callers[0], 4);
	
	
	Bar_t::Reset();
	Bar_t::Check();
	std::clog << "--------------\n" << std::endl;
	{
		map<int, Bar_t> m;
		m[0] = Bar_t(0);
		ASSERTEQ(Bar_t::count, 1);
		ASSERTEQ(Bar_t::callers.size(), 0);
		ASSERT(Bar_t::Check(1, 0));
		
		m[1].n = 4;
		ASSERTEQ(m.size(), 2);
	}
	ASSERTEQ(Bar_t::count, 0);
	ASSERTEQ(Bar_t::callers.size(), 2);
	ASSERT(Bar_t::Check(0, 2));
	ASSERTEQ(Bar_t::callers[0], 4);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	if (f)
	{
		f();
	}
	std::clog << "--------------\n" << std::endl;
	{
		map<int, T> m;
		
		for (int i = 0; i < n; ++i)
		{
			m[positions.at(i)] = T(i);
			MASSERTEQ(m.size(), i+1, i);
			MASSERTEQ(m[positions.at(i)], T(i), i, positions.at(i));
		}
		
		ASSERTEQ(m.size(), n);
		
		for (int i = 0; i < n; ++i)
		{
			MASSERTEQ(m[positions.at(i)], T(i), i, positions.at(i));
		}
		
		map<int, T> m2 = m;
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m2[positions.at(i)], T(i));
		}
		
		for (int i = 0; i < n; ++i)
		{
			m2[positions.at(i)] = T(100 - i);
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m[positions.at(i)], T(i));
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m2[positions.at(i)], T(100 - i));
		}
	}
	
	
	std::clog << "--------------\n" << std::endl;
	{
		map<int, long> m;
		
		for (int i = 0; i < n; ++i)
		{
			m[positions.at(i)] = i;
		}
		
		ASSERTEQ(m.size(), n);
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m.at(positions.at(i)), i);
		}
		
		map<int, long> m2 = m;
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m2.at(positions.at(i)), i);
		}
		
		for (int i = 0; i < n; ++i)
		{
			m2[positions.at(i)] = 100 - i;
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m.at(positions.at(i)), i);
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m2.at(positions.at(i)), 100 - i);
		}
	}
	
	
	std::clog << "--------------\n" << std::endl;
	{
		map<int, long> m;
		const map<int, long>& mc = m;
		
		for (int i = 0; i < n; ++i)
		{
			m[positions.at(i)] = i;
		}
		
		ASSERTEQ(m.size(), n);
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc.at(positions.at(i)), i);
		}
		
		map<int, long> m2 = m;
		const map<int, long>& mc2 = m2;
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc2.at(positions.at(i)), i);
		}
		
		for (int i = 0; i < n; ++i)
		{
			m2[positions.at(i)] = 100 - i;
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc.at(positions.at(i)), i);
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc2.at(positions.at(i)), 100 - i);
		}
	}
	
	
	
	
	std::clog << "--------------\n" << std::endl;
	{
		map<int, long> m;
		const map<int, long>& mc = m;
		
		for (int i = 0; i < n; ++i)
		{
			m[positions.at(i)] = i;
		}
		
		ASSERTEQ(m.size(), n);
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc.count(positions.at(i)), 1);
			ASSERTEQ(m.count(positions.at(i)), 1);
			ASSERTEQ(mc.at(positions.at(i)), i);
		}
		
		map<int, long> m2 = m;
		const map<int, long>& mc2 = m2;
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc2.count(positions.at(i)), 1);
			ASSERTEQ(m2.count(positions.at(i)), 1);
			ASSERTEQ(mc2.at(positions.at(i)), i);
		}
		
		for (int i = 0; i < n; ++i)
		{
			m2[positions.at(i)] = 100 - i;
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc.count(positions.at(i)), 1);
			ASSERTEQ(m.count(positions.at(i)), 1);
			ASSERTEQ(mc.at(positions.at(i)), i);
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc2.count(positions.at(i)), 1);
			ASSERTEQ(m2.count(positions.at(i)), 1);
			ASSERTEQ(mc2.at(positions.at(i)), 100 - i);
		}
		
		
		
		for (auto it = m2.begin(); it != m2.end(); ++it)
		{
			
		}
	}
	
	
	{
		map<int, long> m;
		for (int i = 0; i < n; ++i)
		{
			m[positions.at(i)] = positions.at(i);
		}
		
		int x = 0;
		for (auto it = m.begin(); it != m.end(); ++it)
		{
			ASSERTEQ(it->second, x);
			++x;
		}
		
		ASSERTEQ(x, n);
		
		m.clear();
		ASSERT(m.begin() == m.end());
	}
}