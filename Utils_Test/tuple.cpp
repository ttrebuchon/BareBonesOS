#include <Utils/tuple>
#include "Tests.hh"
#include <tuple>

struct Bar
{
	int x, y;
	
	Bar(int a, int b) : x(a), y(b)
	{}
};

TEST(tuple)
{
	{
		using namespace std;
		tuple<int, long> t;
		get<1>(t) = 4;
		get<0>(t) = 2;
		assert(get<1>(t) == 4);
		assert(get<0>(t) == 2);
		
		const tuple<int, long>& ct = t;
		assert(get<1>(ct) == 4);
		assert(get<0>(ct) == 2);
	}
	
	{
		using namespace Utils;
		tuple<int, long> t;
		get<1>(t) = 4;
		get<0>(t) = 2;
		assert(get<1>(t) == 4);
		assert(get<0>(t) == 2);
		
		const tuple<int, long>& ct = t;
		assert(get<1>(ct) == 4);
		assert(get<0>(ct) == 2);
	}
	
	
	
	{
		using namespace std;
		tuple<Bar, Bar> t(Bar{0, 1}, Bar{1, 2});
		
	}
	
	{
		using namespace Utils;
		tuple<Bar, Bar> t(Bar{0, 1}, Bar{1, 2});
		
	}
	
	
	
	Foo_t::callers.clear();
	assert(Foo_t::count == 0);
	assert(Foo_t::callers.size() == 0);
	{
		using namespace std;
		tuple<Foo_t, Foo_t> t(0, 1);
		
		assert(Foo_t::count == 2);
	}
	std::clog << Foo_t::callers.size() << std::endl;
	assert(Foo_t::callers.size() == 2);
	assert(Foo_t::count == 0);
	
	Foo_t::callers.clear();
	assert(Foo_t::count == 0);
	assert(Foo_t::callers.size() == 0);
	{
		using namespace Utils;
		tuple<Foo_t, Foo_t> t(0, 1);
		
		assert(Foo_t::count == 2);
	}
	std::clog << Foo_t::callers.size() << std::endl;
	assert(Foo_t::callers.size() == 2);
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
}