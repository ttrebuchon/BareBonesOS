#ifndef INCLUDED_MEMORY_HEAPS_BUDDYHEAP_HPP
#define INCLUDED_MEMORY_HEAPS_BUDDYHEAP_HPP

#include "BuddyHeap.hh"
#include "details/buddy_tree.hpp"

namespace Kernel::Memory
{
	namespace detail
	{
		inline addr_t align_heap_start(void*& paddr, size_t& len)
		{
			{
				size_t nlen = 1;
				while (nlen <= len)
				{
					nlen <<= 1;
				}
				nlen >>= 1;
				len = nlen;
			}
			assert(len > 0);
			addr_t oaddr = (addr_t)paddr;
			addr_t addr = oaddr;
			
			// TODO
			if (addr % 2)
			{
				++addr;
			}
			
			
			paddr = (void*)addr;
			len -= (addr - oaddr);
			
			return addr;
		}
		
		inline uint8_t required_order(size_t sz) noexcept
		{
			assert(sz > 0);
			for (uint8_t order = 0; order < 256; ++order)
			{
				if ((((size_t)1) << order) >= sz)
				{
					return order;
				}
			}
			
			assert(false);
			return 0;
		}
	}
	
	
	
	template <class Alloc, class Mutex>
	BuddyHeap<Alloc, Mutex>::BuddyHeap(void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz, float eff) : BuddyHeap(allocator_type(), mem, len, kernel_mem, default_read_only, pg_sz, eff)
	{
		
	}
	
	
	template <class Alloc, class Mutex>
	BuddyHeap<Alloc, Mutex>::BuddyHeap(const allocator_type& a, void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz, float efficiency) : Heap(detail::align_heap_start(mem, len), ((addr_t)mem + len), pg_sz, kernel_mem, default_read_only), tree(mem, detail::required_order(len), a), tree_m(), highest_order(0), lowest_order(0)
	{
		assert(len >= 512);
		
		
		calculate_bounds(mem, len, &lowest_order, &highest_order, efficiency);
		assert(detail::required_order(len) == highest_order);
		assert(highest_order - lowest_order > 0);
	}
	
	
	template <class Alloc, class Mutex>
	BuddyHeap<Alloc, Mutex>::~BuddyHeap()
	{
		
	}
	
	
	
	template <class Alloc, class Mutex>
	void* BuddyHeap<Alloc, Mutex>::alloc(size_t sz, size_t alignment)
	{
		if (!sz || full())
		{
			return nullptr;
		}
		uint8_t order = detail::required_order(sz);
		assert(order < 128);
		
		if (order < lowest_order)
		{
			order = lowest_order;
		}
		
		Utils::lock_guard<mutex_type> lock(tree_m);
		
		auto n = tree.allocate_node(order);
		if (!n)
		{
			return nullptr;
		}
		else
		{
			return n->start();
		}
	}
	
	template <class Alloc, class Mutex>
	void BuddyHeap<Alloc, Mutex>::free(void* ptr)
	{
		if (!ptr && startAddr() > 0)
		{
			return;
		}
		
		Utils::lock_guard<mutex_type> lock(tree_m);
		
		auto n = tree.find_leaf_node(ptr);
		assert(n);
		assert(n->full);
		tree.release_node(n);
	}
	
	template <class Alloc, class Mutex>
	size_t BuddyHeap<Alloc, Mutex>::allocated_size(void* addr) const noexcept
	{
		Utils::lock_guard<mutex_type> lock(tree_m);
		auto n = tree.find_leaf_node(addr);
		if (n)
		{
			return n->size();
		}
		else
		{
			return 0;
		}
	}
	
	
	
	
	
	
	
	template <class Alloc, class Mutex>
	constexpr size_t BuddyHeap<Alloc, Mutex>::pre_calc_max_node_count(void* mem, size_t len, float efficiency) noexcept
	{
		uint8_t lowest_order = 0;
		uint8_t highest_order = 0;
		
		calculate_bounds(mem, len, &lowest_order, &highest_order, efficiency);
		
		return (1 << ((highest_order - lowest_order) + 1)) - 1;
		
	}
	
	
	
	template <class Alloc, class Mutex>
	constexpr size_t BuddyHeap<Alloc, Mutex>::pre_calc_max_meta_size(void* mem, size_t len, float efficiency) noexcept
	{
		return sizeof(tree_node_type)*pre_calc_max_node_count(mem, len, efficiency);
	}
	
	
	
	template <class Alloc, class Mutex>
	constexpr void BuddyHeap<Alloc, Mutex>::calculate_bounds(void* mem, size_t len, uint8_t* low, uint8_t* high, float efficiency) noexcept
	{
		detail::align_heap_start(mem, len);
		uint8_t highest_order = 0;
		
		size_t val = 1;
		while (val <= len)
		{
			++highest_order;
			val <<= 1;
		}
		val >>= 1;
		assert(highest_order > 2);
		--highest_order;
		
		assert(detail::required_order(len) == highest_order);
		
		
		uint8_t order_delta = 0;
		if (efficiency >= 1.0f)
		{
			order_delta = 0;
		}
		else if (efficiency > 0)
		{
			auto log_val = ((long double)len)/sizeof(tree_node_type)*(1 - efficiency) + 1;
			assert(log_val >= 0);
			auto lval = (8*sizeof(unsigned long long) - __builtin_clzll((unsigned long long)log_val) - 1);
			if ((1 << lval) > (unsigned long long)log_val)
			{
				--lval;
			}
			assert(lval < (1 << 7));
			assert(lval > 0);
			order_delta = (uint8_t)(lval - 1);
		}
		else if (efficiency == 0)
		{
			order_delta = highest_order;
		}
		else
		{
			val = 1;
			while (val <= sizeof(tree_node_type))
			{
				++order_delta;
				val <<= 1;
			}
			assert(highest_order > order_delta);
			assert(order_delta > 1);
			order_delta = highest_order - order_delta;
		}
		
		
		
		uint8_t lowest_order = 0;
		if (order_delta < highest_order)
		{
			lowest_order = highest_order - order_delta;
		}
		else
		{
			lowest_order = 0;
		}
		
		if (high)
		{
			*high = highest_order;
		}
		
		if (low)
		{
			*low = lowest_order;
		}
	}
	
	
}


#endif