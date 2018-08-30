#ifndef INCLUDED_MEMORY_HEAPS_KERNELHEAP_HPP
#define INCLUDED_MEMORY_HEAPS_KERNELHEAP_HPP

#include "KernelHeap.hh"
#include "BuddyHeap.hpp"
#include <kernel/Memory/PageDir.hh>
#include "SlabHeap.hh"
#include "PageHeap.hpp"

namespace Kernel::Memory
{
	template <class Alloc>
	kernel_heap<Alloc>::kernel_heap(PageDirectory* pdir, uintptr_t start, uintptr_t end, const allocator_type& alloc) : 
		Heap(start, end, 0, true, false),
		page_directories_m(),
		page_directories(alloc),
		top(alloc, (void*)start, end-start, true, false),
		paged(alloc, pdir, true, false),
		heaps_m(),
		heaps(alloc),
		heaps_set(alloc),
		allocator(alloc)
	{
		assert(pageSize() > 0);
		initialize_default_heaps();
	}
	
	template <class Alloc>
	kernel_heap<Alloc>::kernel_heap(PageDirectory* pdir, void* start, void* end, const allocator_type& alloc) : kernel_heap(pdir, (uintptr_t)start, (uintptr_t)end, alloc)
	{
		
	}
	
	template <class Alloc>
	kernel_heap<Alloc>::kernel_heap(uintptr_t start, uintptr_t end, const allocator_type& alloc) : kernel_heap(PageDirectory::Current, start, end, alloc)
		
	{
		
	}
	
	template <class Alloc>
	kernel_heap<Alloc>::kernel_heap(void* start, void* end, const allocator_type& alloc) : kernel_heap(PageDirectory::Current, (uintptr_t)start, (uintptr_t)end, alloc)
	{
		
	}
	
	template <class Alloc>
	kernel_heap<Alloc>::~kernel_heap()
	{
		for (auto& heap : heaps_set)
		{
			heap->~Heap();
		}
		
		heaps_set.clear();
		
		list_alloc_type<Heap*> lalloc(allocator);
		for (auto& lip : heaps)
		{
			auto li = lip.second;
			auto tmp = li;
			while (li)
			{
				tmp = li;
				li = li->next;
				lalloc.destroy(tmp);
				lalloc.deallocate(tmp, 1);
			}
		}
		
		heaps.clear();
	}
	
	
	template <class Alloc>
	void kernel_heap<Alloc>::initialize_default_heaps()
	{
		Utils::lock_guard<shared_mutex_type> hlock(heaps_m);
		auto smallest_a = top.smallest_alloc();
		auto len = top.endAddr() - top.startAddr();
		
		// Setup quad-word heap
		{
			typedef SlabHeap_N<sizeof(void*)*4, alignof(void*), allocator_type> sheap_t;
			size_t req_len = len / 2;
			auto count = sheap_t::Available_Count(req_len);
			req_len = sheap_t::Size_For_Count(count);
			
			
			while (req_len + sizeof(sheap_t) > len/2)
			{
				count -= 10;
				assert(count > 0);
				req_len = sheap_t::Size_For_Count(count);
			}
			
			
			
			auto region = top.allocate(req_len + sizeof(sheap_t), alignof(sheap_t));
			req_len = top.allocated_size(region) - sizeof(sheap_t);
			assert(req_len + sizeof(sheap_t) <= len/2);
			count = sheap_t::Available_Count(req_len - sizeof(sheap_t));
			
			auto sheap = new (region) sheap_t(allocator, (void*)((uintptr_t)region + sizeof(sheap_t)), req_len, true, false, pageSize());
			assert(sheap);
			list_alloc_type<Heap*> lalloc;
			auto list = lalloc.allocate(1);
			lalloc.construct(list);
			list->entity = sheap;
			heaps[4*sizeof(void*)] = list;
			heaps_set.insert(sheap);
		}
		
		//assert(NOT_IMPLEMENTED);
	}
	
	template <class Alloc>
	const Heap* kernel_heap<Alloc>::heap_for(const alloc_spec_t& spec) const
	{
		if (unlikely(spec.size == 0))
		{
			return nullptr;
		}
		
		if (spec.size <= smallest_size)
		{
			auto heap_li = heaps.at(smallest_size);
			assert(heap_li);
			auto heap = heap_li->entity;
			assert(heap);
			assert(spec.align <= alignof(void*));
			
			return heap;
		}
		else if (spec.size >= pageSize())
		{
			if (unlikely(spec.size % pageSize() == 0) && spec.size >= 10*pageSize())
			{
				return &paged;
			}
			else
			{
				return &top;
			}
		}
	}
	
	template <class Alloc>
	const Heap* kernel_heap<Alloc>::heap_for(void* addr) const
	{
		if (unlikely((uintptr_t)addr < top.startAddr() || (uintptr_t)addr >= top.endAddr()))
		{
			return &paged;
		}
		
		
		auto haddr = top.allocation_info(addr);
		if (unlikely(!haddr))
		{
			return nullptr;
		}
		auto heap = reinterpret_cast<Heap*>(haddr);
		if (likely(heaps_set.count(heap)))
		{
			return heap;
		}
		else
		{
			return &top;
		}
	}
	
