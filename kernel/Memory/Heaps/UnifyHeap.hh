#ifndef INCLUDED_MEMORY_HEAPS_UNIFY_HEAP_HH
#define INCLUDED_MEMORY_HEAPS_UNIFY_HEAP_HH

#include <Common.h>
#include "../Heap.hh"
#include <Utils/OrderedArray.hh>
#include <Utils/Allocator.hh>

namespace Kernel::Memory
{
	class PageDirectory;
	
	namespace unify_heap_detail
	{
		struct HeapHeader
		{
			uint32_t magic;
			uint8_t is_hole;
			uint32_t size; // <- Includes header and footer
		};
		
		struct HeapFooter
		{
			uint32_t magic;
			struct HeapHeader* header;
		};
		
		struct HeapComp
		{
			bool operator()(const struct HeapHeader* left, const struct HeapHeader* right) const
			{
				return left->size < right->size;
			}
		};
	}
	
	
	template <class Meta_Alloc = Utils::allocator<void>>
	class UnifyHeap : public Heap
	{
		public:
		typedef Meta_Alloc allocator_type;
		typedef unify_heap_detail::HeapHeader head_type;
		typedef unify_heap_detail::HeapFooter foot_type;
		
		protected:
		
		typedef typename allocator_type::template rebind<uint8_t>::other memory_allocator_type;
		
		constexpr static size_t max_adjust = 1024;
		
		memory_allocator_type mem_alloc;
		uint8_t* memory;
		size_t memory_size;
		Utils::OrderedArray<struct unify_heap_detail::HeapHeader*, unify_heap_detail::HeapComp> index;
		PageDirectory* _dir;
		
		virtual void free_page(void*);
		virtual void* alloc_page(size_t continuous_count);
		
		public:
		UnifyHeap(PageDirectory*, bool kernel_mem, bool default_read_only, size_t pg_sz = 0) noexcept;
		
		virtual ~UnifyHeap();
		
		virtual void* alloc(size_t size, size_t alignment = 0) override;
		virtual void free(void*) override;
		virtual size_t allocated_size(void*) const noexcept override;
	};
};

#include "UnifyHeap.hpp"

#endif