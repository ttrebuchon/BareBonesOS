#include "DumbHeap.hh"


namespace Kernel { namespace Memory
{
	
	DumbHeap::DumbHeap(addr_t start, addr_t end, bool super, bool readonly) noexcept : Heap(start, end, 0, super, readonly), _placement(start)
	{
		
	}
	
	DumbHeap::~DumbHeap() noexcept
	{
		// TODO
	}
	
	
	void* DumbHeap::alloc(size_t s, size_t al)
	{
		if (al > 0)
		{
			if (_placement % al != 0)
			{
				_placement -= (_placement % al);
				_placement += al;
				// constexpr addr_t iz = ~(size_t(0));
				// const addr_t mask = al*(iz/al);
				// _placement &= mask;
				// _placement += al;
				ASSERT(_placement % al == 0);
			}
			ASSERT(_placement % al == 0);
		}
		
		ASSERT(endAddr() - _placement >= s);
		void* ptr = (void*)_placement;
		_placement += s;
		
		return ptr;
	}
	
	void DumbHeap::free(void*)
	{
		return;
	}
	
	size_t DumbHeap::allocated_size(void*) const noexcept
	{
		ASSERT(false);
	}
}
}