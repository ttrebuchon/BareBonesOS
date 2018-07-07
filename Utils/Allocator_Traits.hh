#ifndef INCLUDED_ALLOCATOR_TRAITS_HH
#define INCLUDED_ALLOCATOR_TRAITS_HH

#include "Pointer_Traits.hh"
#include "Utility"

namespace Utils
{
	namespace detail { namespace alloc_traits {
		template <class A, class T = void>
		struct pointer;
		
		template <class A>
		struct pointer<A, typename A::pointer>
		{
			typedef typename A::pointer type;
		};
		
		template <class A>
		struct pointer<A, void>
		{
			typedef typename A::value_type* type;
		};
		
		
		
		template <class A, class T = void>
		struct const_pointer;
		
		template <class A>
		struct const_pointer<A, typename A::const_pointer>
		{
			typedef typename A::const_pointer type;
		};
		
		template <class A>
		struct const_pointer<A, void>
		{
			typedef const typename A::value_type* type;
		};
		
		
		
		
		template <class A, class T = void>
		struct reference;
		
		template <class A>
		struct reference<A, typename A::reference>
		{
			typedef typename A::reference type;
		};
		
		template <class A>
		struct reference<A, void>
		{
			typedef typename A::value_type& type;
		};
		
		
		
		template <class A, class T = void>
		struct const_reference;
		
		template <class A>
		struct const_reference<A, typename A::const_reference>
		{
			typedef typename A::const_reference type;
		};
		
		template <class A>
		struct const_reference<A, void>
		{
			typedef const typename A::value_type& type;
		};
		
		
		
		template <class A, class T = void>
		struct void_pointer;
		
		template <class A>
		struct void_pointer<A, typename A::void_pointer>
		{
			typedef typename A::void_pointer type;
		};
		
		template <class A>
		struct void_pointer<A, void>
		{
			private:
			typedef typename pointer<A>::type pointer;
			
			typedef Pointer_Traits<pointer> traits;
			
			template <class V>
			using _rebind = typename traits::template rebind<V>;
			
			public:
			
			typedef _rebind<void> type;
		};
		
		
		
		template <class A, class T = void>
		struct const_void_pointer;
		
		template <class A>
		struct const_void_pointer<A, typename A::const_void_pointer>
		{
			typedef typename A::const_void_pointer type;
		};
		
		template <class A>
		struct const_void_pointer<A, void>
		{
			private:
			typedef typename alloc_traits::pointer<A>::type pointer;
			
			typedef Pointer_Traits<pointer> traits;
			
			template <class X>
			using _rebind = typename traits::template rebind<X>;
			
			public:
			
			typedef _rebind<const void> type;
		};
		
		
		
		template <class, class = void>
		struct difference_type;
		
		template <class A>
		struct difference_type<A, typename A::difference_type>
		{
			typedef typename A::difference_type type;
		};
		
		template <class A>
		struct difference_type<A, void>
		{
			typedef typename Pointer_Traits<typename pointer<A>::type>::difference_type type;
		};
		
		
		
		
		template <class Alloc, class = void>
		struct copy_construct_ret
		{
			constexpr static int Instance = 0;
			typedef Alloc type;
		};
		
		template <class Alloc>
		struct copy_construct_ret<Alloc, void_t<decltype(Alloc(declval<Alloc>()))>>
		{
			constexpr static int Instance = 1;
			typedef Alloc type;
		};
		
		
		template <class Alloc1, class Alloc2, class = void>
		struct copy_construct
		{
			private:
			typedef copy_construct_ret<Alloc1> return_struct;
			
			public:
			constexpr static int Instance = (return_struct::Instance << 1) | 0;
			typedef typename return_struct::type return_type;
			
			constexpr inline static typename return_struct::type call(const Alloc2&)
			{
				return Alloc1();
			}
			
			constexpr inline static Alloc1* in_place(void* ptr, const Alloc2&)
			{
				new (ptr) Alloc1();
			}
			
			template <class MetaAlloc>
			constexpr inline static Alloc1* in_place(MetaAlloc& ma, void* ptr, const Alloc2& a2)
			{
				ma.construct((Alloc1*)ptr);
				return (Alloc1*)ptr;
			}
		};
		
		template <class Alloc1, class Alloc2>
		struct copy_construct<Alloc1, Alloc2, void_t<decltype(Alloc1(declval<Alloc2>()))>>
		{
			private:
			typedef copy_construct_ret<Alloc1> return_struct;
			
			public:
			constexpr static int Instance = (return_struct::Instance << 1) | 1;
			typedef typename return_struct::type return_type;
			
			constexpr inline static typename return_struct::type call(const Alloc2& a2)
			{
				return Alloc1(a2);
			}
			
			constexpr inline static Alloc1* in_place(void* ptr, const Alloc2& a2)
			{
				new (ptr) Alloc1(a2);
			}
			
			template <class MetaAlloc>
			constexpr inline static Alloc1* in_place(MetaAlloc& ma, void* ptr, const Alloc2& a2)
			{
				ma.construct((Alloc1*)ptr, a2);
				return (Alloc1*)ptr;
			}
		};
		
	}
	}
	
	
	template <class A>
	struct Allocator_Traits
	{
		typedef A allocator_type;
		typedef typename A::value_type value_type;
		typedef typename detail::alloc_traits::pointer<A>::type pointer;
		typedef typename detail::alloc_traits::const_pointer<A>::type const_pointer;
		typedef typename detail::alloc_traits::reference<A>::type reference;
		
		typedef typename detail::alloc_traits::const_reference<A>::type const_reference;
		
		typedef typename detail::alloc_traits::void_pointer<A>::type void_pointer;
		typedef typename detail::alloc_traits::const_void_pointer<A>::type const_void_pointer;
		
		typedef typename detail::alloc_traits::difference_type<A>::type difference_type;
		
		template <class T, class... Args>
		static void construct(allocator_type& alloc, T* p, Args&&... args)
		{
			alloc.construct(p, Utils::forward<Args>(args)...);
		}
		
		static pointer allocate(allocator_type& alloc, size_t n)
		{
			return alloc.allocate(n);
		}
		
		template <class T>
		static void destroy(allocator_type& alloc, T* p)
		{
			alloc.destroy(p);
		}
		
		static void deallocate(allocator_type& alloc, pointer p, size_t n)
		{
			alloc.deallocate(p, n);
		}
		
		
		
		
		template <class A2>
		static typename detail::alloc_traits::copy_construct<A, A2>::return_type copy_create(const A2& a2)
		{
			return detail::alloc_traits::copy_construct<A, A2>::call(a2);
		}
		
		template <class A2>
		static A* copy_create(void* ptr, const A2& a2)
		{
			return detail::alloc_traits::copy_construct<A, A2>::in_place(ptr, a2);
		}
		
		template <class MA, class A2>
		static A* copy_create(MA& ma, void* ptr, const A2& a2)
		{
			return detail::alloc_traits::copy_construct<A, A2>::in_place(ma, ptr, a2);
		}
	};
	
	template <class A>
	using allocator_traits = Allocator_Traits<A>;
	
	
}
#endif