#ifndef INCLUDED_MEMORY_PAGEREGION_HH
#define INCLUDED_MEMORY_PAGEREGION_HH

#include <Common.h>
#include <Utils/utility>

namespace Kernel::Memory
{
	
	#define CODE_MEM_REGION 0
	
	class PageRegion
	{
		public:
		
		protected:
		
		virtual uint8_t* allocate(size_t size, size_t alignment = 0) = 0;
		virtual void deallocate(void*, size_t size = 0) = 0;
		
		public:
		
		
		
		
		
		template <class T>
		friend class PageRegion_allocator;
	};
	
	
	template <class T>
	class PageRegion_allocator;
	
	template <>
	class PageRegion_allocator<void>
	{
		public:
		typedef void			   value_type;
		typedef void*			  pointer;
		typedef const void*		const_pointer;
		
		template <class U>
		struct rebind
		{
			typedef PageRegion_allocator<U> other;
		};
		
		protected:
		PageRegion* _region;
		
		public:
		PageRegion_allocator(PageRegion&) noexcept;
		
		template <class Y>
		PageRegion_allocator(const PageRegion_allocator<Y>&);
	};
	
	template <>
	class PageRegion_allocator<const void>
	{
		public:
		typedef const void* pointer;
		typedef const void* const_pointer;
		typedef const void value_type;
		
		template <class U>
		struct rebind
		{
			typedef PageRegion_allocator<U> other;
		};
		
		protected:
		PageRegion* _region;
		
		public:
		PageRegion_allocator(PageRegion&) noexcept;
		
		template <class Y>
		PageRegion_allocator(const PageRegion_allocator<Y>&);
	};
	
	template <class T>
	class PageRegion_allocator
	{
		public:
		typedef T			   value_type;
		typedef T*			  pointer;
		typedef const T*		const_pointer;
		typedef T&			  reference;
		typedef const T&		const_reference;
		typedef decltype(sizeof(0))		  size_type;
		typedef ptrdiff_t	   difference_type;
		typedef Utils::true_type	   propagate_on_container_move_assignment;
		typedef Utils::false_type	   is_always_equal;
		

		template <class U>
		struct rebind
		{
			typedef PageRegion_allocator<U> other;
		};
		
		protected:
		PageRegion* _region;
		
		public:
		
		
		constexpr PageRegion_allocator(PageRegion& pr) noexcept : _region(&pr)
		{}
		
		constexpr PageRegion_allocator(const PageRegion_allocator& o) noexcept : _region(o._region)
		{}
		
		template <class U>
		constexpr PageRegion_allocator(const PageRegion_allocator<U>& o) noexcept : _region(o._region)
		{}
		
		~PageRegion_allocator() = default;
		
		PageRegion* region() const
		{
			assert(_region);
			return _region;
		}
		
		
		
		pointer address(reference x) const noexcept
		{
			return &x;
		}
		
		const_pointer address(const_reference x) const noexcept
		{
			return &x;
		}
		
		pointer allocate(size_type n, PageRegion_allocator<void>::const_pointer hint = 0)
		{
			assert(region());
			return (pointer)region()->allocate(n*sizeof(T), alignof(T));
		}
		
		void deallocate(pointer p, size_type n)
		{
			region()->deallocate(p, sizeof(T)*n);
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
		
		template <class U>
		friend class PageRegion_allocator;
		
		template <class Y, class U>
		friend bool operator==(const PageRegion_allocator<Y>&, const PageRegion_allocator<U>&);
		
		template <class Y, class U>
		friend bool operator!=(const PageRegion_allocator<Y>&, const PageRegion_allocator<U>&);
	};
	
	
	template <class T, class Y>
	bool operator==(const PageRegion_allocator<T>& a1, const PageRegion_allocator<Y>& a2)
	{
		return (a1._region == a2._region);
	}
	
	template <class T, class Y>
	bool operator!=(const PageRegion_allocator<T>& a1, const PageRegion_allocator<Y>& a2)
	{
		return (a1._region != a2._region);
	}
	
}

#endif