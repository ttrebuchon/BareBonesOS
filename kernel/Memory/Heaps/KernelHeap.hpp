#ifndef INCLUDED_MEMORY_HEAPS_KERNELHEAP_HPP
#define INCLUDED_MEMORY_HEAPS_KERNELHEAP_HPP

#include "KernelHeap.hh"
#include "BuddyHeap.hpp"
#include <kernel/Memory/PageDir.hh>
#include "SlabHeap.hh"

namespace Kernel::Memory
{
	template <class Alloc>
	kernel_heap<Alloc>::kernel_heap(uintptr_t start, uintptr_t end, const allocator_type& alloc) : 
		Heap(start, end, true, false, 0),
		page_directories_m(),
		page_directories(alloc),
		top(alloc, (void*)start, end-start, true, false),
		heaps_m(),
		heaps(alloc),
		heaps_set(alloc),
		allocator(alloc)
	{
		initialize_default_heaps();
	}
	
	template <class Alloc>
	kernel_heap<Alloc>::kernel_heap(void* start, void* end, const allocator_type& alloc) : kernel_heap((uintptr_t)start, (uintptr_t)end, alloc)
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
	void* kernel_heap<Alloc>::alloc(size_t size, size_t alignment)
	{
		if (unlikely(size == 0))
		{
			return nullptr;
		}
		else if (size <= smallest_size)
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
		auto haddr = top.allocation_info(addr);
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
		}
	}
	
	template <class Alloc>
	size_t kernel_heap<Alloc>::allocated_size(void* addr) const noexcept
	{
		Utils::shared_lock<shared_mutex_type> slock(heaps_m);
		auto haddr = top.allocation_info(addr);
		if (unlikely(!haddr))
		{
			return 0;
		}
		auto heap = reinterpret_cast<Heap*>(haddr);
		if (likely(heaps_set.count(heap)))
		{
			return heap->allocated_size(addr);
		}
		else
		{
			return top.allocated_size(addr);
		}
		assert(NOT_IMPLEMENTED);
	}
}

#endif