#include "MemTracker.hh"

namespace QA
{
	template <class T>
	using MAllocator = Memory::MetaAllocator<T>;
	
	std::map<void*, Memory::Allocation*, std::less<void*>, MAllocator<std::pair<void* const, Memory::Allocation*>>> Memory::allocationMap;
	std::list<Memory::Allocation*, MAllocator<Memory::Allocation*>> Memory::allocations;
		
	void* Memory::metaPool;
	void* Memory::metaPtr;
	size_t Memory::metaLimit;
	MAllocator<Memory::Allocation> Memory::alloc;
	bool Memory::__initted = false;
	size_t Memory::_total = 0;
	
	const std::map<void*, Memory::Allocation*, std::less<void*>, MAllocator<std::pair<void* const, Memory::Allocation*>>>& Memory::Map = Memory::allocationMap;
	const std::list<Memory::Allocation*, MAllocator<Memory::Allocation*>>& Memory::Allocations = Memory::allocations;
	const size_t& Memory::Total = _total;
	
	
	void Memory::Init()
	{
		if (__initted)
		{
			return;
		}
		__initted = true;
		
		
		#ifdef TRACK_ALLOC
		metaLimit = 1024*1024*1024;
		metaPool = malloc(metaLimit);
		metaPtr = metaPool;
		#endif
	}
	
	
	
	void* Memory::Allocate(size_t s)
	{
		auto p = malloc(s);
		
		if (__initted)
		{
			Allocation* a = alloc.allocate(1);
			alloc.construct(a, p, s);
			allocationMap[p] = a;
			allocations.push_back(a);
			_total += s;
		}
		
		return p;
	}
	
	void* Memory::AllocateArray(size_t s)
	{
		auto p = malloc(s);
		
		if (__initted)
		{
			Allocation* a = alloc.allocate(1);
			alloc.construct(a, p, s, true);
			allocationMap[p] = a;
			allocations.push_back(a);
			_total += s;
		}
		
		return p;
	}
	
	void Memory::Release(void* ptr)
	{
		if (__initted)
		{
			allocationMap.erase(ptr);
		}
		
		free(ptr);
	}
	
	void Memory::Release(void* ptr, size_t s)
	{
		if (__initted)
		{
			allocationMap.erase(ptr);
		}
		
		free(ptr);
	}
	
	void Memory::ReleaseArray(void* ptr)
	{
		if (__initted)
		{
			allocationMap.erase(ptr);
		}
		
		free(ptr);
	}
	
	void Memory::ReleaseArray(void* ptr, size_t s)
	{
		if (__initted)
		{
			allocationMap.erase(ptr);
		}
		
		
		free(ptr);
	}
	
	
	void Memory::Reset()
	{
		for (auto a : allocations)
		{
			alloc.destroy(a);
		}
		
		allocations.clear();
		allocationMap.clear();
		metaPtr = metaPool;
		_total = 0;
	}
	
	
	
	
	
	
}
#ifdef TRACK_ALLOC
void* operator new(size_t size)
{
    return QA::Memory::Allocate(size);
}

void* operator new[](size_t size)
{
	return QA::Memory::AllocateArray(size);
}

void operator delete(void* ptr)
{
    QA::Memory::Release(ptr);
}

void operator delete(void* ptr, size_t s)
{
    QA::Memory::Release(ptr, s);
}

void operator delete[](void* ptr)
{
    QA::Memory::ReleaseArray(ptr);
}

void operator delete[](void* ptr, size_t s)
{
    QA::Memory::ReleaseArray(ptr, s);
}
#endif