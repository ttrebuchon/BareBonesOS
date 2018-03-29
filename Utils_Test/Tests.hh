#ifndef INCLUDED_TESTS_HH
#define INCLUDED_TESTS_HH

#define kassert(x) assert(x)

#ifdef __cplusplus
#include <iostream>
#include <vector>
#include <sstream>
#include "TestUtils/MemTracker.hh"

using std::cerr;

#else

#include <stdio.h>

#endif

#define TEST(X) void Test_##X()
#ifdef __cplusplus
#define CTEST(X) extern "C" void TestC_##X ()
#else
#define CTEST(X) void TestC_##X ()
#endif










#ifdef __cplusplus
class Foo_t
{
	public:
	int n;
	
	
	public:
	static int count;
	static std::vector<int> callers;
	
	Foo_t(int n) : n(n)
	{
		std::cout << "Foo_t(" << n << ") called!" << std::endl;
		++count;
	}
	
	Foo_t(const Foo_t& f) : n(f.n)
	{
		std::cout << "Foo_t(Foo_t{" << n << "}) called!" << std::endl;
		++count;
	}
	
	Foo_t(Foo_t&& f) : n(f.n)
	{
		f.n = -1;
		std::cout << "Foo_t(~Foo_t{" << n << "}) called!" << std::endl;
	}
	
	~Foo_t()
	{
		std::cout << "~Foo_t(){" << n << "} called!" << std::endl;
		if (n != -1)
		{
		callers.push_back(n);
		--count;
		}
	}
	
	int operator()() const
	{
		std::cout << "Foo_t{" << n << "}() called!" << std::endl;
		return n;
	}
	
	int operator()(int x)
	{
		int old = n;
		n = x;
		std::cout << "Foo_t{n=" << n << "}() called!" << std::endl;
		return old;
	}
	
	static bool Check();
	static void Reset();
	static bool Check(const int count, const int callers);
};


class Bar_t
{
	private:
	static int __X;
	static bool __printing;
	
	public:
	int n;
	static int count;
	static std::vector<int> callers;
	
	Bar_t() : n(++__X)
	{
		if (__printing)
		{
			std::cout << "Bar_t(){" << n << "} called!" << std::endl;
		}
		++count;
	}
	
	Bar_t(int n) : n(n)
	{
		if (__printing)
		{
			std::cout << "Bar_t(" << n << ") called!" << std::endl;
		}
		++count;
	}
	
	Bar_t(const Bar_t& f) : n(f.n)
	{
		if (__printing)
		{
			std::cout << "Bar_t(Bar_t{" << n << "}) called!" << std::endl;
		}
		++count;
	}
	
	Bar_t(Bar_t&& f) : n(f.n)
	{
		f.n = -1;
		if (__printing)
		{
			std::cout << "Bar_t(~Bar_t{" << n << "}) called!" << std::endl;
		}
	}
	
	~Bar_t()
	{
		if (__printing)
		{
			std::cout << "~Bar_t(){" << n << "} called!" << std::endl;
		}
		
		if (n != -1)
		{
			callers.push_back(n);
			--count;
		}
	}
	
	Bar_t& operator=(const Bar_t& b)
	{
		n = b.n;
		return *this;
	}
	
	Bar_t& operator=(Bar_t&& b)
	{
		n = b.n;
		b.n = -1;
		if (__printing)
		{
			std::cout << "Bar_t = ~Bar_t{" << n << "} called!" << std::endl;
		}
		--count;
		return *this;
	}
	
	static int getN()
	{
		return __X;
	}
	
	static bool Check();
	static void Reset();
	static bool Check(const int count, const int callers);
	
	static bool TogglePrinting();
	static bool SetPrinting(bool value);
	static bool IsPrintingEnabled();
};

inline bool operator==(const Bar_t& l, const Bar_t& r)
{
	return l.n == r.n;
}

inline bool operator!=(const Bar_t& l, const Bar_t& r)
{
	return l.n != r.n;
}

inline std::ostream& operator<<(std::ostream& os, const Bar_t& b)
{
	return (os << "[Bar_t:" << b.n << "]");
}

template <class T, class V>
inline void do_assert_eq(const T& x, const V& y, const char* file, const int line, const char* func, const char* X, const char* Y, const char* msg = nullptr)
{
	if (x != y)
	{
		std::stringstream ss;
		ss << "eq(" << X << ", " << Y << "):" << std::endl << "{ (" << x << ") != (" << y << ") }" << std::endl;
		if (msg)
		{
			ss << msg << std::endl;
		}
		auto str = ss.str();
		//std::cout << std::endl << "(" << x << ") != (" << y << ")" << std::endl;
		__assert2(file, line, func, str.c_str());
		
	}
}

