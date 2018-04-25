#include "Tests.hh"
#include <kernel/Memory/PhysicalMemory.hh>
#include <kernel/Memory/Managers/Basic_Physical.hh>


using namespace Kernel::Memory;

TEST(PhysicalMemory)
{
	PhysicalMemory::Use<Basic_Physical>();
	size_t alen = 1;
	auto a = PhysicalMemory::reserve(alen);
	auto aOld = a;
	assert((void*)a != nullptr);
	assert(alen == 4096);
	
	assert((void*)a < QA::physical_end());
	assert((void*)a >= QA::physical_start());
	
	alen = 1;
	bool rel = PhysicalMemory::release(a, alen);
	assert(rel);
	
	alen = 1;
	a = PhysicalMemory::reserve(alen);
	assert((void*)a != nullptr);
	assert(alen == 4096);
	assert(a == aOld);
}