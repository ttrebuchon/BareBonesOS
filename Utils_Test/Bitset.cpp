#include "Tests.hh"
#include <kernel/utils/Bitset.hh>
#include <cassert>
#include <iomanip>

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
	
	
	
	
	
	
	Bitset_Ptr<> bs3(new uchar[256], 256);
	prevIndex = index;
	assert(!bs3.firstTrue(&index));
	assert(index == prevIndex);
	
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
}