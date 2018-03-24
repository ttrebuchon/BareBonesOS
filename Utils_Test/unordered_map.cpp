#include "Tests.hh"
#include <Utils/unordered_map>
//#include "Tests.hh"
#include <unordered_map>
#include <fstream>



template <template <class...> class map, class T, class F, template <class> class H, class K = int>
static void do_tests(const int n, const std::vector<K>& positions, F);


template <class K = int, class T = int, class F = void(*)()>
void run_for(const int n, const int shuffles, F f = F());

template<template <class...> class map>
void misc_test(size_t n);


TEST(unordered_map)
{
	srand(time(NULL));
	
	auto barReset = []()
	{
		Bar_t::Reset();
		Bar_t::Check();
	};
	
	misc_test<std::unordered_map>(10);
	//return;
	
	run_for<long long, long>(10000, 3);
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
	
	run_for<E>(10000, 3);
}




template <class K, class T, class F>
void run_for(const int n, const int shuffles, F f)
{
	std::vector<K> positions(n);
	for (int i = 0; i < n; ++i)
	{
		positions[i] = K(i);
	}
	
	std::clog << "Testing std..." << std::endl;
	do_tests<std::unordered_map, T, F, std::hash, K>(n, positions, f);
	
	/*std::clog << "Testing std (Utils hash)..." << std::endl;
	do_tests<std::unordered_map, T, F, Utils::hash, K>(n, positions, f);*/
	
	std::clog << "Testing Utils..." << std::endl;
	do_tests<Utils::unordered_map, T, F, Utils::hash, K>(n, positions, f);
	
	std::clog << "Testing Utils (std hash)..." << std::endl;
	do_tests<Utils::unordered_map, T, F, std::hash, K>(n, positions, f);
	
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
	do_tests<std::unordered_map, T, F, std::hash, K>(n, positions, f);
	
	/*std::clog << "Testing std (Utils hash)..." << std::endl;
	do_tests<std::unordered_map, T, F, Utils::hash, K>(n, positions, f);*/
	
	std::clog << "Testing Utils..." << std::endl;
	do_tests<Utils::unordered_map, T, F, Utils::hash, K>(n, positions, f);
	
	std::clog << "Testing Utils (std hash)..." << std::endl;
	do_tests<Utils::unordered_map, T, F, std::hash, K>(n, positions, f);
}






