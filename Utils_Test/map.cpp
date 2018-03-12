#include <Utils/map>
#include "Tests.hh"
#include <map>



TEST(map)
{
	std::clog << "--------------\n" << std::endl;
	{
		using namespace std;
		map<int, int> m;
		m[0] = 0;
		assert(m[0] == 0);
		m[1] = 4;
		assert(m[1] == 4);
		m[0] = 1;
		assert(m[0] == 1);
		m[20] = -1;
		assert(m[20] == -1);
	}
	
	std::clog << "--------------\n" << std::endl;
	{
		using namespace Utils;
		map<int, int> m;
		m[0] = 0;
		assert(m[0] == 0);
		m[1] = 4;
		assert(m[1] == 4);
		m[0] = 1;
		assert(m[0] == 1);
		m[20] = -1;
		assert(m[20] == -1);
	}
	
	
	Bar_t::Reset();
	Bar_t::Check();
	std::clog << "--------------\n" << std::endl;
	{
		using namespace std;
		map<int, Bar_t> m;
		m[0] = Bar_t(0);
		assert(Bar_t::count == 1);
		assert(Bar_t::callers.size() == 0);
		assert(Bar_t::Check(1, 0));
		
		m[1].n = 4;
	}
	assert(Bar_t::count == 0);
	assert(Bar_t::callers.size() == 2);
	assert(Bar_t::Check(0, 2));
	assert(Bar_t::callers[0] == 4);
	
	
	Bar_t::Reset();
	Bar_t::Check();
	std::clog << "--------------\n" << std::endl;
	{
		using namespace Utils;
		map<int, Bar_t> m;
		m[0] = Bar_t(0);
		assert(m[0].n == 0);
		assert(Bar_t::count == 1);
		assert(Bar_t::callers.size() == 0);
		assert(Bar_t::Check(1, 0));
		
		m[1].n = 4;
	}
	assert(Bar_t::count == 0);
	assert(Bar_t::callers.size() == 2);
	assert(Bar_t::Check(0, 2));
	assert(Bar_t::callers[0] == 4);
	
	
	Bar_t::Reset();
	Bar_t::Check();
	std::clog << "--------------\n" << std::endl;
	{
		using namespace std;
		map<int, Bar_t> m;
		m[0] = Bar_t(0);
		assert(Bar_t::count == 1);
		assert(Bar_t::callers.size() == 0);
		assert(Bar_t::Check(1, 0));
		
		m[1].n = 4;
	}
	assert(Bar_t::count == 0);
	assert(Bar_t::callers.size() == 2);
	assert(Bar_t::Check(0, 2));
	assert(Bar_t::callers[0] == 4);
	
	
	Bar_t::Reset();
	Bar_t::Check();
	std::clog << "--------------\n" << std::endl;
	{
		using namespace Utils;
		map<int, Bar_t> m;
		m[0] = Bar_t(0);
		assert(Bar_t::count == 1);
		assert(Bar_t::callers.size() == 0);
		assert(Bar_t::Check(1, 0));
		
		m[1].n = 4;
	}
	assert(Bar_t::count == 0);
	assert(Bar_t::callers.size() == 2);
	assert(Bar_t::Check(0, 2));
	assert(Bar_t::callers[0] == 4);
}