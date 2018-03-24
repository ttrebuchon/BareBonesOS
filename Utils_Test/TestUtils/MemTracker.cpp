#include "MemTracker.hh"
#include <iostream>

using Memory = typename QA::Memory;
	
	
	
	template <class T>
	using MAllocator = Memory::MetaAllocator<T>;
	
	std::map<void*, Memory::Allocation*, std::less<void*>, MAllocator<std::pair<void* const, Memory::Allocation*>>> Memory::allocationMap;
	std::list<Memory::Allocation*, MAllocator<Memory::Allocation*>> Memory::allocations;
		
	MAllocator<Memory::Allocation> Memory::alloc;
	bool Memory::__initted = false;
	size_t Memory::_total = 0;
	
	const std::map<void*, Memory::Allocation*, std::less<void*>, MAllocator<std::pair<void* const, Memory::Allocation*>>>& Memory::Map = Memory::allocationMap;
	const std::list<Memory::Allocation*, MAllocator<Memory::Allocation*>>& Memory::Allocations = Memory::allocations;
	const size_t& Memory::Total = _total;
	
	
	void Memory::Start()
	{
		__initted = true;
	}
	
	void Memory::Pause()
	{
		__initted = false;
	}
	
	
	
	void* Memory::Allocate(size_t s, bool meta)
	{
		auto p = malloc(s);
		
		if (__initted && !meta)
		{
			if (s == 0)
			{
				out << "\n\nZero byte allocation requested, granted at " << p << "\n\n" << std::endl;
				while (true);
			}
			Allocation* a = alloc.allocate(1);
			alloc.construct(a, p, s);
			allocationMap[p] = a;
			allocations.push_back(a);
			_total += s;
		}
		
		return p;
	}
	
	void* Memory::AllocateArray(size_t s, bool meta)
	{
		auto p = malloc(s);
		
		if (__initted && !meta)
		{
			if (s == 0)
			{
				out << "\n\nZero byte allocation requested, granted at " << p << "\n\n" << std::endl;
				while (true) ;
			}
			Allocation* a = alloc.allocate(1);
			alloc.construct(a, p, s, true);
			allocationMap[p] = a;
			allocations.push_back(a);
			_total += s;
		}
		
		return p;
	}
	
	void Memory::Release(void* ptr, bool meta)
	{
		if (__initted && !meta)
		{
			if (!allocationMap.count(ptr))
			{
				out << "Releasing unallocated memory: " << ptr << std::endl;
				//while (true) ;
			}
			allocationMap.erase(ptr);
		}
		
		free(ptr);
	}
	
	void Memory::Release(void* ptr, size_t s, bool meta)
	{
		if (__initted && !meta)
		{
			if (!allocationMap.count(ptr))
			{
				out << "Releasing unallocated memory: " << ptr << std::endl;
				//while (true) ;
			}
			allocationMap.erase(ptr);
		}
		
		free(ptr);
	}
	
	void Memory::ReleaseArray(void* ptr, bool meta)
	{
		if (__initted && !meta)
		{
			if (!allocationMap.count(ptr))
			{
				out << "Releasing unallocated memory: " << ptr << std::endl;
				//while (true) ;
			}
			allocationMap.erase(ptr);
		}
		
		free(ptr);
	}
	
	void Memory::ReleaseArray(void* ptr, size_t s, bool meta)
	{
		if (__initted && !meta)
		{
			if (!allocationMap.count(ptr))
			{
				out << "Releasing unallocated memory: " << ptr << std::endl;
				//while (true) ;
			}
			allocationMap.erase(ptr);
		}
		
		
		free(ptr);
	}
	
	
	void Memory::Reset()
	{
		for (auto a : allocations)
		{
			alloc.destroy(a);
			alloc.deallocate(a, 1);
		}
		
		allocations.clear();
		allocationMap.clear();
		_total = 0;
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