template <template <class...> class map, class T, class F, template <class> class _H, class K>
static void do_tests(const int n, const std::vector<K>& positions, F f)
{
	typedef _H<K> H;
	
	typedef map<K, T, H> M;
	
	
	std::clog << "--------------\n" << std::endl;
	{
		M m;
		//m.emplace(K(0), T(0));
		m[K(0)] = T(0);
		ASSERTEQ(m.size(), 1);
		ASSERTEQ(m[K(0)], T(0));
		if (m[K(0)] != T(0))
		{
			std::cout << "ERROR!!!" << std::endl;
		}
		m[K(1)] = T(4);
		ASSERTEQ(m.size(), 2);
		ASSERTEQ(m[K(1)], T(4));
		m[K(0)] = T(1);
		ASSERTEQ(m.size(), 2);
		ASSERTEQ(m[K(0)], T(1));
		m[K(20)] = T(-1);
		ASSERTEQ(m.size(), 3);
		ASSERTEQ(m[K(20)], T(-1));
		
		ASSERTEQ(m.at(K(20)), T(-1));
	}
	
	
	Bar_t::Reset();
	Bar_t::Check();
	std::clog << "--------------\n" << std::endl;
	{
		map<K, Bar_t, H> m;
		m[K(0)] = Bar_t(0);
		ASSERTEQ(Bar_t::count, 1);
		ASSERTEQ(Bar_t::callers.size(), 0);
		ASSERT(Bar_t::Check(1, 0));
		
		m[K(1)].n = 4;
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
		map<K, Bar_t, H> m;
		m[K(0)] = Bar_t(0);
		ASSERTEQ(Bar_t::count, 1);
		ASSERTEQ(Bar_t::callers.size(), 0);
		ASSERT(Bar_t::Check(1, 0));
		
		m[K(1)].n = 4;
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
		M m;
		
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
		
		M m2 = m;
		
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
		map<K, long, H> m;
		
		for (int i = 0; i < n; ++i)
		{
			m[positions.at(i)] = i;
		}
		
		ASSERTEQ(m.size(), n);
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m.at(positions.at(i)), i);
		}
		
		map<K, long, H> m2 = m;
		
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
		map<K, long, H> m;
		const map<K, long, H>& mc = m;
		
		for (int i = 0; i < n; ++i)
		{
			m[positions.at(i)] = i;
		}
		
		ASSERTEQ(m.size(), n);
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(mc.at(positions.at(i)), i);
		}
		
		map<K, long, H> m2 = m;
		const map<K, long, H>& mc2 = m2;
		
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
		map<K, long, H> m;
		const map<K, long, H>& mc = m;
		
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
		
		map<K, long, H> m2 = m;
		const map<K, long, H>& mc2 = m2;
		
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
		
		
		map<K, long, H> m3;
		ASSERTEQ(m3.size(), 0);
		m3 = m2;
		ASSERTEQ(m2.size(), m3.size());
		ASSERTEQ(m3.size(), n);
		size_t z = 0;
		for (auto it = m2.begin(); it != m2.end(); ++it)
		{
			ASSERTEQ(it->second, m3.at(it->first));
			++z;
		}
		
		
		ASSERTEQ(z, m2.size());
		ASSERTEQ(m2.size(), n);
		
		z = 0;
		for (const auto& p : m2)
		{
			ASSERT(p.first < n);
			MASSERTEQ(p.second, m3.at(p.first), p.first);
			ASSERTEQ(m3.count(p.first), 1);
			++z;
		}
		
		ASSERTEQ(z, m2.size());
		ASSERTEQ(m2.size(), n);
		
		z = 0;
		for (auto& p : m2)
		{
			ASSERT(p.first < n);
			ASSERTEQ(m3.count(p.first), 1);
			ASSERTEQ(p.second, m3.at(p.first));
			++z;
		}
		ASSERTEQ(z, m2.size());
		ASSERTEQ(m2.size(), n);
	}
	
	
	std::clog << "--------------\n" << std::endl;
	{
		map<K, long, H> m;
		for (int i = 0; i < n; ++i)
		{
			m[positions.at(i)] = positions.at(i);
		}
		m.clear();
		ASSERTEQ(m.size(), 0);
		ASSERT(m.begin() == m.end());
		
		map<K, long, H> em;
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(em.count(positions.at(i)), 0);
		}
		
		for (int i = 0; i < n; ++i)
		{
			ASSERTEQ(m.count(positions.at(i)), 0);
		}
		
		for (int i = 0; i < n; ++i)
		{
			long h = m[positions.at(i)];
			MASSERTEQ(h, 0, positions.at(i));
		}
	}
	
	
	std::clog << "--------------\n" << std::endl;
	{
		map<K, long, H> m;
		const map<K, long, H>& mc = m;
		
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
		
		long double bc = m.bucket_count();
		long double ec = m.size();
		ASSERTEQ(m.max_load_factor(), 1);
		MASSERT(m.max_load_factor() >= m.load_factor(), m.max_load_factor(), m.load_factor(), (ec/bc), ec, bc);
		MASSERT(bc*m.max_load_factor() >= ec, bc, ec);
		m.rehash(bc*2);
		MASSERT(m.bucket_count() >= 2*bc, m.bucket_count(), 2*bc);
		bc = m.bucket_count();
		MASSERT(m.load_factor() <= 1, m.load_factor(), bc, ec, (ec/bc));
		
		
		map<K, long, H> m2 = m;
		const map<K, long, H>& mc2 = m2;
		
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
		
		
		map<K, long, H> m3;
		ASSERTEQ(m3.size(), 0);
		m3 = m2;
		ASSERTEQ(m2.size(), m3.size());
		ASSERTEQ(m3.size(), n);
		size_t z = 0;
		for (auto it = m2.begin(); it != m2.end(); ++it)
		{
			ASSERTEQ(it->second, m3.at(it->first));
			++z;
		}
		
		
		ASSERTEQ(z, m2.size());
		ASSERTEQ(m2.size(), n);
		
		z = 0;
		for (const auto& p : m2)
		{
			ASSERT(p.first < n);
			MASSERTEQ(p.second, m3.at(p.first), p.first);
			ASSERTEQ(m3.count(p.first), 1);
			++z;
		}
		
		ASSERTEQ(z, m2.size());
		ASSERTEQ(m2.size(), n);
		
		z = 0;
		for (auto& p : m2)
		{
			ASSERT(p.first < n);
			ASSERTEQ(m3.count(p.first), 1);
			ASSERTEQ(p.second, m3.at(p.first));
			++z;
		}
		ASSERTEQ(z, m2.size());
		ASSERTEQ(m2.size(), n);
	}
}


template <class Key>
struct id_hasher
{
	size_t operator()(const Key& k) const
	{
		return (size_t)k;
	}
};

template <class Key>
struct bad_hasher
{
	size_t operator()(const Key& k) const
	{
		return 1;
	}
};

template <class Inner>
struct printing_hasher;

template <template <class> class Inner, class Key>
struct printing_hasher<Inner<Key>>
{
	Inner<Key> inner;
	
	size_t operator()(const Key& k) const
	{
		std::cout << "Hashing " << k << ": ";
		auto r = inner(k);
		std::cout << r << std::endl;
		return r;
	}
};


template<template <class...> class map>
void misc_test(size_t n)
{
	typedef int* Key;
	typedef std::hash<Key> IH;
	//typedef id_hasher<Key> IH;
	//typedef bad_hasher<Key> IH;
	
	
	//typedef IH H;
	typedef printing_hasher<IH> H;
	typedef map<Key, size_t, H> M;
	
	{
		M m;
		H h;
		for (Key i = 0; i < (Key)n; i = (Key)((size_t)i + 1))
		{
			m[i] = (size_t)i;
		}
		/*auto max = std::numeric_limits<Key>::max();
		m[max] = h(max);*/
		
		for (Key i = (Key)(n-1); i > (Key)0; i = (Key)((size_t)i - 1))
		{
			ASSERTEQ(m.at(i), (size_t)i);
		}
		ASSERTEQ(m.at((Key)0), 0);
	}
}