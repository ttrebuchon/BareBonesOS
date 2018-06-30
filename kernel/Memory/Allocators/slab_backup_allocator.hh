#ifndef INCLUDED_MEMORY_ALLOCATORS_SLAB_BACKUP_HH
#define INCLUDED_MEMORY_ALLOCATORS_SLAB_BACKUP_HH

#include <Common.h>
#include <Utils/shared_ptr>
#include <kernel/Memory/Heaps/SlabHeap.hh>
#include <kernel/Memory/alloc_spec.h>

namespace Kernel { namespace Memory
{
	template <class T, class Alloc>
	class slab_backup_allocator;
	
	
	template <class Alloc>
	class slab_backup_allocator<void, Alloc>
	{
		public:
		typedef void value_type;
		typedef void* pointer;
		typedef const void* const_pointer;
		
		typedef Alloc allocator_type;
		
		template <class U>
		struct rebind
		{
			typedef slab_backup_allocator<U, allocator_type> other;
		};
		
		protected:
		alloc_spec_t spec;
		allocator_type alloc;
		Utils::shared_ptr<Heap> heap;
		
		public:
		template <class U>
		slab_backup_allocator(const slab_backup_allocator<U, allocator_type>&);
		template <class U>
		slab_backup_allocator(slab_backup_allocator<U, allocator_type>&&);
		
		slab_backup_allocator(const slab_backup_allocator&);
		slab_backup_allocator(slab_backup_allocator&&);
		
		~slab_backup_allocator() = default;
		
	};
	
	
	
	template <class Alloc>
	class slab_backup_allocator<const void, Alloc>
	{
		public:
		typedef const void value_type;
		typedef const void* pointer;
		typedef const void* const_pointer;
		
		typedef Alloc allocator_type;
		
		template <class U>
		struct rebind
		{
			typedef slab_backup_allocator<U, allocator_type> other;
		};
		
		protected:
		alloc_spec_t spec;
		allocator_type alloc;
		Utils::shared_ptr<Heap> heap;
		
		public:
		template <class U>
		slab_backup_allocator(const slab_backup_allocator<U, allocator_type>&);
		template <class U>
		slab_backup_allocator(slab_backup_allocator<U, allocator_type>&&);
		
		slab_backup_allocator(const slab_backup_allocator&);
		slab_backup_allocator(slab_backup_allocator&&);
		
		~slab_backup_allocator() = default;
		
	};
	
	
	template <class T, class Alloc>
	class slab_backup_allocator
	{
		public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T*		const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef decltype(sizeof(0)) size_type;
		typedef ptrdiff_t difference_type;
		typedef Utils::true_type	   propagate_on_container_move_assignment;
		typedef Utils::true_type	   propagate_on_container_copy_assignment;
		typedef Utils::false_type	   is_always_equal;
		typedef Utils::true_type	   propagate_on_container_swap;
		
		
		
		typedef Alloc allocator_type;
		typedef SlabHeap<T, allocator_type> heap_type;
		
		
		template <class U>
		struct rebind
		{
			typedef slab_backup_allocator<U, allocator_type> other;
		};
		
		protected:
		typedef typename allocator_type::template rebind<uint8_t>::other raw_alloc_type;
		typedef typename allocator_type::template rebind<heap_type>::other heap_alloc_type;
		typedef typename allocator_type::template rebind<T>::other rebound_alloc_type;
		
		alloc_spec_t spec;
		rebound_alloc_type alloc;
		Utils::shared_ptr<Heap> heap;
		
		public:
		slab_backup_allocator(size_t init_pool);
		slab_backup_allocator(size_t init_pool, const allocator_type&);
		slab_backup_allocator(size_t init_pool, bool kernel_mem, bool read_only = false, size_t page_size = PAGE_SIZE, const allocator_type& = allocator_type());
		//slab_backup_allocator();
		slab_backup_allocator(const slab_backup_allocator&) noexcept;
		slab_backup_allocator(slab_backup_allocator&&);
		template <class U>
		slab_backup_allocator(const slab_backup_allocator<U, Alloc>&);
		
		~slab_backup_allocator() = default;
		
		
		
		pointer address(reference x) const noexcept
		{
			return &x;
		}
		
		const_pointer address(const_reference x) const noexcept
		{
			return &x;
		}
		
		pointer allocate(size_type n, typename slab_backup_allocator<void, allocator_type>::const_pointer hint = 0);
		void deallocate(pointer p, size_type n);
		template <class U, class... Args>
		void construct(U* p, Args... args);
		template <class U>
		void destroy(U* u);
		template <class U>
		bool compare(const slab_backup_allocator<U, allocator_type>&) const;
		void swap(slab_backup_allocator& other)
		{
			swap(alloc, other.alloc);
			swap(heap, other.heap);
		}
		
		
		
		template <class, class>
		friend class slab_backup_allocator;
	};
	
	
	template <class T, class Y, class Alloc>
	bool operator==(const slab_backup_allocator<T, Alloc>& a1, const slab_backup_allocator<Y, Alloc>& a2)
	{
		return a1.compare(a2);
	}
	
	template <class T, class Y, class Alloc>
	bool operator!=(const slab_backup_allocator<T, Alloc>& a1, const slab_backup_allocator<Y, Alloc>& a2)
	{
		return !a1.compare(a2);
	}
	
}
}

#include "slab_backup_allocator.hpp"

#endif