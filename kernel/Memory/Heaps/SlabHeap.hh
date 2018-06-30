#ifndef INCLUDED_MEMORY_HEAPS_SLABHEAP_HH
#define INCLUDED_MEMORY_HEAPS_SLABHEAP_HH

#include <Common.h>
#include <kernel/Memory/Heap.hh>
#include <Utils/Allocator.hh>
#include <Utils/Bitset.hh>
#include <Utils/mutex>


#define SLAB_SIGNATURE 0x51AB51AB

namespace Kernel::Memory
{
	
	namespace slab_detail
	{
		template <class T>
		struct slab_t
		{
			uint32_t signature;
			uint16_t free_count;
			uint16_t free_mask;
			
			static_assert((alignof(T) == sizeof(T)) || (sizeof(T) % alignof(T) == 0));
			uint8_t region[sizeof(T)*16] __attribute__((__aligned__(alignof(T))));
			
			slab_t() : signature(SLAB_SIGNATURE), free_count(16), free_mask(0xFFFF), region()
			{}
			
			uint8_t* allocate(bool* full);
			bool free(uint32_t index, bool* empty);
		};
		
		
		
		
	}
	
	template <size_t sz, size_t alignment = 1>
	struct alignas(alignment) slab_heap_item
	{
		uint8_t dat[sz];
	};// __attribute__((alignas(alignment)));
	
	
	template <class T, class Meta_Alloc = Utils::allocator<void>>
	class SlabHeap : public Heap
	{
		public:
		typedef T value_type;
		typedef Meta_Alloc allocator_type;
		typedef slab_detail::slab_t<T> slab_type;
		
		
		protected:
		
		typedef typename allocator_type::template rebind<uint32_t>::other bits_allocator_type;
		
		size_t _slab_count;
		size_t bit_count;
		slab_type* _slabs;
		bits_allocator_type bits_alloc;
		Utils::Bitset_Ptr<uint32_t> full_set, empty_set, partial_set;
		Utils::mutex sets_m;
		
		
		public:
		SlabHeap(void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz = 0);
		SlabHeap(const allocator_type&, void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz = 0);
		
		virtual ~SlabHeap();
		
		
		virtual void* alloc(size_t size, size_t alignment = 0) override;
		virtual void free(void*) override;
		virtual size_t allocated_size(void*) const noexcept override;
		
		__attribute__((__always_inline__))
		size_t slab_count() const noexcept
		{ return _slab_count; }
		
		
		static size_t Available_Count(size_t len) noexcept;
		static size_t Size_For_Count(size_t count) noexcept;
	};
	
	template <size_t size, size_t alignment = 0, class Meta_Alloc = Utils::allocator<void>>
	using SlabHeap_N = SlabHeap<slab_heap_item<size, alignment>, Meta_Alloc>;
	
	
}

#include "SlabHeap.hpp"

#endif