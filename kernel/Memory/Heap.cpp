#include "Heap.hh"
#include <kernel/Memory.h>

namespace Kernel { namespace Memory
{
	Heap::Heap(addr_t start, addr_t end, size_t pSize, bool super, bool readonly) noexcept : _startAddr(start), _endAddr(end), pSize(pSize), _supervisor(super), _readonly(readonly)
	{
		
	}
	
	void* Heap::_realloc(void* p, size_t prev, size_t s, size_t alignment)
	{
		auto nPtr = alloc(s, alignment);
		if (!nPtr)
		{
			return nullptr;
		}
		
		#ifdef DEBUG
		auto res =
		#endif
		memcpy(nPtr, const_cast<const void*>(p), prev);
		#ifdef DEBUG
		ASSERT(res);
		#endif
		
		free(p);
		
		return nPtr;
	}
	
	
	
	
	
	void* Heap::realloc(void* p, size_t s, size_t alignment)
	{
		ASSERT(s > 0);
		if (p == nullptr)
		{
			return alloc(s, alignment);
		}
		size_t prev = allocated_size(p);
		if (prev == 0)
		{
			return nullptr;
		}
		
		return _realloc(p, prev, s, alignment);
	}
	
	
}
}