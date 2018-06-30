#ifndef INCLUDED_MEMORY_ALLOCATORS_HEAP_HH
#define INCLUDED_MEMORY_ALLOCATORS_HEAP_HH

#include <Common.h>
#include <kernel/Memory/Heap.hh>
#include <Utils/type_traits>
#include <Utils/shared_ptr>


namespace Kernel { namespace Memory
{
	namespace
	{
		template <class T>
		using raw_ptr = T*;
	}
	
	template <class T, template <class...> class pointerT = Utils::shared_ptr>
	class heap_allocator;
	
	
	
	template <template <class...> class pointerT>
	class heap_allocator<void, pointerT>
	{
		public:
		typedef void value_type;
		typedef void* pointer;
		typedef const void* const_pointer;
		
		typedef pointerT<Heap> heap_type;
		
		template <class U>
		struct rebind
		{
			typedef heap_allocator<U, pointerT> other;
		};
		
		protected:
		pointerT<Heap> heap;
		
		public:
		heap_allocator() = delete;
		heap_allocator(heap_type);
		template <class U>
		heap_allocator(const heap_allocator<U, pointerT>&);
		template <class U>
		heap_allocator(heap_allocator<U, pointerT>&&);
		
		heap_allocator(const heap_allocator&);
		heap_allocator(heap_allocator&&);
		
		~heap_allocator() = default;
		
		
		template <class, template <class...> class>
		friend class heap_allocator;
	};
	
	
	
	template <template <class...> class pointerT>
	class heap_allocator<const void, pointerT>
	{
		public:
		typedef const void value_type;
		typedef const void* pointer;
		typedef const void* const_pointer;
		
		typedef pointerT<Heap> heap_type;
		
		template <class U>
		struct rebind
		{
			typedef heap_allocator<U, pointerT> other;
		};
		
		protected:
		pointerT<Heap> heap;
		
		public:
		heap_allocator() = delete;
		heap_allocator(heap_type);
		template <class U>
		heap_allocator(const heap_allocator<U, pointerT>&);
		template <class U>
		heap_allocator(heap_allocator<U, pointerT>&&);
		
		heap_allocator(const heap_allocator&);
		heap_allocator(heap_allocator&&);
		
		~heap_allocator() = default;
		
		
		template <class, template <class...> class>
		friend class heap_allocator;
	};
	
	
	template <class T, template <class...> class pointerT>
	class heap_allocator
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
		
		
		
		typedef pointerT<Heap> heap_type;
		
		
		template <class U>
		struct rebind
		{
			typedef heap_allocator<U, pointerT> other;
		};
		
		protected:
		heap_type heap;
		
		public:
		heap_allocator() = delete;
		heap_allocator(heap_type);
		heap_allocator(const heap_allocator&) noexcept;
		heap_allocator(heap_allocator&&);
		template <class U>
		heap_allocator(const heap_allocator<U, pointerT>&);
		
		~heap_allocator() = default;
		
		
		
		pointer address(reference x) const noexcept
		{
			return &x;
		}
		
		const_pointer address(const_reference x) const noexcept
		{
			return &x;
		}
		
		pointer allocate(size_type n, typename heap_allocator<void, pointerT>::const_pointer hint = 0);
		void deallocate(pointer p, size_type n);
		template <class U, class... Args>
		void construct(U* p, Args... args);
		template <class U>
		void destroy(U* u);
		
		
		template <class U>
		bool compare(const heap_allocator<U, pointerT>&) const;
		
		void swap(heap_allocator& other)
		{
			swap(heap, other.heap);
		}
		
		
		
		template <class, template <class...> class>
		friend class heap_allocator;
	};
	
	
	template <class T, class Y, template <class...> class pointerT>
	bool operator==(const heap_allocator<T, pointerT>& a1, const heap_allocator<Y, pointerT>& a2)
	{
		return a1.compare(a2);
	}
	
	template <class T, class Y, template <class...> class pointerT>
	bool operator!=(const heap_allocator<T, pointerT>& a1, const heap_allocator<Y, pointerT>& a2)
	{
		return !a1.compare(a2);
	}
	
}
}

#include "heap_allocator.hpp"


#endif