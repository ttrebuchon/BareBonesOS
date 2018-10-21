#ifndef INCLUDED_MEMORY_HEAPS_UNTYPED_SLABHEAP_HPP
#define INCLUDED_MEMORY_HEAPS_UNTYPED_SLABHEAP_HPP

#include "Untyped_SlabHeap.hh"




namespace Kernel::Memory
{
	
	template <class Meta_Alloc>
	Untyped_SlabHeap<Meta_Alloc>::Untyped_SlabHeap(const alloc_spec_t& spec, void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz) : Untyped_SlabHeap(spec, allocator_type(), mem, len, kernel_mem, default_read_only, pg_sz)
	{}
	
	template <class Meta_Alloc>
	Untyped_SlabHeap<Meta_Alloc>::Untyped_SlabHeap(const alloc_spec_t& spec, const allocator_type& a, void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz) : Heap((addr_t)mem, (addr_t)mem + len, pg_sz, kernel_mem, default_read_only), spec(spec), _slab_count(len / (sizeof(slab_type) + alloc_spec_req_size(spec, 16))), bit_count((_slab_count / 32) + ((_slab_count % 32) ? 1 : 0)), _slabs(nullptr), bits_alloc(a), full_set(bits_alloc.allocate(bit_count), bit_count, _slab_count), empty_set(bits_alloc.allocate(bit_count), bit_count, _slab_count), partial_set(bits_alloc.allocate(bit_count), bit_count, _slab_count),
		sets_m()
	{
		assert(_slab_count > 0);
		empty_set.setAll(true);
		
		_slab_size = sizeof(slab_type) + alloc_spec_req_size(spec, 16);
		
		for (size_t i = 0; i < _slab_count; ++i)
		{
			new ((slab_type*)((uint8_t*)mem + i*slab_size())) slab_type(spec);
		}
		
		_slabs = (slab_type*)mem;
	}
	
	template <class Meta_Alloc>
	Untyped_SlabHeap<Meta_Alloc>::~Untyped_SlabHeap()
	{
		bits_alloc.deallocate(full_set.bits, bit_count);
		full_set.bits = nullptr;
		bits_alloc.deallocate(empty_set.bits, bit_count);
		empty_set.bits = nullptr;
		
		bits_alloc.deallocate(partial_set.bits, bit_count);
		partial_set.bits = nullptr;
	}
	
	
	
	template <class Meta_Alloc>
	void* Untyped_SlabHeap<Meta_Alloc>::alloc(size_t size, size_t alignment)
	{
		if (size == 0)
		{
			return nullptr;
		}
		if (size > spec.size)
		{
			TRACE(size);
		}
		assert(size <= spec.size);
		assert(alignment == 0 || (spec.align % alignment == 0));
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
			uint8_t* addr = get_slab(index)->allocate(&now_full);
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
	
	template <class Meta_Alloc>
	void Untyped_SlabHeap<Meta_Alloc>::free(void* addr)
	{
		if (addr == nullptr)
		{
			return;
		}
		assert((addr_t)addr > startAddr());
		assert((addr_t)addr < endAddr());
		
		auto offset = (addr_t)addr - startAddr();
		auto index = offset / (slab_size());//sizeof(slab_type);
		auto slab_off = offset - (index*slab_size());
		assert(slab_off >= offsetof(slab_type, region));
		
		bool now_empty;
		if (!get_slab(index)->free((slab_off - offsetof(slab_type, region)) / spec.size, &now_empty))
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
	
	template <class Meta_Alloc>
	size_t Untyped_SlabHeap<Meta_Alloc>::allocated_size(void*) const noexcept
	{
		return alloc_spec_req_size(spec, 16)/16;
	}
	
	
	
	template <class Meta_Alloc>
	size_t Untyped_SlabHeap<Meta_Alloc>::Available_Count(const alloc_spec_t& spec, size_t len) noexcept
	{
		return (len / (sizeof(slab_type) + alloc_spec_req_size(spec, 16)))*16;
	}
	
	template <class Meta_Alloc>
	size_t Untyped_SlabHeap<Meta_Alloc>::Size_For_Count(const alloc_spec_t& spec, size_t count) noexcept
	{
		size_t slab_count = (count / 16) + (count % 16 != 0 ? 1 : 0);
		assert(Available_Count(spec, slab_count*(sizeof(slab_type) + alloc_spec_req_size(spec, 16))) >= count);
		return slab_count * (sizeof(slab_type) + alloc_spec_req_size(spec, 16));
	}
	
	
}

#endif