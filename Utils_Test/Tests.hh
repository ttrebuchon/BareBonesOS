#ifndef INCLUDED_TESTS_HH
#define INCLUDED_TESTS_HH

#define kassert(x) assert(x)

#ifdef __cplusplus
#include <iostream>
#include <vector>

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




TEST(List);
CTEST(Bitset);
TEST(Bitset);
TEST(IDE);
TEST(IOSTREAM);
TEST(Limits);
TEST(String);
TEST(SQLite);
TEST(shared_ptr);
TEST(vector);
TEST(tuple);
TEST(map);





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
	
	static bool Check();
	static void Reset();
	static bool Check(const int count, const int callers);
};


class Bar_t
{
	private:
	static int __X;
	
	public:
	int n;
	static int count;
	static std::vector<int> callers;
	
	Bar_t() : n(++__X)
	{
		std::cout << "Bar_t(){" << n << "} called!" << std::endl;
		++count;
	}
	
	Bar_t(int n) : n(n)
	{
		std::cout << "Bar_t(" << n << ") called!" << std::endl;
		++count;
	}
	
	Bar_t(const Bar_t& f) : n(f.n)
	{
		std::cout << "Bar_t(Bar_t{" << n << "}) called!" << std::endl;
		++count;
	}
	
	Bar_t(Bar_t&& f) : n(f.n)
	{
		f.n = -1;
		std::cout << "Bar_t(~Bar_t{" << n << "}) called!" << std::endl;
	}
	
	~Bar_t()
	{
		std::cout << "~Bar_t(){" << n << "} called!" << std::endl;
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
		std::cout << "Bar_t = ~Bar_t{" << n << "} called!" << std::endl;
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
};
#endif

#endif