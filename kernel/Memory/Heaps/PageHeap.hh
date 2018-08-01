#ifndef INCLUDED_MEMORY_HEAPS_PAGEHEAP_HH
#define INCLUDED_MEMORY_HEAPS_PAGEHEAP_HH

#include <kernel/Memory/Heap.hh>
#include <Utils/Allocator.hh>
#include <Utils/mutex>
#include <Utils/list>
#include <Utils/map>
#include <Utils/vector>

namespace Kernel::Memory
{
	class PageDirectory;
	
	
	template <class Meta_Alloc, class Mutex = Utils::mutex>
	class PageHeap : public Heap
	{
		public:
		typedef Mutex mutex_type;
		typedef Meta_Alloc allocator_type;
		
		
		struct allocation
		{
			void* page;
			size_t length;
			bool used;
		};
		
		protected:
		
		template <class T>
		using alloc_rebind = typename allocator_type::template rebind<T>::other;
		
		
		allocator_type allocator;
		PageDirectory* pdir;
		mutex_type mut;
		Utils::list<allocation, alloc_rebind<allocation>> allocations;
		Utils::map<void*, allocation*, Utils::less<void*>, alloc_rebind<Utils::pair<void* const, allocation*>>> page_map;
		size_t _free_count;
		size_t _use_count;
		
		
		template <class T>
		alloc_rebind<T> get_alloc_T() const noexcept;
		
		public:
		PageHeap(const allocator_type&, PageDirectory*, bool kernel_only, bool default_readonly = false);
		PageHeap(PageDirectory*, bool kernel_only, bool default_readonly = false);
		virtual ~PageHeap();
		
		
		const allocator_type& get_allocator() const noexcept;
		
		virtual void* alloc(size_t size, size_t alignment = 0) override;
		virtual void free(void*) override;
		virtual size_t allocated_size(void*) const noexcept override;
		
	};
}

#include "PageHeap.hpp"

#endif