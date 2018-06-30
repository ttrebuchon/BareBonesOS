#ifndef INCLUDED_MEMORY_ALLOCATORS_HEAP_HPP
#define INCLUDED_MEMORY_ALLOCATORS_HEAP_HPP

#include "heap_allocator.hh"

namespace Kernel { namespace Memory
{
	
	template <template <class...> class pointerT>
	heap_allocator<void, pointerT>::heap_allocator(heap_type h) : heap(h)
	{
		assert(h);
	}
	
	template <template <class...> class pointerT>
	heap_allocator<void, pointerT>::heap_allocator(const heap_allocator& other) : heap(other.heap)
	{
		assert(heap);
	}
	
	template <template <class...> class pointerT>
	heap_allocator<void, pointerT>::heap_allocator(heap_allocator&& other) : heap(Utils::forward<heap_type&&>(Utils::move(other.heap)))
	{
		assert(heap);
	}
	
	template <template <class...> class pointerT>
	template <class U>
	heap_allocator<void, pointerT>::heap_allocator(const heap_allocator<U, pointerT>& other) : heap(other.heap)
	{
		assert(heap);
	}
	
	template <template <class...> class pointerT>
	template <class U>
	heap_allocator<void, pointerT>::heap_allocator(heap_allocator<U, pointerT>&& other) : heap(Utils::forward<heap_type&&>(Utils::move(other.heap)))
	{
		assert(heap);
	}
	
	
	
	
	
	
	
	
	template <class T, template <class...> class pointerT>
	heap_allocator<T, pointerT>::heap_allocator(heap_type h) : heap(h)
	{
		assert(h);
	}
	
	
	template <class T, template <class...> class pointerT>
	heap_allocator<T, pointerT>::heap_allocator(const heap_allocator& other) noexcept : heap(other.heap)
	{
		assert(heap);
	}
	
	template <class T, template <class...> class pointerT>
	heap_allocator<T, pointerT>::heap_allocator(heap_allocator&& other) : heap(Utils::forward<heap_type&&>(Utils::move(other.heap)))
	{
		assert(heap);
	}
	
	template <class T, template <class...> class pointerT>
	template <class U>
	heap_allocator<T, pointerT>::heap_allocator(const heap_allocator<U, pointerT>& other) : heap(other.heap)
	{
		assert(heap);
	}
	
	
	
	template <class T, template <class...> class pointerT>
	auto heap_allocator<T, pointerT>::allocate(size_t sz, typename heap_allocator<void, pointerT>::const_pointer hint) -> pointer
	{
		assert(heap);
		#ifdef DEBUG
		auto ptr = (pointer)heap->alloc(sizeof(T)*sz);
		assert((addr_t)ptr % alignof(T) == 0);
		return ptr;
		#else
		return (pointer)heap->alloc(sizeof(T)*sz);
		#endif
	}
	
	template <class T, template <class...> class pointerT>
	void heap_allocator<T, pointerT>::deallocate(pointer ptr, size_t sz)
	{
		assert(heap);
		heap->free(ptr);
	}
	
	template <class T, template <class...> class pointerT>
	template <class U, class... Args>
	void heap_allocator<T, pointerT>::construct(U* ptr, Args... args)
	{
		assert(ptr);
		new (ptr) U(Utils::forward<Args>(args)...);
	}
	
	template <class T, template <class...> class pointerT>
	template <class U>
	void heap_allocator<T, pointerT>::destroy(U* ptr)
	{
		assert(ptr);
		ptr->~U();
	}
}
}

#endif