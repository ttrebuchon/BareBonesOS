#ifndef INCLUDED_MEMORY_ALLOCATORS_SLAB_HPP
#define INCLUDED_MEMORY_ALLOCATORS_SLAB_HPP

#include "slab_allocator.hh"

namespace Kernel { namespace Memory
{
	
	template <class T, class Alloc>
	slab_allocator<T, Alloc>::slab_allocator(size_t pool_sz, bool kernel_mem, bool read_only, size_t pg_sz, const allocator_type& a) : alloc(a), heap(nullptr)
	{
		raw_alloc_type ralloc(alloc);
		heap_alloc_type halloc(ralloc);
		
		auto mem = ralloc.allocate(pool_sz);
		assert(mem);
		
		auto h = halloc.allocate(1);
		assert(h);
		halloc.construct(h, alloc, mem, pool_sz, kernel_mem, read_only, pg_sz);
		heap = Utils::shared_ptr<heap_type>(h, [=](auto _obj) -> void
		{
			#ifdef __cpp_rtti
			auto obj = dynamic_cast<heap_type*>(_obj);
			assert(obj);
			#else
			auto obj = static_cast<heap_type*>(_obj);
			assert(obj);
			#endif
			auto halloc2 = halloc;
			halloc2.destroy(obj);
			halloc2.deallocate(obj, 1);
			auto ralloc2 = ralloc;
			ralloc2.deallocate(mem, pool_sz);
		});
		assert(heap);
	}
	
	template <class T, class Alloc>
	slab_allocator<T, Alloc>::slab_allocator(size_t pool_sz, const allocator_type& a) : slab_allocator(pool_sz, true, false, PAGE_SIZE, a)
	{
		assert(heap);
	}
	
	template <class T, class Alloc>
	slab_allocator<T, Alloc>::slab_allocator(size_t pool_sz) : slab_allocator(pool_sz, true, false, PAGE_SIZE)
	{
		assert(heap);
	}
	
	
	template <class T, class Alloc>
	slab_allocator<T, Alloc>::slab_allocator(const slab_allocator& other) noexcept : alloc(other.alloc), heap(other.heap)
	{
		assert(heap);
	}
	
	template <class T, class Alloc>
	slab_allocator<T, Alloc>::slab_allocator(slab_allocator&& other) : alloc(Utils::forward<allocator_type&&>(Utils::move(other.alloc))), heap(Utils::forward<Utils::shared_ptr<Heap>&&>(Utils::move(other.heap)))
	{
		assert(heap);
	}
	
	template <class T, class Alloc>
	template <class U>
	slab_allocator<T, Alloc>::slab_allocator(const slab_allocator<U, Alloc>& other) : alloc(other.alloc), heap(other.heap)
	{
		assert(heap);
	}
	
	
	
	template <class T, class Alloc>
	auto slab_allocator<T, Alloc>::allocate(size_t sz, typename slab_allocator<void, allocator_type>::const_pointer hint) -> pointer
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
	
	template <class T, class Alloc>
	void slab_allocator<T, Alloc>::deallocate(pointer ptr, size_t sz)
	{
		assert(heap);
		heap->free(ptr);
	}
	
	template <class T, class Alloc>
	template <class U, class... Args>
	void slab_allocator<T, Alloc>::construct(U* ptr, Args... args)
	{
		assert(ptr);
		new (ptr) U(Utils::forward<Args>(args)...);
	}
	
	template <class T, class Alloc>
	template <class U>
	void slab_allocator<T, Alloc>::destroy(U* ptr)
	{
		assert(ptr);
		ptr->~U();
	}
}
}

#endif