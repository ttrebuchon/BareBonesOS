#include "Tests.hh"
#include <kernel/utils/detail/basic_ios.hh>
#include <kernel/utils/iostream.hh>
#include <sstream>
#include <cassert>
//#include <kernel/utils/String.hh>

class Foo : public Utils::basic_ios<char>
{
	
	
};

class StdBuf : public Utils::streambuf
{
	std::stringbuf buf;
	
	public:
	
	StdBuf() : Utils::streambuf(), buf()
	{}
};


class Bar : public Utils::basic_iostream<char>
{
	
};

TEST(IOSTREAM)
{
	//Foo foo;
	typedef typename Utils::basic_iostream<char>::char_type G;
	typedef typename Bar::char_type H;
	Utils::ostream os(new StdBuf());
	
	int a = 1;
	short b = 2;
	long c = 3;
	char d = 'a';
	double e = 4.5;
	unsigned int f = 5;
	unsigned short g = 6;
	unsigned long h = 7;
	
	
	assert(os.good());
	assert(&os == &(os << a));
	assert(os.good());
	os << b;
	assert(os.good());
	os << c;
	assert(os.good());
	os << d;
	assert(os.good());
	os << e;
	assert(os.good());
	os << f;
	assert(os.good());
	os << g;
	assert(os.good());
	os << h;
	assert(os.good());
}
