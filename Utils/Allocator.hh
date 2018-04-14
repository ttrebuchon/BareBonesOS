#ifndef INCLUDED_ALLOCATOR_HH
#define INCLUDED_ALLOCATOR_HH

#include <Common.h>
#include "detail/forward.hh"
#include "BoolTypes.hh"
#include "ptrdiff.hh"

namespace Utils
{
	template <class T>
	class Allocator;
	
	template <>
	class Allocator<void>
	{
		public:
		typedef void			   value_type;
		typedef void*			  pointer;
		typedef const void*		const_pointer;
		
		template <class U>
		struct rebind
		{
			typedef Allocator<U> other;
		};
	};
	
	
	
	template <>
	class Allocator<const void>
	{
		public:
		typedef const void* pointer;
		typedef const void* const_pointer;
		typedef const void value_type;
		
		template <class U>
		struct rebind
		{
			typedef Allocator<U> other;
		};
	};
	
	
	
	
	template <class T>
	class Allocator
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
		typedef true_type	   is_always_equal;
		

		template <class U>
		struct rebind
		{
			typedef Allocator<U> other;
		};
		
		
		constexpr Allocator() noexcept {}
		constexpr Allocator(const Allocator&) noexcept {}
		template <class U>
		constexpr Allocator(const Allocator<U>&) noexcept {}
		
		~Allocator() = default;
		
		
		
		
		pointer address(reference x) const noexcept
		{
			return &x;
		}
		
		const_pointer address(const_reference x) const noexcept
		{
			return &x;
		}
		
		pointer allocate(size_type n, Allocator<void>::const_pointer hint = 0)
		{
			return (pointer)operator new(sizeof(T)*n);
		}
		
		void deallocate(pointer p, size_type n)
		{
			::operator delete((void*)p);
		}
		
		template <class U, class... Args>
		void construct(U* p, Args&&... args)
		{
			::new ((void*)p) U(forward<Args>(args)...);
		}
		
		template <class U>
		void destroy(U* u)
		{
			u->~U();
		}
	};
	
	template <class T>
	bool operator==(const Allocator<T>& a1, const Allocator<T>& a2)
	{
		return true;
	}
	
	template <class T>
	bool operator!=(const Allocator<T>& a1, const Allocator<T>& a2)
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
	class Allocator<const T>
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
		typedef true_type	   is_always_equal;
		

		template <class U>
		struct rebind
		{
			typedef Allocator<U> other;
		};
		
		
		constexpr Allocator() noexcept {}
		constexpr Allocator(const Allocator&) noexcept {}
		template <class U>
		constexpr Allocator(const Allocator<U>&) noexcept {}
		
		~Allocator() = default;
		
		
		
		
		pointer address(reference x) const noexcept
		{
			return &x;
		}
		
		pointer allocate(size_type n, Allocator<void>::const_pointer hint = 0)
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
			::new ((void*)p) U(forward<Args>(args)...);
		}
		
		template <class U>
		void destroy(U* u)
		{
			u->~U();
		}
	};
	
	
	
	
	
	
	template <class T>
	using allocator = Allocator<T>;
	
	
	template <class Alloc, class T>
	using alloc_deleter = Alloc_Deleter<Alloc, T>;
	
}
#endif