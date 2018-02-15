#include "Tests.hh"
#include <kernel/utils/detail/basic_ios.hh>
#include <kernel/utils/iostream.hh>
#include <sstream>
#include <cassert>
//#include <kernel/utils/String.hh>
#include <kernel/utils/locale>

class Foo : public Utils::basic_ios<char>
{
	
	
};

class StdBuf : public Utils::streambuf, std::stringbuf
{
	
	protected:
		
	/*virtual Utils::streambuf::pos_type seekoff(Utils::streambuf::off_type off, Utils::ios_base::seekdir way, Utils::ios_base::openmode which)
	{
		
	}*/
	
	
	virtual int sync()
	{
		assert(false);
		return std::stringbuf::sync();
	}
		
		
		
	virtual Utils::streamsize xsputn(const Utils::streambuf::char_type* s, Utils::streamsize n)
	{
		//std::stringbuf::xsputn(s, n);
		
		auto v = Utils::streambuf::xsputn(s, n);
		assert(v == n);
		//std::stringbuf::pbump(v);
		return v;
	}
	
	virtual Utils::streambuf::int_type overflow(Utils::streambuf::int_type c)
	{
		assert(false);
		auto res = std::stringbuf::overflow(c);
		resetPtrs();
		
		return res;
	}
	
	void resetPtrs()
	{
		Utils::streambuf::setg(std::stringbuf::eback(), std::stringbuf::gptr(), std::stringbuf::egptr());
		
		Utils::streambuf::setp(std::stringbuf::pptr(), std::stringbuf::epptr());
		assert(Utils::streambuf::pptr() == std::stringbuf::pptr());
	}
	
	public:
	
	StdBuf() : Utils::streambuf(), std::stringbuf()
	{
		resetPtrs();
	}
	
	std::string str()
	{
		std::stringbuf::setp(Utils::streambuf::pbase(), Utils::streambuf::epptr());
		std::stringbuf::pbump(Utils::streambuf::pptr() - Utils::streambuf::pbase());
		
		
		assert(std::stringbuf::pbase() == Utils::streambuf::pbase());
		assert(std::stringbuf::pptr() == Utils::streambuf::pptr());
		assert(std::stringbuf::epptr() == Utils::streambuf::epptr());
		assert(std::stringbuf::eback() == Utils::streambuf::eback());
		assert(std::stringbuf::gptr() == Utils::streambuf::gptr());
		assert(std::stringbuf::egptr() == Utils::streambuf::egptr());
		
		auto ret = std::stringbuf::str();
		
		assert(std::stringbuf::pbase() == Utils::streambuf::pbase());
		assert(std::stringbuf::pptr() == Utils::streambuf::pptr());
		assert(std::stringbuf::epptr() == Utils::streambuf::epptr());
		assert(std::stringbuf::eback() == Utils::streambuf::eback());
		assert(std::stringbuf::gptr() == Utils::streambuf::gptr());
		assert(std::stringbuf::egptr() == Utils::streambuf::egptr());
		
		return ret;
	}
	
	void str(const std::string s)
	{
		std::stringbuf::str(s);
		std::stringbuf::sync();
		resetPtrs();
	}
	
	Utils::streamsize howmany()
	{
		//return Utils::streambuf::showmanyc();
		return std::stringbuf::showmanyc();
	}
	
	void setArray(const size_t n)
	{
		char* arr = new char[n];
		std::stringbuf::setbuf(arr, n);
		Utils::streambuf::setbuf(arr, n);
		
		
		std::stringbuf::setp(arr, arr+n);
		Utils::streambuf::setp(arr, arr+n);
		std::stringbuf::setg(arr, arr, arr+n);
		Utils::streambuf::setg(arr, arr, arr+n);
		assert(std::stringbuf::pbase() == arr);
		assert(std::stringbuf::pbase() == Utils::streambuf::pbase());
		assert(std::stringbuf::pptr() == Utils::streambuf::pptr());
		assert(std::stringbuf::epptr() == Utils::streambuf::epptr());
		assert(std::stringbuf::eback() == Utils::streambuf::eback());
		assert(std::stringbuf::gptr() == Utils::streambuf::gptr());
		assert(std::stringbuf::egptr() == Utils::streambuf::egptr());
		
		
		
		std::stringbuf::pbump(4);
		Utils::streambuf::pbump(4);
		
		assert(std::stringbuf::pbase() == Utils::streambuf::pbase());
		assert(std::stringbuf::pptr() == Utils::streambuf::pptr());
		assert(std::stringbuf::epptr() == Utils::streambuf::epptr());
		
		
		std::stringbuf::pbump(-4);
		Utils::streambuf::pbump(-4);
		
		assert(std::stringbuf::pbase() == Utils::streambuf::pbase());
		assert(std::stringbuf::pptr() == Utils::streambuf::pptr());
		assert(std::stringbuf::epptr() == Utils::streambuf::epptr());
		
	}
};


class Bar : public Utils::basic_iostream<char>
{
	
};

TEST(IOSTREAM)
{
	//Foo foo;
	typedef typename Utils::basic_iostream<char>::char_type G;
	typedef typename Bar::char_type H;
	StdBuf* buf;
	Utils::ostream os(buf = new StdBuf());
	
	os.flags(os.flags() | Utils::ios::boolalpha);
	
	/*std::string longS;
	for (int i = 0; i < 1000; ++i)
	{
		longS += "DEADBABA";
	}
	
	buf->str(longS);*/
	
	int a = 1;
	short b = 2;
	long c = 3;
	char d = 'a';
	double e = 4.5;
	unsigned int f = 5;
	unsigned short g = 6;
	unsigned long h = 7;
	bool j = true;
	
	buf->setArray(400000);
	assert(buf->str() == "");
	
	std::string control = "";
	
	for (int i = 0; i < 10; ++i)
	{
		assert(os.good());
		os << j;
		control += "true";
		assert(os.good());
	}
	//auto bstr = buf->str();
	std::string bstr;
	std::cout << "\"" << buf->str() << "\"\n" << std::flush;
	std::cout << "\"" << (bstr = buf->str()) << "\"\n" << std::flush;
	std::cout << "\"" << bstr << "\"\n" << std::flush;
	std::cout << "\"" << control << "\"\n" << std::flush;
	std::cout << bstr.length() << " : " << control.length() << " : " << buf->str().length() << std::endl;
	std::cout << bstr.length() << " : " << control.length() << " : " << buf->str().length() << std::endl;
	assert(bstr == control);
	
	assert(os.good());
	os << j;
	assert(os.good());
	/*assert(&os == &(os << a));
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
	assert(os.good());*/
}

//using namespace __gnu_cxx;