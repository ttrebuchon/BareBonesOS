#include <Utils/vector>
#include "Tests.hh"
#include <vector>

template <class T, class A = Utils::Allocator<T>>
using uvector = Utils::vector<T, A>;



TEST(vector)
{
	assert(Foo_t::callers.size() == 0);
	assert(Foo_t::count == 0);
	{
		std::vector<Foo_t> v1;
		v1.reserve(3);
		v1.emplace_back(0);
		v1.emplace_back(1);
		v1.emplace_back(2);
		assert(v1.size() == 3);
	}
	assert(Foo_t::callers.size() == 3);
	std::clog << Foo_t::count << std::endl;
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	
	
	
	assert(Foo_t::callers.size() == 0);
	assert(Foo_t::count == 0);
	{
		uvector<Foo_t> v1;
		v1.reserve(3);
		v1.emplace_back(0);
		v1.emplace_back(1);
		v1.emplace_back(2);
		assert(v1.size() == 3);
		assert(Foo_t::callers.size() == 0);
	}
	ASSERTEQ(Foo_t::callers.size(), 3);
	std::clog << Foo_t::count << std::endl;
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	
	
	assert(Foo_t::callers.size() == 0);
	assert(Foo_t::count == 0);
	{
		uvector<Foo_t> v1;
		v1.emplace_back(0);
		v1.emplace_back(1);
		v1.emplace_back(2);
		assert(v1.size() == 3);
	}
	assert(Foo_t::callers.size() == 3);
	std::clog << Foo_t::count << std::endl;
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	
	
	
}