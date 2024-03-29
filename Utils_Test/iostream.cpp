#include <cassert>
#include "Tests.hh"
#include <Utils/detail/basic_ios.hh>
#include <Utils/iostream.hh>
#include <sstream>

//#include <Utils/String.hh>
#include <Utils/Locale>
#include <drivers/VGA_Stream.hh>

namespace Drivers {
extern uint16_t* vga_buf;
extern uint16_t* terminal_buffer;
}

class Foo : public Utils::basic_ios<char>
{
	
	
};

class StdBuf : public Utils::streambuf, std::stringbuf
{
	
	protected:
	char* _arr;
		
	/*virtual Utils::streambuf::pos_type seekoff(Utils::streambuf::off_type off, Utils::ios_base::seekdir way, Utils::ios_base::openmode which)
	{
		
	}*/
	
	
	virtual int sync()
	{
		//assert(false);
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
	
	StdBuf() : Utils::streambuf(), std::stringbuf(), _arr(nullptr)
	{
		resetPtrs();
	}
	
	std::string str()
	{
		return std::string(Utils::streambuf::pbase(), Utils::streambuf::pptr());
		
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
		if (_arr)
		{
			std::stringbuf::setg(nullptr, nullptr, nullptr);
			std::stringbuf::setp(nullptr, nullptr);
			delete[] _arr;
		}
		char* arr = new char[n];
		_arr = arr;
		memset(arr, 0, n);
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
		
		
		assert(std::stringbuf::pptr() == _arr);
		assert(strlen(_arr) == 0);
		assert(std::stringbuf::epptr() == _arr + n);
		assert(std::stringbuf::pbase() == _arr);
		ASSERTEQ(_arr[n-1], '\0');
		assert(_arr[0] == 0);
		assert(std::stringbuf::sync() == 0);
		ASSERTEQ(std::string(std::stringbuf::pbase(), std::stringbuf::pptr()).length(), 0);
		if (str().length() != 0)
		{
			std::clog << "n: " << n << std::endl;
		}
		ASSERTEQ(str().length(), 0);
	}
};









class ArrBuf : public Utils::streambuf
{
	static constexpr size_t default_size = 2048;
	char* array;
	size_t s;
	public:
	
	ArrBuf(const size_t s = default_size) : Utils::streambuf(), array(new char[s]), s(s)
	{
		reset();
	}
	
	virtual ~ArrBuf()
	{
		delete[] array;
	}
	
	std::string to_string() const
	{
		return std::string(array);
	}
	
	void reset()
	{
		memset(array, 0, s);
		setp(array, array + s);
	}
	
	size_t array_size() const noexcept
	{
		return s;
	}
	
	char* array_copy() const
	{
		auto p = new char[s];
		memcpy(p, array, s);
		return p;
	}
};






class StdArrBuf : public std::streambuf
{
	static constexpr size_t default_size = 2048;
	char* array;
	size_t s;
	public:
	
	StdArrBuf(const size_t s = default_size) : std::streambuf(), array(new char[s]), s(s)
	{
		reset();
	}
	
	virtual ~StdArrBuf()
	{
		delete[] array;
	}
	
	std::string to_string() const
	{
		return std::string(array);
	}
	
	void reset()
	{
		memset(array, 0, s);
		setp(array, array + s);
	}
	
	size_t array_size() const noexcept
	{
		return s;
	}
	