	template <class Alloc>
	Heap* kernel_heap<Alloc>::heap_for(const alloc_spec_t& spec)
	{
		if (unlikely(spec.size == 0))
		{
			return nullptr;
		}
		
		if (spec.size <= smallest_size)
		{
			auto heap_li = heaps.at(smallest_size);
			assert(heap_li);
			auto heap = heap_li->entity;
			assert(heap);
			assert(spec.align <= alignof(void*));
			
			return heap;
		}
		else if (spec.size >= pageSize())
		{
			if (unlikely(spec.size % pageSize() == 0) && spec.size >= 10*pageSize())
			{
				return &paged;
			}
			else
			{
				return &top;
			}
		}
		
		assert(NOT_IMPLEMENTED);
	}
	
	template <class Alloc>
	Heap* kernel_heap<Alloc>::heap_for(void* addr)
	{
		if (unlikely((uintptr_t)addr < top.startAddr() || (uintptr_t)addr >= top.endAddr()))
		{
			return &paged;
		}
		
		
		auto haddr = top.allocation_info(addr);
		if (unlikely(!haddr))
		{
			return nullptr;
		}
		auto heap = reinterpret_cast<Heap*>(haddr);
		if (likely(heaps_set.count(heap)))
		{
			return heap;
		}
		else
		{
			return &top;
		}
	}
	
	
	
	
	
	
	
	
	
	template <class Alloc>
	void* kernel_heap<Alloc>::alloc(size_t size, size_t alignment)
	{
		if (unlikely(size == 0))
		{
			return nullptr;
		}
		else
		{
			Utils::shared_lock<shared_mutex_type> slock(heaps_m);
			auto heap = heap_for(alloc_spec_t(size, alignment));
			assert(heap);
			return heap->alloc(size, alignment);
		}
		
		
		
		/*else if (size <= smallest_size)
		{
			Utils::shared_lock<shared_mutex_type> slock(heaps_m);
			auto heap_li = heaps.at(smallest_size);
			assert(heap_li);
			auto heap = heap_li->entity;
			assert(heap);
			assert(alignment <= alignof(void*));
			
			return heap->allocate(smallest_size, alignof(void*));
			
			size = smallest_size;
		}
		else if (size >= pageSize())
		{
			if (unlikely(size % pageSize() == 0))
			{
				auto ptr = paged.alloc(size, alignment);
				assert(ptr);
				return ptr;
			}
		}
		
		if (unlikely(alignment < alignof(void*)))
		{
			alignment = alignof(void*);
		}
		
		size_t sa;
		
		if (size >= (sa = top.smallest_alloc()))
		{
			if (unlikely(size == sa))
			{
				return top.allocate(size, alignment);
			}
			
			return top.allocate(size, alignment);
			
			assert(NOT_IMPLEMENTED);
		}
		
		
		*/
		
		assert(NOT_IMPLEMENTED);
	}
	
	
	template <class Alloc>
	void kernel_heap<Alloc>::free(void* addr)
	{
		if (unlikely(!addr))
		{
			return;
		}
		
		Utils::shared_lock<shared_mutex_type> slock(heaps_m);
		
		auto heap = heap_for(addr);
		assert(heap);
		heap->free(addr);
		return;
		
		/*auto haddr = top.allocation_info(addr);
		assert(haddr);
		auto heap = reinterpret_cast<Heap*>(haddr);
		if (likely(heaps_set.count(heap)))
		{
			heap->free(addr);
			
			// TODO: Check if heap was full
			// and adjust as necessary
		}
		else if (haddr == addr)
		{
			top.free(addr);
		}
		else
		{
			heap = nullptr;
			
			// TODO ...
			
			assert(NOT_IMPLEMENTED);
		}*/
	}
	
	template <class Alloc>
	size_t kernel_heap<Alloc>::allocated_size(void* addr) const noexcept
	{
		Utils::shared_lock<shared_mutex_type> slock(heaps_m);
		
		auto heap = heap_for(addr);
		if (likely(heap != nullptr))
		{
			return heap->allocated_size(addr);
		}
		else
		{
			return 0;
		}
		
		/*auto haddr = top.allocation_info(addr);
		if (unlikely(!haddr))
		{
			return 0;
		}
		auto heap = reinterpret_cast<Heap*>(haddr);
		if (likely(heaps_set.count(heap)))
		{
			return heap->allocated_size(addr);
		}
		else if (((uintptr_t)addr >= top.startAddr()) && ((uintptr_t)addr < top.endAddr()))
		{
			return top.allocated_size(addr);
		}
		else
		{
			return paged.allocated_size(addr);
		}
		assert(NOT_IMPLEMENTED);*/
	}
}

#endif