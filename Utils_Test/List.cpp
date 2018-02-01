#include "Tests.hh"
#include <kernel/utils/List.hh>
#include <type_traits>
#include <kernel/Memory/Paging.h>
#include <kernel/utils/Bitset.hh>

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
	x.deallocate(p, 4);
	std::cerr << p << "\n";
	x.construct<Foo>(p);
	p = x.allocate(4);
	x.construct<Foo>(p);
	x.deallocate(p, 4);
	x.destroy(p);
	std::cerr << p << "\n";
	
	List<Foo> fl;
	
	fl.push_back(Foo());
	Foo f;
	fl.push_back(f);
	
	cerr << "Done." << std::endl;
}