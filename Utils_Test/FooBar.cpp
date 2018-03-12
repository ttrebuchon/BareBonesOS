#include "Tests.hh"

const std::string _prefix = "\n!!!!! ";

int Foo_t::count = 0;
std::vector<int> Foo_t::callers;
bool Foo_t::Check()
{
	assert(Foo_t::count == 0);
	assert(callers.size() == 0);
	return true;
}

void Foo_t::Reset()
{
	Foo_t::count = 0;
	Foo_t::callers.clear();
}

bool Foo_t::Check(const int _count, const int _callers)
{
	bool b = ((count == _count) && (callers.size() == _callers));
	if (!b)
	{
		if (count != _count)
		{
			std::cerr << _prefix << "count != " << _count << std::endl;
		}
		if (callers.size() != _callers)
		{
			std::cerr << _prefix << "callers.size() != " << _callers << std::endl;
		}
		return false;
	}
	return true;
}


int Bar_t::__X = 0;
int Bar_t::count = 0;
std::vector<int> Bar_t::callers;
bool Bar_t::Check()
{
	assert(count == 0);
	assert(callers.size() == 0);
	return true;
}

void Bar_t::Reset()
{
	count = 0;
	callers.clear();
	__X = 0;
}

bool Bar_t::Check(const int _count, const int _callers)
{
	bool b = ((count == _count) && (callers.size() == _callers));
	if (!b)
	{
		if (count != _count)
		{
			std::cerr << _prefix << "count != " << _count << std::endl;
		}
		if (callers.size() != _callers)
		{
			std::cerr << _prefix << "callers.size() != " << _callers << std::endl;
		}
		return false;
	}
	return true;
}