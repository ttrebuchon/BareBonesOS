#ifndef INCLUDED_MEMORY_ALLOCATORS_SLAB_BACKUP_HPP
#define INCLUDED_MEMORY_ALLOCATORS_SLAB_BACKUP_HPP

#include "slab_backup_allocator.hh"

namespace Kernel { namespace Memory
{
	
	template <class T, class Alloc>
	slab_backup_allocator<T, Alloc>::slab_backup_allocator(size_t pool_sz, bool kernel_mem, bool read_only, size_t pg_sz, const allocator_type& a) : spec(sizeof(T), alignof(T)), alloc(a), heap(nullptr)
	{
		raw_alloc_type ralloc(alloc);
		heap_alloc_type halloc(ralloc);
		
		auto mem = ralloc.allocate(pool_sz);
		assert(mem);
		
		auto h = halloc.allocate(1);
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
		}, alloc);
		
		assert(heap);
	}
	
	template <class T, class Alloc>
	slab_backup_allocator<T, Alloc>::slab_backup_allocator(size_t pool_sz, const allocator_type& a) : slab_backup_allocator(pool_sz, true, false, PAGE_SIZE, a)
	{
		assert(heap);
	}
	
	template <class T, class Alloc>
	slab_backup_allocator<T, Alloc>::slab_backup_allocator(size_t pool_sz) : slab_backup_allocator(pool_sz, true, false, PAGE_SIZE)
	{
		assert(heap);
	}
	
	
	template <class T, class Alloc>
	slab_backup_allocator<T, Alloc>::slab_backup_allocator(const slab_backup_allocator& other) noexcept : spec(other.spec), alloc(other.alloc), heap(other.heap)
	{
		assert(heap);
	}
	
	template <class T, class Alloc>
	slab_backup_allocator<T, Alloc>::slab_backup_allocator(slab_backup_allocator&& other) : spec(other.spec), alloc(Utils::forward<allocator_type&&>(Utils::move(other.alloc))), heap(Utils::forward<Utils::shared_ptr<Heap>&&>(Utils::move(other.heap)))
	{
		assert(heap);
	}
	
	template <class T, class Alloc>
	template <class U>
	slab_backup_allocator<T, Alloc>::slab_backup_allocator(const slab_backup_allocator<U, Alloc>& other) : spec(other.spec), alloc(other.alloc), heap(other.heap)
	{
		assert(heap);
	}
	
	
	
	template <class T, class Alloc>
	auto slab_backup_allocator<T, Alloc>::allocate(size_t sz, typename slab_backup_allocator<void, allocator_type>::const_pointer hint) -> pointer
	{
		assert(heap);
		bool invalid = (spec.size > 0 && sizeof(T) != spec.size);
		if (!invalid && spec.align > 0)
		{
			invalid = (alignof(T) % spec.align != 0);
		}
		
		if (invalid)
		{
			#ifdef DEBUG
			auto ptr = 
			#else
			return 
			#endif
			alloc.allocate(sz, hint);
			#ifdef DEBUG
			assert(ptr);
			return ptr;
			#endif
		}
		
		#ifdef DEBUG
		auto ptr = 
		#else
		return 
		#endif
		(pointer)heap->alloc(sizeof(T)*sz);
		#ifdef DEBUG
		assert((addr_t)ptr % alignof(T) == 0);
		assert(ptr);
		return ptr;
		#endif
	}
	
	template <class T, class Alloc>
	void slab_backup_allocator<T, Alloc>::deallocate(pointer ptr, size_t sz)
	{
		assert(heap);
		bool invalid = (spec.size > 0 && sizeof(T) != spec.size);
		if (!invalid && spec.align > 0)
		{
			invalid = (alignof(T) % spec.align != 0);
		}
		
		if (invalid)
		{
			alloc.deallocate(ptr, sz);
		}
		else
		{
			heap->free(ptr);
		}
	}
	
	template <class T, class Alloc>
	template <class U, class... Args>
	void slab_backup_allocator<T, Alloc>::construct(U* ptr, Args... args)
	{
		assert(ptr);
		new (ptr) U(Utils::forward<Args>(args)...);
	}
	
	template <class T, class Alloc>
	template <class U>
	void slab_backup_allocator<T, Alloc>::destroy(U* ptr)
	{
		assert(ptr);
		ptr->~U();
	}
}
}

#endif