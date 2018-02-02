#ifndef INCLUDED_ALLOCATOR_TRAITS_HH
#define INCLUDED_ALLOCATOR_TRAITS_HH

#include "Pointer_Traits.hh"

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
		
	};
	
	
	
	
}
#endif