#ifndef INCLUDED_MEMORY_ALLOCATORS_VIRTUAL_HH
#define INCLUDED_MEMORY_ALLOCATORS_VIRTUAL_HH

#include <Common.h>
#include <Utils/type_traits>
#include <Utils/shared_ptr>

#include <Utils/map>
#include <kernel/Memory/alloc_spec.h>
#include <Utils/shared_mutex>
#include <Support/concepts/allocator.hh>
#include <Support/sharable.hh>


namespace Kernel::Memory
{
	namespace detail::virtual_allocator
	{
		
		class inner_alloc
		{
			protected:
			
			public:
			
			
			virtual void* allocate(const size_t size, const size_t align, const size_t count, const void* cvptr = nullptr) = 0;
			virtual void deallocate(void*, const size_t) noexcept = 0;
			virtual bool compare(const void*) const noexcept = 0;
			virtual inner_alloc* copy() const = 0;
			
			virtual size_t max_size()
			{
				return (size_t)(-1);
			}
			
			
			const void* vtable() const noexcept
			{
				static_assert(sizeof(inner_alloc) == sizeof(void*));
				
				auto vthis = (const void* const*)this;
				const void* tbl = *vthis;
				
				return tbl;
			}
			
		};
		
		template <class T, bool b>
		struct max_size_choose;
		
		template <class T>
		struct max_size_choose<T, false>
		{
			constexpr static size_t func(const T&) noexcept
			{
				return size_t(-1);
			}
		};
		
		template <class T>
		struct max_size_choose<T, true>
		{
			static size_t func(const T& a) noexcept
			{
				return a.max_size();
			}
		};
		
		template <class Delegated>
		class inner_alloc_delegate : public inner_alloc
		{
			public:
			typedef typename Delegated::template rebind<uint8_t>::other delegate_type;
			typedef typename delegate_type::pointer delegate_pointer;
			
			protected:
			delegate_type del;
			
			
			public:
			inner_alloc_delegate(const Delegated& d) : del(d)
			{
				
			}
			
			
			virtual void* allocate(const size_t size, const size_t align, const size_t count, const void* cvptr = nullptr) override
			{
				return del.allocate(size*count, cvptr);
			}
			
			virtual void deallocate(void* ptr, const size_t size) noexcept override
			{
				del.deallocate((delegate_pointer)ptr, size);
			}
			
			virtual bool compare(const void* _other) const noexcept override
			{
				auto other = (const inner_alloc_delegate<Delegated>*)_other;
				return del == other->del;
			}
			
			virtual inner_alloc* copy() const override
			{
				return new inner_alloc_delegate(del);
			}
			
			virtual size_t max_size()
			{
				typedef Support::concepts::allocator::has_max_size_fn<delegate_type> concept_val;
				return max_size_choose<delegate_type, concept_val::value>::func(del);
				//return del.max_size();
			}
		};
	}
	
	
	template <class T>
	class virtual_allocator
	{
		public:
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef void* void_pointer;
		typedef const void* const_void_pointer;
		typedef T value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		
		
		
		
		template <class Y>
		struct rebind
		{
			typedef virtual_allocator<Y> other;
		};
		
		protected:
		typedef detail::virtual_allocator::inner_alloc inner_type;
		typedef Utils::shared_ptr<inner_type> inner_ptr;
		inner_ptr inner;
		
		virtual_allocator(inner_ptr inner) : inner(inner)
		{
			
		}
		
		public:
		
		virtual_allocator(const virtual_allocator& other) : inner(other.inner)
		{
			
		}
		
		template <class Y>
		virtual_allocator(const virtual_allocator<Y>& other) : inner(other.inner)
		{
			
		}
		virtual_allocator(virtual_allocator&& other) : inner(Utils::move(other.inner))
		{
			
		}
		
		template <class Y>
		virtual_allocator(virtual_allocator<Y>&& other) : inner(Utils::move(other.inner))
		{
			
		}
		
		virtual_allocator& operator=(const virtual_allocator& a)
		{
			inner = a.inner;
			return *this;
		}
		
		template <class Y>
		virtual_allocator& operator=(const virtual_allocator<Y>& a)
		{
			inner = a.inner;
			return *this;
		}
		
		
		virtual_allocator& operator=(virtual_allocator&& a)
		{
			inner = Utils::move(a.inner);
			return *this;
		}
		
		template <class Y>
		virtual_allocator& operator=(virtual_allocator<Y>&& a)
		{
			inner = Utils::move(a.inner);
			return *this;
		}
		
		
		
		
		pointer allocate(const size_type n)
		{
			return (pointer)inner->allocate(sizeof(T), alignof(T), n);
		}
		
		pointer allocate(const size_type n, const_void_pointer cvptr)
		{
			return (pointer)inner->allocate(sizeof(T), alignof(T), n, cvptr);
		}
		
		void deallocate(pointer ptr, const size_type n) noexcept
		{
			inner->deallocate(ptr, n);
		}
		
		size_type max_size()
		{
			return inner->max_size();
		}
		
		template <class... Args>
		void construct(pointer ptr, Args&&... args)
		{
			new (ptr) T(Utils::forward<Args>(args)...);
		}
		
		void destroy(pointer ptr)
		{
			assert(ptr);
			ptr->~T();
		}
		
		
		
		bool operator==(const virtual_allocator& rhs) const noexcept
		{
			if (inner == rhs.inner)
			{
				return true;
			}
			else if (inner->vtable() == rhs.inner->vtable())
			{
				return inner->compare(rhs.inner.get());
			}
			else
			{
				return false;
			}
		}
		
		bool operator!=(const virtual_allocator& rhs) const noexcept
		{ return !(*this == rhs); }
		
		
		
		template <class A>
		static virtual_allocator make(const A& a)
		{
			typedef typename A::template rebind<uint8_t>::other delegate_type;
			auto inner = new detail::virtual_allocator::inner_alloc_delegate<delegate_type>(delegate_type(a));
			return virtual_allocator(Utils::shared_ptr<inner_type>(inner));
		}
		
		template <class>
		friend class virtual_allocator;
	};
}

#include "virtual_allocator.hpp"

#endif