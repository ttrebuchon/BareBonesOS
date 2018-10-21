#ifndef INCLUDED_MEMORY_HEAPS_PAGEHEAP_HPP
#define INCLUDED_MEMORY_HEAPS_PAGEHEAP_HPP

#include "PageHeap.hh"
#include <kernel/Memory/PageDir.hh>
#include <kernel/Memory/PhysicalMemory.hh>

namespace Kernel::Memory
{
	template <class A, class M>
	PageHeap<A, M>::PageHeap(const allocator_type& alloc, PageDirectory* dir, bool kernel_only, bool default_readonly) :
		Heap(0, 0, PAGE_SIZE, kernel_only, default_readonly),
		allocator(alloc),
		pdir(dir),
		mut(),
		allocations(alloc),
		page_map(alloc),
		_free_count(0),
		_use_count(0)
	{
		assert(dir);
		assert(pageSize());
	}
	
	
	template <class A, class M>
	PageHeap<A, M>::PageHeap(PageDirectory* dir, bool kernel_only, bool default_readonly) :
		PageHeap(allocator_type(), dir, kernel_only, default_readonly)
	{
		
	}
	
	template <class A, class M>
	PageHeap<A, M>::~PageHeap()
	{
		Utils::lock_guard<mutex_type> lock(mut);
		for (auto & allocation : allocations)
		{
			if (!allocation.used)
			{
				continue;
			}
			
			size_t pages = allocation.length / pageSize();
			if (pages == 0)
			{
				pages = 1;
			}
			for (size_t i = 0; i < pages; ++i)
			{
				auto pg = pdir->at((void*)((uintptr_t)allocation.page + i*pageSize()));
				assert(pg);
				assert(pg->present());
				if (pg->present())
				{
					pg->deallocate();
				}
			}
		}
		
		allocations.clear();
	}
	
	
	
	// Public Member Functions
	
	template <class A, class M>
	auto PageHeap<A, M>::get_allocator() const noexcept -> const allocator_type&
	{
		return allocator;
	}
	
	template <class A, class M>
	void* PageHeap<A, M>::alloc(size_t size, size_t alignment)
	{
		assert(pageSize() == PAGE_SIZE);
		assert(size >= (3*pageSize())/4);
		
		auto pg_count = size / pageSize();
		if (size % pageSize())
		{
			++pg_count;
		}
		
		for (auto& a : allocations)
		{
			if (!a.used)
			{
				if (a.length/pageSize() == pg_count)
				{
					a.used = true;
					for (size_t i = 0; i < a.length; i += PAGE_SIZE)
					{
						void* addr = (void*)((uintptr_t)a.page + i);
						auto pg = pdir->at(addr);
						assert(pg);
						assert(!pg->present());
						pg->allocate(supervisor(), readonly());
						page_map[addr] = &a;
						--_free_count;
						++_use_count;
					}
					return a.page;
				}
			}
		}
		
		for (size_t t = 1; t < pdir->table_count(); ++t)
		{
			auto tbl = pdir->table(t);
			if (!tbl)
			{
				tbl = pdir->table(t, true);
			}
			
			assert(tbl);
			for (size_t p = 0; p < tbl->page_count(); ++p)
			{
				if (((pdir->table_count()-t)*tbl->page_count() - p) < pg_count)
				{
					break;
				}
				auto pg = &tbl->at(p);
				if (!pg->present())
				{
					size_t i;
					for (i = 1; i < pg_count; ++i)
					{
						auto tmp = &pdir->table(t + ((i+p)/tbl->page_count()), true)->at((p+i)%tbl->page_count());
						if (tmp->present())
						{
							break;
						}
					}
					
					if (i == pg_count)
					{
						TRACE("Found page region.");
						allocations.emplace_back();
						auto a = &allocations.back();
						//allocation a;
						a->page = pg->virtual_address();
						a->length = pg_count * pageSize();
						a->used = true;
						//allocations.push_back(a);
						
						for (i = 0; i < pg_count; ++i)
						{
							pg = &pdir->table(t + ((i+p)/tbl->page_count()), true)->at((p+i)%tbl->page_count());
							pg->allocate(supervisor(), readonly());
							assert(pg->present());
							page_map[pg->virtual_address()] = a;
							++_use_count;
							
						}
						TRACE("Mapped.");
						pg = &tbl->at(p);
						assert(pg);
						return pg->virtual_address();
						
					}
					else
					{
						p += (i-1);
						continue;
					}
				}
			}
		}
		
		assert(NOT_IMPLEMENTED);
	}
	
	template <class A, class M>
	void PageHeap<A, M>::free(void* addr)
	{
		assert(addr);
		Utils::lock_guard<mutex_type> lock(mut);
		auto allocation = page_map[addr];
		if (!allocation)
		{
			return;
		}
		assert(allocation);
		size_t pages = allocation->length / pageSize();
		if (pages == 0)
		{
			pages = 1;
		}
		
		for (size_t i = 0; i < pages; ++i)
		{
			void* pg_addr = (void*)((uintptr_t)allocation->page + i*pageSize());
			auto pg = pdir->at(pg_addr);
			assert(pg);
			assert(pg->present());
			if (pg->present())
			{
				pg->deallocate();
				++_free_count;
				--_use_count;
			}
			page_map.erase(pg_addr);
		}
		
		allocation->used = false;
		return;
		
		assert(NOT_IMPLEMENTED);
	}
	
	template <class A, class M>
	size_t PageHeap<A, M>::allocated_size(void*) const noexcept
	{
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	// Protected Member Functions
	
	template <class A, class M>
	template <class T>
	auto PageHeap<A, M>::get_alloc_T() const noexcept -> alloc_rebind<T>
	{
		return alloc_rebind<T>(get_allocator());
	}
		
}

#endif