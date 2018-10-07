#ifndef INCLUDED_ALLOCATOR_HH
#define INCLUDED_ALLOCATOR_HH

#include <Common.h>
#include "detail/forward.hh"
#include "BoolTypes.hh"
#include "ptrdiff.hh"

namespace Utils
{
	template <class T>
	class allocator;
	
	template <>
	class allocator<void>
	{
		public:
		typedef void			   value_type;
		typedef void*			  pointer;
		typedef const void*		const_pointer;
		
		template <class U>
		struct rebind
		{
			typedef allocator<U> other;
		};
	};
	
	
	
	template <>
	class allocator<const void>
	{
		public:
		typedef const void* pointer;
		typedef const void* const_pointer;
		typedef const void value_type;
		
		template <class U>
		struct rebind
		{
			typedef allocator<U> other;
		};
	};
	
	
	
	
	template <class T>
	class allocator
	{
		public:
		typedef T			   value_type;
		typedef T*			  pointer;
		typedef const T*		const_pointer;
		typedef T&			  reference;
		typedef const T&		const_reference;
		typedef decltype(sizeof(0))		  size_type;
		typedef ptrdiff_t	   difference_type;
		typedef true_type	   propagate_on_container_move_assignment;
		//typedef true_type	   is_always_equal;
		

		template <class U>
		struct rebind
		{
			typedef allocator<U> other;
		};
		
		
		__attribute__ ((__visibility__("hidden"), __always_inline__)) allocator() {}
		template <class U>
		__attribute__ ((__visibility__("hidden"), __always_inline__)) allocator(const allocator<U>&) {}
		
		
		
		
		pointer address(reference x) const noexcept
		{
			assert(false);
			return &x;
		}
		
		const_pointer address(const_reference x) const noexcept
		{
			assert(false);
			return &x;
		}
		
		__attribute__ ((__visibility__("hidden"), __always_inline__)) pointer allocate(size_type n, allocator<void>::const_pointer hint = 0)
		{
			return static_cast<pointer>(::operator new(sizeof(T)*n));
			//return (pointer)operator new(sizeof(T)*n, (std::align_val_t)alignof(T));
		}
		
		__attribute__ ((__visibility__("hidden"), __always_inline__)) void deallocate(pointer p, size_type n)
		{
			::operator delete((void*)p);
		}
		
		template <class U, class... Args>
		__attribute__ ((__visibility__("hidden"), __always_inline__)) void construct(U* p, Args&&... args)
		{
			::new ((void*)p) U(Utils::forward<Args>(args)...);
		}
		
		__attribute__ ((__visibility__("hidden"), __always_inline__)) void destroy(pointer p)
		{
			p->~T();
		}
		
		size_type max_size() const noexcept
		{
			return size_type(~0)/sizeof(T);
		}
	};
	
	template <class T, class U>
	bool operator==(const allocator<T>& a1, const allocator<U>& a2)
	{
		return true;
	}
	
	template <class T, class U>
	bool operator!=(const allocator<T>& a1, const allocator<U>& a2)
	{
		return false;
	}
	
	
	
	template <class Alloc, class T>
	struct Alloc_Deleter
	{
		private:
		typedef typename Alloc::template rebind<T>::other Rebound;
		
		Rebound alloc;
		
		public:
		constexpr Alloc_Deleter() noexcept = default;
		
		template <class A, class U>
		constexpr Alloc_Deleter(const Alloc_Deleter<A, U>& d) noexcept : alloc(d.alloc)
		{
			
		}
		
		void operator()(T* ptr) const
		{
			alloc.destroy(ptr);
			alloc.deallocate(ptr, 1);
		}
		
		template <class U>
		void operator()(U* ptr) const = delete;
	};
	
	
	template <class Alloc, class T>
	struct Alloc_Deleter<Alloc, T[]>
	{
		private:
		typedef typename Alloc::template rebind<T[]>::other Rebound;
		
		Rebound alloc;
		
		public:
		constexpr Alloc_Deleter() noexcept = default;
		
		template <class A, class U>
		constexpr Alloc_Deleter(const Alloc_Deleter<A, U[]>& d) noexcept : alloc(d.alloc)
		{
			
		}
		
		template <class U>
		void operator()(U* ptr) const
		{
			typedef T(*h)[];
			h j;
			
			alloc.destroy(ptr);
			auto x = sizeof(ptr)/sizeof(U);
		}
	};
	
	
	
	
	
	
	template <class T>
	class allocator<const T>
	{
		public:
		typedef const T value_type;
		typedef const T* pointer;
		typedef const T*		const_pointer;
		typedef const T& reference;
		typedef const T& const_reference;
		typedef decltype(sizeof(0))		  size_type;
		typedef ptrdiff_t	   difference_type;
		typedef true_type	   propagate_on_container_move_assignment;
		//typedef true_type	   is_always_equal;
		

		template <class U>
		struct rebind
		{
			typedef allocator<U> other;
		};
		
		
		allocator() {}
		template <class U>
		allocator(const allocator<U>&) {}
		
		
		
		
		const_pointer address(const_reference x) const noexcept
		{
			assert(false);
			return &x;
		}
		
		pointer allocate(size_type n, allocator<void>::const_pointer hint = 0)
		{
			return static_cast<pointer>(::operator new(n*sizeof(T)));
		}
		
		void deallocate(pointer p, size_type n)
		{
			::operator delete((void*)p);
		}
		
		template <class U, class... Args>
		void construct(U* p, Args&&... args)
		{
			::new ((void*)p) U(Utils::forward<Args>(args)...);
		}
		
		void destroy(pointer p)
		{
			p->~T();
		}
		
		size_type max_size() const
		{
			return size_type(~0) / sizeof(T);
		}
	};
	
	
	template <class Alloc, class T>
	using alloc_deleter = Alloc_Deleter<Alloc, T>;
	
}
#endif