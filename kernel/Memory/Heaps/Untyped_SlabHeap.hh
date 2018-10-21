#ifndef INCLUDED_MEMORY_HEAPS_UNTYPED_SLABHEAP_HH
#define INCLUDED_MEMORY_HEAPS_UNTYPED_SLABHEAP_HH

#include <Common.h>
#include <kernel/Memory/Heap.hh>
#include <Utils/Allocator.hh>
#include <Utils/Bitset.hh>
#include <Utils/mutex>
#include <kernel/Memory/alloc_spec.h>


#ifndef SLAB_SIGNATURE
	#define SLAB_SIGNATURE 0x51AB51AB
#endif

namespace Kernel::Memory
{
	
	namespace untyped_slab_detail
	{
		struct slab_t
		{
			uint32_t signature;
			uint16_t free_count;
			uint16_t free_mask;
			alloc_spec_t spec;
			
			//static_assert((alignof(T) == sizeof(T)) || (sizeof(T) % alignof(T) == 0));
			uint8_t region[/*sizeof(T)*16*/] __attribute__((__aligned__(alignof(void*))));
			
			slab_t(const alloc_spec_t& spec) : signature(SLAB_SIGNATURE), free_count(16), free_mask(0xFFFF), spec(spec), region()
			{}
			
			uint8_t* allocate(bool* full);
			bool free(uint32_t index, bool* empty);
			
			
			static slab_t* create(void* region, const alloc_spec_t& spec)
			{
				return new (region) slab_t(spec);
			}
		};
		
		
		
		
	}
	
	
	template <class Meta_Alloc = Utils::allocator<void>>
	class Untyped_SlabHeap : public Heap
	{
		public:
		typedef Meta_Alloc allocator_type;
		typedef untyped_slab_detail::slab_t slab_type;
		
		
		protected:
		
		typedef typename allocator_type::template rebind<uint32_t>::other bits_allocator_type;
		
		alloc_spec_t spec;
		size_t _slab_size;
		size_t _slab_count;
		size_t bit_count;
		slab_type* _slabs;
		bits_allocator_type bits_alloc;
		Utils::Bitset_Ptr<uint32_t> full_set, empty_set, partial_set;
		Utils::mutex sets_m;
		
		slab_type* get_slab(uint32_t i)
		{
			return (slab_type*)(((uint8_t*)_slabs) + i*slab_size());
		}
		size_t slab_size() const
		{
			return _slab_size;
		}
		
		
		public:
		Untyped_SlabHeap(const alloc_spec_t& spec, void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz = 0);
		Untyped_SlabHeap(const alloc_spec_t& spec, const allocator_type&, void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz = 0);
		
		virtual ~Untyped_SlabHeap();
		
		
		virtual void* alloc(size_t size, size_t alignment = 0) override;
		virtual void free(void*) override;
		virtual size_t allocated_size(void*) const noexcept override;
		
		__attribute__((__always_inline__))
		size_t slab_count() const noexcept
		{ return _slab_count; }
		
		
		static size_t Available_Count(const alloc_spec_t& spec, size_t len) noexcept;
		static size_t Size_For_Count(const alloc_spec_t& spec, size_t count) noexcept;
	};
	
	
}

#include "Untyped_SlabHeap.hpp"

#endif