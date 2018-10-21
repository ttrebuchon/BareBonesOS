#ifndef INCLUDED_MEMORY_HEAPS_KERNELHEAP_HH
#define INCLUDED_MEMORY_HEAPS_KERNELHEAP_HH

#include <kernel/Memory/Heap.hh>
#include <Utils/Allocator.hh>
#include <Utils/mutex>
#include <Utils/shared_mutex>
#include "BuddyHeap.hh"
#include <Utils/vector>
#include <Support/Collections/simple_list.h>
#include <Utils/map>
#include <Utils/unordered_set>
#include <kernel/Memory/alloc_spec.h>
#include "PageHeap.hh"
#include <kernel/Memory/Allocators/heap_allocator.hh>

namespace Kernel::Memory
{
	class PageDirectory;
	
	
	template <class Alloc>
	class kernel_heap : public Heap
	{
		public:
		typedef Alloc allocator_type;
		typedef Utils::mutex mutex_type;
		typedef Utils::shared_timed_mutex shared_mutex_type;
		
		protected:
		
		constexpr static size_t smallest_size = 4*sizeof(void*);
		
		template <class T>
		using alloc_rebind = typename allocator_type::template rebind<T>::other;
		
		template <class T>
		using simple_list = Support::Collections::list_node_t<T>;
		
		template <class T>
		using list_alloc_type = alloc_rebind<simple_list<T>>;
		
		
		mutable shared_mutex_type page_directories_m;
		Utils::vector<PageDirectory*, alloc_rebind<PageDirectory*>> page_directories;
		
		
		BuddyHeap<allocator_type, mutex_type> top;
		PageHeap<allocator_type, mutex_type> paged;
		
		
		typedef typename Utils::map<size_t, simple_list<Heap*>*>::value_type heaps_value_type;
		mutable shared_mutex_type heaps_m;
		Utils::map<size_t, simple_list<Heap*>*, Utils::less<size_t>, alloc_rebind<heaps_value_type>> heaps;
		Utils::unordered_set<Heap*, Utils::hash<Heap*>, Utils::equal_to<Heap* const>, alloc_rebind<Heap*>> heaps_set;
		
		allocator_type allocator;
		
		
		
		// Counters
		size_t allocations;
		size_t frees;
		size_t total_heap_reqs;
		Utils::map<size_t, size_t, Utils::less<size_t>, alloc_rebind<typename Utils::map<size_t, size_t>::value_type>> heap_requests;
		mutable shared_mutex_type heap_requests_m;
		
		
		
		
		void initialize_default_heaps();
		const Heap* heap_for(const alloc_spec_t&, Utils::shared_lock<shared_mutex_type>& lock, simple_list<Heap*>** = nullptr) const;
		const Heap* heap_for(void*, Utils::shared_lock<shared_mutex_type>& lock) const;
		Heap* heap_for(const alloc_spec_t&, Utils::shared_lock<shared_mutex_type>& lock, simple_list<Heap*>** = nullptr);
		Heap* heap_for(void*, Utils::shared_lock<shared_mutex_type>& lock);
		
		template <class HeapType>
		HeapType* create_heap_for(const alloc_spec_t&, const size_t min_count, Utils::shared_lock<shared_mutex_type>& lock);
		Heap* create_heap_for(const alloc_spec_t&, const size_t min_count, Utils::shared_lock<shared_mutex_type>& lock);
		Heap* create_heap_for(const alloc_spec_t&, const size_t min_count, Utils::unique_lock<shared_mutex_type>& lock);
		
		Heap* heap_is_full(Heap*, Utils::shared_lock<shared_mutex_type>& lock, simple_list<Heap*>** list = nullptr);
		
		public:
		kernel_heap(uintptr_t start, uintptr_t end, const allocator_type& = allocator_type());
		kernel_heap(void* start, void* end, const allocator_type& = allocator_type());
		kernel_heap(PageDirectory*, uintptr_t start, uintptr_t end, const allocator_type& = allocator_type());
		kernel_heap(PageDirectory*, void* start, void* end, const allocator_type& = allocator_type());
		
		virtual ~kernel_heap();
		
		virtual void* alloc(size_t size, size_t alignment = 0) override;
		virtual void free(void*) override;
		// virtual void* realloc(void*, size_t, size_t alignment = 0) override;
		virtual size_t allocated_size(void*) const noexcept override;
		
		void link_page_directory(PageDirectory*);
	};
	
}

#include "KernelHeap.hpp"

#endif