template <class T>
inline void do_assert_eq(const T& x, const std::nullptr_t& y, const char* file, const int line, const char* func, const char* X, const char* Y, const char* msg = nullptr)
{
	if (x != y)
	{
		std::stringstream ss;
		ss << "eq(" << X << ", " << Y << "):" << std::endl << "{ (" << x << ") != (" << (T)y << ") }" << std::endl;
		if (msg)
		{
			ss << msg << std::endl;
		}
		auto str = ss.str();
		//std::cout << std::endl << "(" << x << ") != (" << y << ")" << std::endl;
		__assert2(file, line, func, str.c_str());
		
	}
}


inline void do_assert(const bool x, const char* file, const int line, const char* func, const char* X, const char* msg = nullptr)
{
	if (!x)
	{
		std::stringstream ss;
		ss << "assert(" << X << "):" << std::endl << "{ (" << x << ") != (true) }" << std::endl;
		if (msg)
		{
			ss << msg << std::endl;
		}
		auto str = ss.str();
		//std::cout << std::endl << "(" << x << ") != (" << y << ")" << std::endl;
		__assert2(file, line, func, str.c_str());
		
	}
}

template <int I, int N, class Stream, class... Args>
struct Tuple_Printer
{
	/*template <int I>
	struct Inner;*/
	/*{
		inline static void call(Stream& out, const std::tuple<Args...>& tup, const char* seperator = nullptr)
		{
			if (seperator)
			{
				out << seperator;
			}
			out << std::get<I>(tup);
			Inner<I+1>::call(out, tup, seperator);
		}
	}*/
	
	inline static void call(Stream& out, const std::tuple<Args...>& tup, const char* seperator = nullptr)
	{
		if (seperator)
		{
			out << seperator;
		}
		out << std::get<I>(tup);
		Tuple_Printer<I+1, N, Stream, Args...>::call(out, tup, seperator);
	}
};


template <int N, class Stream, class... Args>
struct Tuple_Printer<0, N, Stream, Args...>
{
	inline static void call(Stream& out, const std::tuple<Args...>& tup, const char* seperator = nullptr)
	{
		out << std::get<0>(tup);
		Tuple_Printer<1, N, Stream, Args...>::call(out, tup, seperator);
	}
};

template <int N, class Stream, class... Args>
struct Tuple_Printer<N, N, Stream, Args...>
{
	inline static void call(Stream& out, const std::tuple<Args...>& tup, const char* seperator = nullptr)
	{
		
		
	}
};

/*template <class Stream, class... Args>
struct Tuple_Printer<sizeof...(Args), Stream, Args...>
{
	inline static void call(Stream& out, const std::tuple<Args...>& tup, const char* seperator = nullptr)
	{
		
	}
};*/

template <class... NameArgs, class... Args>
inline void do_assert(const bool x, const char* file, const int line, const char* func, const char* X, std::tuple<NameArgs...> argNames, std::tuple<Args...> args)
{
	if (!x)
	{
		std::stringstream ss;
		ss << "{ ";
		Tuple_Printer<0, sizeof...(NameArgs), std::stringstream, NameArgs...>::call(ss, argNames, ", ");
		ss << " } = { ";
		Tuple_Printer<0, sizeof...(Args), std::stringstream, Args...>::call(ss, args, ", ");
		ss << " }";
		//(ss << ... << args);
		auto str = ss.str();
		do_assert(x, file, line, func, X, str.c_str());
	}
}

template <class T, class V, class... NameArgs, class... Args>
inline void do_assert_eq(const T& x, const V& y, const char* file, const int line, const char* func, const char* X, const char* Y, std::tuple<NameArgs...> argNames, std::tuple<Args...> args)
{
	if (x != y)
	{
		std::stringstream ss;
		ss << "{ ";
		Tuple_Printer<0, sizeof...(NameArgs), std::stringstream, NameArgs...>::call(ss, argNames, ", ");
		ss << " } = { ";
		Tuple_Printer<0, sizeof...(Args), std::stringstream, Args...>::call(ss, args, ", ");
		ss << " }";
		//(ss << ... << args);
		auto str = ss.str();
		do_assert_eq(x, y, file, line, func, X, Y, str.c_str());
	}
}

#define ASSERTEQ(X, Y) do { auto __x = X; auto __y = Y; if (__x != __y) { do_assert_eq(__x, __y, __FILE__, __LINE__, __func__, #X, #Y); } } while (0)

#define MASSERTEQ(X, Y, ...) do { auto __x = X; auto __y = Y; if (__x != __y) { do_assert_eq(__x, __y, __FILE__, __LINE__, __func__, #X, #Y, std::make_tuple(#__VA_ARGS__), std::make_tuple(__VA_ARGS__)); } } while (0)

#define MASSERT(X, ...) do { auto __x = X; if (!__x) { do_assert(__x, __FILE__, __LINE__, __func__, #X, std::make_tuple(#__VA_ARGS__), std::make_tuple(__VA_ARGS__)); } } while (0)


#endif

#endif
#include <kernel/Debug.h>