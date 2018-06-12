#ifndef INCLUDED_MEMORY_ALLOCATORS_CROSS_PROCESSOR_HH
#define INCLUDED_MEMORY_ALLOCATORS_CROSS_PROCESSOR_HH

#include <Common.h>

namespace Kernel { namespace Memory
{
	
	void* allocate_cross_processor(size_t, size_t alignment);
	void free_cross_processor(void*);
	
	template <class T>
	class cross_proc_allocator;
	
	template <>
	class cross_proc_allocator<void>
	{
		public:
		typedef void			   value_type;
		typedef void*			  pointer;
		typedef const void*		const_pointer;
		
		template <class U>
		struct rebind
		{
			typedef cross_proc_allocator<U> other;
		};
	};
	
	
	
	template <>
	class cross_proc_allocator<const void>
	{
		public:
		typedef const void* pointer;
		typedef const void* const_pointer;
		typedef const void value_type;
		
		template <class U>
		struct rebind
		{
			typedef cross_proc_allocator<U> other;
		};
	};
	
	
	
	
	template <class T>
	class cross_proc_allocator
	{
		public:
		typedef T			   value_type;
		typedef T*			  pointer;
		typedef const T*		const_pointer;
		typedef T&			  reference;
		typedef const T&		const_reference;
		typedef decltype(sizeof(0))		  size_type;
		typedef ptrdiff_t	   difference_type;
		

		template <class U>
		struct rebind
		{
			typedef cross_proc_allocator<U> other;
		};
		
		
		constexpr cross_proc_allocator() noexcept {}
		constexpr cross_proc_allocator(const cross_proc_allocator&) noexcept {}
		template <class U>
		constexpr cross_proc_allocator(const cross_proc_allocator<U>&) noexcept {}
		
		~cross_proc_allocator() = default;
		
		
		
		
		pointer address(reference x) const noexcept
		{
			return &x;
		}
		
		const_pointer address(const_reference x) const noexcept
		{
			return &x;
		}
		
		pointer allocate(size_type n, cross_proc_allocator<void>::const_pointer hint = 0)
		{
			return (pointer)allocate_cross_processor(sizeof(T)*n, alignof(T));
		}
		
		void deallocate(pointer p, size_type n)
		{
			free_cross_processor(p);
		}
		
		template <class U, class... Args>
		void construct(U* p, Args&&... args)
		{
			::new ((void*)p) U(Utils::forward<Args>(args)...);
		}
		
		template <class U>
		void destroy(U* u)
		{
			u->~U();
		}
	};
	
	template <class T>
	bool operator==(const cross_proc_allocator<T>& a1, const cross_proc_allocator<T>& a2)
	{
		return true;
	}
	
	template <class T>
	bool operator!=(const cross_proc_allocator<T>& a1, const cross_proc_allocator<T>& a2)
	{
		return false;
	}
	
}
}


#endif