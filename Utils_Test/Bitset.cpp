#include "Tests.hh"
#define private public
#define protected public
#include <Utils/Bitset.hh>
#undef protected
#undef private
#include <cassert>
#include <iomanip>
#include <bitset>

static void test_bitset();

TEST(Bitset)
{
	using namespace Utils;
	
	Bitset<1024> bs;
	bs.set(0, true);
	assert(bs.get(0));
	bs.set(1, true);
	assert(bs.get(1));
	for (int i = 0; i < 1024; ++i)
	{
		bs.set(i, true);
	}
	
	for (int i = 0; i < 1024; ++i)
	{
		static_assert(sizeof(uchar) == 1);
		assert(bs.get(i));
	}
	
	
	for (volatile int i = 0; i < 10000000; ++i)
	{
		volatile auto j = rand() % 1024;
		if (i % 2 == 0)
		{
			bs.set(j, true);
			assert(bs.get(j));
		}
		else
		{
			bs.set(j, false);
			assert(!bs.get(j));
		}
	}
	
	bs.setAll(true);
	for (int i = 0; i < 1024; ++i)
	{
		assert(bs.get(i));
	}
	
	bs.setAll(false);
	for (int i = 0; i < 1024; ++i)
	{
		if (bs.get(i))
		{
			std::cerr << i << std::endl;
			
		}
		assert(!bs.get(i));
	}
	
	static_assert(Bitset<1024>::Count == 128);
	static_assert(Bitset<1025>::Count == 129);
	
	
	Bitset<1025> bs2;
	for (int i = 0; i < 1025; ++i)
	{
		bs2.set(i, (i%2 == 0));
	}
	
	bs2.invert();
	for (int i = 0; i < 1025; ++i)
	{
		assert(bs2.get(i) == (i%2 == 1));
	}
	
	bs2.setAll(false);
	uint32_t index;
	
	bs2.set(574, true);
	assert(bs2.firstTrue(&index));
	assert(index == 574);
	
	bs2.set(562, true);
	assert(bs2.firstTrue(&index));
	assert(index == 562);
	
	bs2.set(562, false);
	assert(bs2.firstTrue(&index));
	assert(index == 574);
	
	bs2.invert();
	assert(bs2.firstTrue(&index));
	assert(index == 0);
	
	bs2.setAll(false);
	bs2.set(1024, true);
	assert(bs2.firstTrue(&index));
	assert(index == 1024);
	
	
	uint32_t prevIndex = index;
	
	bs2.setAll(false);
	assert(!bs2.firstTrue(&index));
	assert(index == prevIndex);
	
	bs2.setAll(true);
	assert(!bs2.firstFalse(&index));
	assert(index == prevIndex);
	
	/*bs2.setAll(false);
	bs2.set(1025, true);
	assert(!bs2.firstTrue(&index));
	assert(index == prevIndex);*/
	
	bs2.setAll(true);
	assert(bs2.firstTrue(nullptr));
	
	bs2.setAll(false);
	assert(bs2.firstFalse(nullptr));
	
	
	
	
	
	auto mem = new uchar[256];
	Bitset_Ptr<> bs3(mem, 256);
	prevIndex = index;
	assert(!bs3.firstTrue(&index));
	assert(index == prevIndex);
	assert(bs3.size() == 256*8);
	assert(bs3._bits_used_mask == 0);
	
	bs3.setAll(true);
	assert(!bs3.firstFalse(&index));
	assert(index == prevIndex);
	
	assert(bs3.firstTrue(nullptr));
	
	bs3.setAll(false);
	for (uint32_t i = 0; i < 256; ++i)
	{
		assert(bs3.bits[i] == 0x0);
	}
	assert(bs3.firstFalse(nullptr));
	bs3.bits = nullptr;
	delete[] mem;
	mem = nullptr;
	
	
	
	
	
	auto mem2 = new uint32_t[2];
	Bitset_Ptr<uint32_t> bs4(mem2, 2, 60);
	assert(bs4._bits_used == 60);
	
	bs4.setAll(true);
	for (uint32_t i = 0; i < 60; ++i)
	{
		bs4.set(i, false);
	}
	assert(!bs4.firstTrue(nullptr));
	
	
	bs4.setAll(false);
	prevIndex = index;
	assert(!bs4.firstTrue(&index));
	assert(index == prevIndex);
	assert(bs4.size() == 60);
	//assert(bs4._bits_used_mask == 3);
	
	bs4.setAll(true);
	assert(!bs4.firstFalse(&index));
	assert(index == prevIndex);
	
	assert(bs4.firstTrue(nullptr));
	
	bs4.setAll(false);
	for (uint32_t i = 0; i < 2; ++i)
	{
		assert(bs4.bits[i] == 0x0);
	}
	assert(bs4.firstFalse(nullptr));
	
	bs4.setAll(false);
	for (uint32_t i = 0; i < 59; ++i)
	{
		bs4.set(i, true);
	}
	assert(bs4.firstFalse(nullptr));
	bs4.set(59, true);
	assert(!bs4.firstFalse(nullptr));
	
	bs4.setAll(true);
	for (uint32_t i = 0; i < 60; ++i)
	{
		bs4.set(i, false);
	}
	{
		uint32_t ind;
		if (bs4.firstTrue(&ind))
		{
			std::cout << ind << std::endl;
		}
		std::cout << "bits[1]: " << (void*)(addr_t)(bs4.bits[1] & bs4._bits_used_mask) << std::endl;
	}
	assert(!bs4.firstTrue(nullptr));
	
	bs4.bits = nullptr;
	delete[] mem2;
	
	test_bitset();
}

template <template <size_t> class T>
static void test_type();

static void test_bitset()
{
	test_type<std::bitset>();
	test_type<Utils::bitset>();
}

template <template <size_t> class T, size_t N>
static void test_type_standard();

template <template <size_t> class T>
static void test_type()
{
	test_type_standard<T, 8>();
	test_type_standard<T, 4>();
	test_type_standard<T, 8192>();
	test_type_standard<T, 2048>();
}


template <template <size_t> class T, size_t N>
static void test_type_standard()
{
	static_assert(N > 2);
	T<N> s;
		
	s[2] = true;
	s[0] = true;
		
	//assert(s.any());
	//assert(!s.all());
		
	assert(s[0]);
	assert(s[2]);
		
		
	for (int i = 0; i < N; ++i)
	{
		s[i] = true;
	}
		
	//assert(s.any());
	//assert(s.all());
		
	for (int i = 0; i < N; ++i)
	{
		assert(s[i]);
	}
	
	if (N <= 2048)
	for (int i = 0; i < N; ++i)
	{
		s[i] = false;
		for (int j = 0; j < N; ++j)
		{
			if (i != j)
			{
				assert(s[j]);
			}
		}
		s[i] = true;
	}
	
	for (int i = 0; i < N; ++i)
	{
		s[i] = (i % 2 == 0);
	}
	
	for (int i = 0; i < N; ++i)
	{
		ASSERTEQ(s[i], (i % 2 == 0));
	}
}