	char* array_copy() const
	{
		auto p = new char[s];
		memcpy(p, array, s);
		return p;
	}
};


class Bar : public Utils::basic_iostream<char>
{
	
};

void test_vs_std();

TEST(IOSTREAM)
{
	STACK();
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
	{
		std::string s1 = buf->str();
		ASSERTEQ(s1.length(), 0);
		std::string s2("");
		std::cout << "'" << s1 << "'" << std::endl;
		std::cout << "'" << s2 << "'" << std::endl;
		ASSERTEQ(s1, s2);
	}
	
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
	
	ArrBuf* buf2;
	delete os.rdbuf(buf2 = new ArrBuf(101));
	
	for (int i = 0; i < 25; ++i)
	{
		assert(os.good());
		os << j;
		assert(os.good());
		os.flush();
		assert(os.good());
	}
	
	std::cout << buf2->to_string() << "\n";
	buf2->reset();
	
	std::cout << "Disabling boolalpha...\n";
	buf2->pubimbue(os.getloc());
	os.flags((os.flags() & ~(Utils::ios::boolalpha | Utils::ios::hex | Utils::ios::oct)) | Utils::ios::dec);
	assert(os.flags() & Utils::ios::dec);
	for (int i = 0; i < 25; ++i)
	{
		assert(os.good());
		os << j;
		assert(os.good());
		os.flush();
		assert(os.good());
	}
	std::cout << buf2->to_string() << "\n";
	control = std::string(25, '1');
	std::cout << control << std::endl;
	assert(control == buf2->to_string());
	
	buf2->reset();
	os << 14;
	os << 15;
	os << 10;
	os << 9 << 8 << 7 << 6 << 2 << 1 << 0;
	std::cout << buf2->to_string() << "\n";
	
	os.flags((os.flags() & ~Utils::ios::dec) | Utils::ios::hex | Utils::ios::showbase);
	buf2->reset();
	os << 16;
	std::cout << buf2->to_string() << "\n";
	
	
	
	std::stringstream ss;
	ss.flags(os.flags());
	
	buf2->reset();
	ss.str("");
	ss << "Hello, world!\n";
	os << "Hello, world!\n";
	assert(os.good());
	assert(ss.good());
	std::cout << buf2->to_string() << std::flush;
	assert(buf2->to_string() == ss.str());
	
	
	
	buf2->reset();
	ss.str("");
	os.flags(Utils::ios::dec);
	ss.flags(os.flags());
	os << L"Test" << "\n";
	ss << L"Test" << "\n";
	assert(os.good());
	assert(ss.good());
	std::cout << buf2->to_string() << std::flush;
	assert(ss.str() == buf2->to_string());
	
	
	os.unsetf(Utils::ios_base::hex);
	os.flags(os.flags() | Utils::ios_base::dec);
	buf2->reset();
	ss.str("");
	os << 4.20001 << "\n";
	ss << 4.20001 << "\n";
	assert(os.good());
	assert(ss.good());
	std::cout << buf2->to_string() << std::flush;
	std::cout << ss.str() << std::flush;
	//assert(buf2->to_string() == ss.str());
	
	
	Drivers::VGAStreamBuf vbuf;
	Utils::ostream vos(&vbuf);
	
	vos << "Test";
	vos.flush();
	assert(((char*)Drivers::vga_buf)[0] == 'T');
	assert(((char*)Drivers::vga_buf)[2] == 'e');
	for (int i = 0; i < 30; ++i)
	{
		vos << "Hi\n";
	}
	vos.flush();
	assert(((char*)Drivers::vga_buf)[0] == 'H');
	assert(((char*)Drivers::vga_buf)[2] == 'i');
	
	
	
	buf2->reset();
	unsigned char cbuf[41];
	cbuf[0] = 'A';
	cbuf[1] = 'B';
	cbuf[2] = 'C';
	cbuf[3] = 0;
	
	os << cbuf;
	os.flush();
	
	std::cout << buf2->to_string() << std::endl;
	assert(buf2->to_string() == "ABC");
	
	
	delete buf2;
	
	test_vs_std();
}

//using namespace __gnu_cxx;












template <class OS, class Buf, class Fn>
static Buf* test_ios(Fn f)
{
	auto b = new Buf;
	OS o(b);
	
	f(o);
	
	return b;
}

template <class Fn>
static void __test_vs_ios(Fn f)
{
	auto sb = test_ios<std::ostream, StdArrBuf>(f);
	
	auto ub = test_ios<Utils::ostream, ArrBuf>(f);
	
	ASSERTEQ(sb->array_size(), ub->array_size());
	
	auto str1 = sb->to_string();
	auto str2 = ub->to_string();
	
	ASSERTEQ(str1, str2);
	
	
	auto sa = sb->array_copy();
	auto ua = ub->array_copy();
	
	assert(memcmp(sa, ua, sb->array_size()) == 0);
	
	delete[] sa;
	delete[] ua;
	
	delete sb;
	delete ub;
}





void test_vs_std()
{
	{
		auto fn = [](auto& os)
		{
			os << "Mark.";
			os << "Mark2";
			os.flush();
		};
		__test_vs_ios(fn);
	}
	
	
	{
		auto fn = [](auto& os)
		{
			os << "Mark.";
			os.flush();
			os << "Mark2";
			os << 41;
			os.flush();
		};
		__test_vs_ios(fn);
	}
}