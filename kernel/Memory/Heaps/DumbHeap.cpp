#include "DumbHeap.hh"
#include <kernel/Memory/PageDir.hh>


namespace Kernel { namespace Memory
{
	
	DumbHeap::DumbHeap(addr_t start, addr_t end, bool super, bool readonly) noexcept : Heap(start, end, PAGE_SIZE, super, readonly), _placement(start), furthest_paged(start)
	{
		// assert(pageSize() > 0);
		// assert(!readonly);
		// bool alloc_result;
		// for (addr_t page = start; page < end; page += pageSize())
		// {
		// 	auto pg = PageDirectory::Current->at(page, true);
		// 	if (!pg->present())
		// 	{
		// 		alloc_result = pg->allocate(!readonly, super);
		// 		assert(alloc_result);
		// 	}
		// }
	}
	
	DumbHeap::~DumbHeap() noexcept
	{
		// TODO
		assert(NOT_IMPLEMENTED);
	}
	
	
	void* DumbHeap::alloc(size_t sz, size_t al)
	{
		const size_t s = sz;
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
				assert(_placement % al == 0);
			}
			assert(_placement % al == 0);
		}

		if (_placement + s >= furthest_paged)
		{
			addr_t page = furthest_paged;
			while (page <= _placement + s + this->pageSize())
			{
				assert(page < endAddr());
				auto pg = PageDirectory::Current->at(page, true);
				if (!pg->present())
				{
					bool alloc_result = pg->allocate(!readonly(), this->supervisor());
					assert(alloc_result);
				}
				furthest_paged = page;
				page += this->pageSize();
			}
		}

		assert(endAddr() > _placement);
		if (!(endAddr() - _placement >= s))
		{
			TRACE((void*)endAddr());
			TRACE((void*)_placement);
			TRACE((void*)s);
		}
		assert(endAddr() - _placement >= s);
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
		assert(false);
	}
}
}