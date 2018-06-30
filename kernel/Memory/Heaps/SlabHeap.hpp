#ifndef INCLUDED_MEMORY_HEAPS_SLABHEAP_HPP
#define INCLUDED_MEMORY_HEAPS_SLABHEAP_HPP

#include "SlabHeap.hh"




namespace Kernel::Memory
{
	namespace slab_detail
	{
		#define __MASK(I) (1 << (15 - index))
		template <class T>
		uint8_t* slab_t<T>::allocate(bool* full)
		{
			assert(signature == SLAB_SIGNATURE);
			assert(full);
			*full = false;
			assert(free_count > 0);
			assert(free_count <= 16);
			assert(free_mask);
			
			uint8_t index;
			for (index = 0; index < 16; ++index)
			{
				if (free_mask & __MASK(index))
				{
					break;
				}
			}
			assert(index < 16);
			assert(((uint16_t)__MASK(index)) > 0);
			
			if (--free_count == 0)
			{
				*full = true;
			}
			free_mask &= ~__MASK(index);
			
			return (region + sizeof(T)*index);
		}
		
		
		template <class T>
		bool slab_t<T>::free(uint32_t index, bool* empty)
		{
			assert(signature == SLAB_SIGNATURE);
			assert(empty);
			assert(index < 16);
			*empty = false;
			
			if (~free_mask & __MASK(index))
			{
				free_mask |= __MASK(index);
				if (++free_count == 16)
				{
					*empty = true;
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		
		#undef __MASK
	
	}
	
	
	
	template <class T, class Meta_Alloc>
	SlabHeap<T, Meta_Alloc>::SlabHeap(void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz) : SlabHeap(allocator_type(), mem, len, kernel_mem, default_read_only, pg_sz)
	{}
	
	template <class T, class Meta_Alloc>
	SlabHeap<T, Meta_Alloc>::SlabHeap(const allocator_type& a, void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz) : Heap((addr_t)mem, (addr_t)mem + len, pg_sz, kernel_mem, default_read_only), _slab_count(len / sizeof(slab_type)), bit_count((_slab_count / 32) + ((_slab_count % 32) ? 1 : 0)), _slabs(nullptr), bits_alloc(a), full_set(bits_alloc.allocate(bit_count), bit_count, _slab_count), empty_set(bits_alloc.allocate(bit_count), bit_count, _slab_count), partial_set(bits_alloc.allocate(bit_count), bit_count, _slab_count),
		sets_m()
	{
		assert(_slab_count > 0);
		empty_set.setAll(true);
		
		for (size_t i = 0; i < _slab_count; ++i)
		{
			new (&((slab_type*)mem)[i]) slab_type();
		}
		
		_slabs = (slab_type*)mem;
	}
	
	template <class T, class Meta_Alloc>
	SlabHeap<T, Meta_Alloc>::~SlabHeap()
	{
		bits_alloc.deallocate(full_set.bits, bit_count);
		full_set.bits = nullptr;
		bits_alloc.deallocate(empty_set.bits, bit_count);
		empty_set.bits = nullptr;
		
		bits_alloc.deallocate(partial_set.bits, bit_count);
		partial_set.bits = nullptr;
	}
	
	
	
	template <class T, class Meta_Alloc>
	void* SlabHeap<T, Meta_Alloc>::alloc(size_t size, size_t alignment)
	{
		if (size == 0)
		{
			return nullptr;
		}
		if (size > sizeof(T))
		{
			TRACE(size);
		}
		assert(size <= sizeof(T));
		assert(alignment == 0 || (alignof(T) % alignment == 0));
		uint32_t index;
		bool found;
		Utils::lock_guard<Utils::mutex> lock(sets_m);
		if (!(found = partial_set.firstTrue(&index)))
		{
			if ((found = empty_set.firstTrue(&index)))
			{
				empty_set.set(index, false);
				partial_set.set(index, true);
			}
		}
		
		
		if (found)
		{
			assert(index < _slab_count);
			bool now_full;
			uint8_t* addr = _slabs[index].allocate(&now_full);
			if (now_full)
			{
				partial_set.set(index, false);
				full_set.set(index, true);
			}
			return addr;
		}
		else
		{
			assert(!full_set.firstFalse(&index));
			return nullptr;
		}
	}
	
	template <class T, class Meta_Alloc>
	void SlabHeap<T, Meta_Alloc>::free(void* addr)
	{
		if (addr == nullptr)
		{
			return;
		}
		assert((addr_t)addr > startAddr());
		assert((addr_t)addr < endAddr());
		
		auto offset = (addr_t)addr - startAddr();
		auto index = offset / sizeof(slab_type);
		auto slab_off = offset - (index*sizeof(slab_type));
		assert(slab_off >= offsetof(slab_type, region));
		
		bool now_empty;
		if (!_slabs[index].free((slab_off - offsetof(slab_type, region)) / sizeof(T), &now_empty))
		{
			// TODO: Handle freeing already-free
			//return;
			assert(NOT_IMPLEMENTED);
		}
		
		if (now_empty)
		{
			partial_set.set(index, false);
			empty_set.set(index, true);
		}
		else
		{
			full_set.set(index, false);
			partial_set.set(index, true);
		}
	}
	
	template <class T, class Meta_Alloc>
	size_t SlabHeap<T, Meta_Alloc>::allocated_size(void*) const noexcept
	{
		return sizeof(T);
	}
	
	
	
	template <class T, class Meta_Alloc>
	size_t SlabHeap<T, Meta_Alloc>::Available_Count(size_t len) noexcept
	{
		return (len / sizeof(slab_type))*16;
	}
	
	template <class T, class Meta_Alloc>
	size_t SlabHeap<T, Meta_Alloc>::Size_For_Count(size_t count) noexcept
	{
		size_t slab_count = (count / 16) + (count % 16 != 0 ? 1 : 0);
		assert(Available_Count(slab_count*sizeof(slab_type)) >= count);
		return slab_count * sizeof(slab_type);
	}
	
	
}

#endif