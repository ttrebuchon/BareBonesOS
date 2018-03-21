#ifndef INCLUDED_POINTER_TRAITS_HH
#define INCLUDED_POINTER_TRAITS_HH

#include "void_t.hh"
#include "ptrdiff.hh"

namespace Utils {
	
	template <class>
	class Pointer_Traits;
	
	
	namespace detail { namespace ptr_traits {
		
		template <class, class = void>
		struct elem_type;
		
		template <class P>
		struct elem_type<P, void_t<typename P::element_type>>
		{
			typedef typename P::element_type type;
		};
		
		template <template <class...> class P, class T, class... X>
		struct elem_type<P<T, X...>, T>
		{
			typedef T type;
		};
		
		
		template <class, class, class = void>
		struct rebind;
		
		template <class P, class V>
		struct rebind<P, V, void_t<typename P::template rebind<V>>>
		{
			private:
			template <class X>
			using _rebind = typename P::template rebind<X>;
			
			public:
			
			typedef _rebind<V> type;
		};
		
		template <class V, template <class...> class P, class T, class... X>
		struct rebind<P<T, X...>, V, T>
		{
			typedef P<V, X...> type;
		};
		
		
		template <class, class = void>
		struct ptr_diff;
		
		template <class P>
		struct ptr_diff<P, void_t<typename P::difference_type>>
		{
			typedef typename P::difference_type type;
		};
		
		template <class P>
		struct ptr_diff<P, void>
		{
			typedef ptrdiff_t type;
		};
	}
	}
	
	
	template <class P>
	class Pointer_Traits
	{
		public:
		
		typedef P pointer;
		typedef typename detail::ptr_traits::elem_type<P>::type element_type;
		
		typedef typename detail::ptr_traits::ptr_diff<P>::type difference_type;
		
		
		template <class V>
		using rebind = typename detail::ptr_traits::rebind<P, V>::type;
		
	};
	
	template <class T>
	class Pointer_Traits<T*>
	{
		public:
		
		typedef T* pointer;
		typedef T element_type;
		typedef ptrdiff_t difference_type;
		
		template <class V>
		using rebind = V*;
		
	};
	
}
#endif