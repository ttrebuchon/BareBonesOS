#include "Tests.hh"
#include <Utils/List.hh>
#include <type_traits>
#include <kernel/Memory/Paging.hh>
#include <Utils/Bitset.hh>
#include <list>

class Foo
{
	public:
	Foo()
	{
		cerr << __func__ << "\n";
	}
};

TEST(List)
{
	cerr << "Test\n";
	using namespace Utils;
	
	Allocator<Foo> x;
	auto p = x.allocate(4);
	assert(p);
	x.deallocate(p, 4);
	std::cerr << p << "\n";
	p = x.allocate(4);
	x.construct<Foo>(p);
	x.destroy(p);
	x.deallocate(p, 4);
	std::cerr << p << "\n";
	
	List<Foo> fl;
	TRACE(alignof(std::max_align_t));
	fl.push_back(Foo());
	Foo f;
	QA::out << "Pushing back 2nd..." << std::endl;
	fl.push_back(f);
	QA::out << "Pushed back 2nd." << std::endl;
	
	
	QA::out << "\n\n\n" << std::endl;
	
	
	assert(Foo_t::callers.size() == 0);
	assert(Foo_t::count == 0);
	{
		std::list<Foo_t> v1;
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
		Utils::list<Foo_t> v1;
		v1.emplace_back(0);
		v1.emplace_back(1);
		v1.emplace_back(2);
		assert(v1.size() == 3);
	}
	std::clog << Foo_t::count << std::endl;
	assert(Foo_t::callers.size() == 3);
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	
	
	assert(Foo_t::callers.size() == 0);
	assert(Foo_t::count == 0);
	{
		Utils::list<Foo_t> v1;
		v1.emplace_back(0);
		v1.emplace_back(1);
		v1.emplace_back(2);
		assert(v1.size() == 3);
	}
	assert(Foo_t::callers.size() == 3);
	std::clog << Foo_t::count << std::endl;
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	
	cerr << "Done." << std::endl;
}