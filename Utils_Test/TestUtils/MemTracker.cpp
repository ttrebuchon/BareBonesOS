#include "MemTracker.hh"
#include <iostream>
#include <kernel/Memory.h>


#ifdef __USE_MEM_POOL__
	#ifdef TRACK_ALLOC
		#error Cannot use both TRACK_ALLOC and __USE_MEM_POOL__
	#endif
#endif

using Memory = typename QA::Memory;
	
	
	
	template <class T>
	using MAllocator = Memory::MetaAllocator<T>;
	
	std::map<void*, Memory::Allocation*, std::less<void*>, MAllocator<std::pair<void* const, Memory::Allocation*>>> Memory::allocationMap;
	std::list<Memory::Allocation*, MAllocator<Memory::Allocation*>> Memory::allocations;
		
	MAllocator<Memory::Allocation> Memory::alloc;
	bool Memory::__initted = false;
	size_t Memory::_total = 0;
	bool Memory::__print_allocs = false;
	
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
			if (__print_allocs)
			{
				out << "Allocating " << p << " {" << s << "}" << std::endl;
			}
			
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
			if (__print_allocs)
			{
				out << "Allocating (Array) " << s << std::endl;
			}
			
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
			if (__print_allocs)
			{
				out << "Freeing " << ptr << std::endl;
			}
			
			/*if (!allocationMap.count(ptr))
			{
				out << "Releasing unallocated memory: " << ptr << std::endl;
				//while (true) ;
			}*/
			allocationMap.erase(ptr);
		}
		
		free(ptr);
	}
	
	void Memory::Release(void* ptr, size_t s, bool meta)
	{
		if (__initted && !meta)
		{
			if (__print_allocs)
			{
				out << "Freeing " << ptr << std::endl;
			}
			
			/*if (!allocationMap.count(ptr))
			{
				//out << "Releasing unallocated memory: " << ptr << std::endl;
				//while (true) ;
			}*/
			allocationMap.erase(ptr);
		}
		
		free(ptr);
	}
	
	void Memory::ReleaseArray(void* ptr, bool meta)
	{
		if (__initted && !meta)
		{
			if (__print_allocs)
			{
				out << "Freeing (Array) " << ptr << std::endl;
			}
			
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
			if (__print_allocs)
			{
				out << "Freeing (Array) " << ptr << std::endl;
			}
			
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
	
	bool Memory::SetPrintAllocs(bool value) noexcept
	{
		std::swap(value, __print_allocs);
		return value;
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

void* operator new(size_t size, const std::nothrow_t&)
{
    return QA::Memory::Allocate(size);
}

void* operator new[](size_t size, const std::nothrow_t&)
{
	return QA::Memory::AllocateArray(size);
}

void* operator new(size_t size, std::align_val_t al)
{
	auto ptr = QA::Memory::Allocate(size);
	assert(ptr);
	assert((addr_t)ptr % (size_t)al == 0);
	return ptr;
}

void* operator new[](size_t size, std::align_val_t al)
{
	auto ptr = QA::Memory::AllocateArray(size);
	assert(ptr);
	assert((addr_t)ptr % (size_t)al == 0);
	return ptr;
}

void* operator new(size_t size, std::align_val_t al, const std::nothrow_t&)
{
	auto ptr = QA::Memory::Allocate(size);
	assert(ptr);
	assert((addr_t)ptr % (size_t)al == 0);
	return ptr;
}

void* operator new[](size_t size, std::align_val_t al, const std::nothrow_t&)
{
	auto ptr = QA::Memory::AllocateArray(size);
	assert(ptr);
	assert((addr_t)ptr % (size_t)al == 0);
	return ptr;
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

void operator delete(void* ptr, std::align_val_t al)
{
	QA::Memory::Release(ptr);
}
#endif

static bool mem_pool_initialized = false;

bool QA::MemPoolEnabled() noexcept
{
	return ::mem_pool_initialized;
}

void QA::EnableMemPool() noexcept
{
	::mem_pool_initialized = true;
}

void QA::DisableMemPool() noexcept
{
	::mem_pool_initialized = false;
}

#ifdef __USE_MEM_POOL__
void* operator new(size_t size)
{
	if (!mem_pool_initialized)
	{
		return malloc(size);
	}
    return kmalloc(size, 0, 0);
}

void* operator new[](size_t size)
{
	if (!mem_pool_initialized)
	{
		return malloc(size);
	}
	return kmalloc(size, 0, 0);
}

void* operator new(size_t size, std::align_val_t al)
{
	void* ptr = nullptr;
	/*if ((size_t)al < 2)
	{
		al = (std::align_val_t)sizeof(void*);
	}*/
	if (!mem_pool_initialized)
	{
		assert(size > 0);
		ptr = malloc(size);
		/*int err;
		err = posix_memalign(&ptr, (size_t)al, size);
		if (err != 0)
		{
			QA::out << "No. " << err << std::endl;
			QA::out << "EINVAL = " << EINVAL << "\nENOMEM = " << ENOMEM << std::endl;
			QA::out << "Al: " << (size_t)al << std::endl;
		}
		assert(ptr);*/
	}
	else
	{
		ptr = kmalloc(size, (size_t)al, 0);
	}
	assert(ptr);
	if ((addr_t)ptr % (size_t)al != 0)
	{
		QA::out << (void*)ptr << std::endl << (size_t)al << std::endl;
	}
	assert((addr_t)ptr % (size_t)al == 0);
	return ptr;
}

void operator delete(void* ptr)
{
	assert(ptr);
	if (!mem_pool_initialized)
	{
		free(ptr);
		return;
	}
    kfree(ptr);
}

void operator delete(void* ptr, size_t s)
{
	assert(ptr);
	if (!mem_pool_initialized)
	{
		free(ptr);
		return;
	}
	kfree(ptr);
}

void operator delete[](void* ptr)
{
	assert(ptr);
	if (!mem_pool_initialized)
	{
		free(ptr);
		return;
	}
    kfree(ptr);
}

void operator delete[](void* ptr, size_t s)
{
	assert(ptr);
	if (!mem_pool_initialized)
	{
		free(ptr);
		return;
	}
    kfree(ptr);
}

void operator delete(void* ptr, std::align_val_t al)
{
	assert(ptr);
	if (!mem_pool_initialized)
	{
		free(ptr);
		return;
	}
    kfree(ptr);
}
#endif

#if !defined(__USE_MEM_POOL__) && !defined(TRACK_ALLOC)
void* operator new(size_t size, std::align_val_t al)
{
	assert(size > 0);
	void* ptr = malloc(size);
	assert(ptr);
	assert((addr_t)ptr % (size_t)al == 0);
	return ptr;
}

void operator delete(void* ptr, std::align_val_t al)
{
	assert(ptr);
    free(ptr);
}
#endif