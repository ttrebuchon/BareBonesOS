#ifndef INCLUDED_MEMORY_ALLOCATORS_CHOICE_HH
#define INCLUDED_MEMORY_ALLOCATORS_CHOICE_HH

#include <Common.h>
#include <Utils/type_traits>
#include <Utils/shared_ptr>

#include <Utils/map>
#include <kernel/Memory/alloc_spec.h>
#include <Utils/shared_mutex>


namespace Kernel { namespace Memory
{
	
	namespace detail
	{
		
		class choice_option
		{
			public:
			
			protected:
			
			choice_option(size_t Size, size_t Align) : Size(Size), Alignment(Align)
			{}
			
			public:
			const size_t Size;
			const size_t Alignment;
			
			virtual void* allocate(size_t, const void* hint = nullptr) = 0;
			virtual void deallocate(void*, size_t) = 0;
		};
		
		
		template <size_t Size, size_t Align>
		struct alignas(Align) arbitrary_object
		{
			private:
			uint8_t dat[Size];
		};
		
		template <size_t Size>
		struct alignas(Size) arbitrary_object<Size, 0>
		{
			private:
			uint8_t dat[Size];
		};
		
		
		template <class Alloc, size_t _Size, size_t Align>
		class choice_alloc_option : public choice_option
		{
			protected:
			typedef arbitrary_object<_Size, Align> arb_type;
			
			
			
			public:
			typedef typename Alloc::template rebind<arb_type>::other allocator_type;
			
			
			protected:
			allocator_type alloc;
			
			public:
			
			choice_alloc_option(const Alloc& a) : choice_option(_Size, Align), alloc(a)
			{}
			
			choice_alloc_option() : choice_alloc_option(Alloc())
			{}
			
			
			virtual void* allocate(size_t count, const void* hint = nullptr)
			{
				return alloc.allocate(count, hint);
			}
			
			virtual void deallocate(void* ptr, size_t count)
			{
				alloc.deallocate((typename allocator_type::pointer)ptr, count);
				//assert(NOT_IMPLEMENTED);
			}
		};
		
		
		struct alloc_spec_cmp
		{
			
			int operator()(const alloc_spec_t& lhs, const alloc_spec_t& rhs) const noexcept;
		};
		
		
		template <class Alloc>
		class choice_option_set
		{
			public:
			typedef Alloc allocator_type;
			typedef Utils::shared_timed_mutex mutex_type;
			typedef Utils::shared_lock<mutex_type> s_mutex_lock_type;
			
			protected:
			typedef typename allocator_type::template rebind<Utils::pair<const alloc_spec_t, Utils::shared_ptr<choice_option>>>::other pair_alloc_t;
			
			allocator_type alloc;
			Utils::map<alloc_spec_t, Utils::shared_ptr<choice_option>, alloc_spec_cmp, pair_alloc_t> options;
			mutex_type mut;
			
			
			choice_option_set(const Alloc&);
			
			
			public:
			
			template <class T>
			void add_alloc(const T& a);
			
			Utils::shared_ptr<choice_option> get(const alloc_spec_t&);
			
			static Utils::shared_ptr<choice_option_set> Make(const Alloc& a);
		};
	}
	
	
	template <class T, class A>
	class choice_allocator;
	
	
	
	
	
	
	template <class A>
	class choice_allocator<void, A>
	{
		public:
		typedef void value_type;
		typedef void* pointer;
		typedef const void* const_pointer;
		
		template <class U>
		struct rebind
		{
			typedef choice_allocator<U, A> other;
		};
		
		protected:
		Utils::shared_ptr<detail::choice_option_set<A>> set;
		
		public:
		choice_allocator(const A& = A());
		template <class U>
		choice_allocator(const choice_allocator<U, A>&);
		template <class U>
		choice_allocator(choice_allocator<U, A>&&);
		
		choice_allocator(const choice_allocator&);
		choice_allocator(choice_allocator&&);
		
		~choice_allocator() = default;
		
		
		template <class, class>
		friend class choice_allocator;
	};
	
	
	
	template <class A>
	class choice_allocator<const void, A>
	{
		public:
		typedef const void value_type;
		typedef const void* pointer;
		typedef const void* const_pointer;
		
		template <class U>
		struct rebind
		{
			typedef choice_allocator<U, A> other;
		};
		
		protected:
		Utils::shared_ptr<detail::choice_option_set<A>> set;
		
		public:
		choice_allocator(const A& = A());
		template <class U>
		choice_allocator(const choice_allocator<U, A>&);
		template <class U>
		choice_allocator(choice_allocator<U, A>&&);
		
		choice_allocator(const choice_allocator&);
		choice_allocator(choice_allocator&&);
		
		~choice_allocator() = default;
		
		
		template <class, class>
		friend class choice_allocator;
	};
	
	
	template <class T, class A>
	class choice_allocator
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
		
		
		template <class U>
		struct rebind
		{
			typedef choice_allocator<U, A> other;
		};
		
		protected:
		Utils::shared_ptr<detail::choice_option_set<A>> set;
		typename A::template rebind<T>::other backup;
		
		public:
		choice_allocator(const A& = A());
		choice_allocator(const choice_allocator&) noexcept;
		choice_allocator(choice_allocator&&);
		template <class U>
		choice_allocator(const choice_allocator<U, A>&);
		
		~choice_allocator() = default;
		
		
		
		pointer address(reference x) const noexcept
		{
			return &x;
		}
		
		const_pointer address(const_reference x) const noexcept
		{
			return &x;
		}
		
		pointer allocate(size_type n, typename choice_allocator<void, A>::const_pointer hint = 0);
		void deallocate(pointer p, size_type n);
		template <class U, class... Args>
		void construct(U* p, Args... args);
		void destroy(pointer u);
		
		
		template <class U>
		bool compare(const choice_allocator<U, A>&) const;
		
		void swap(choice_allocator& other)
		{
			swap(set, other.set);
		}
		
		template <class U>
		void add_choice(const U& = U());
		
		
		template <class, class>
		friend class choice_allocator;
	};
	
	
	template <class T, class Y, class A>
	bool operator==(const choice_allocator<T, A>& a1, const choice_allocator<Y, A>& a2)
	{
		return a1.compare(a2);
	}
	
	template <class T, class Y, class A>
	bool operator!=(const choice_allocator<T, A>& a1, const choice_allocator<Y, A>& a2)
	{
		return !a1.compare(a2);
	}
	
	
}
}

#include "choice_allocator.hpp"

#endif