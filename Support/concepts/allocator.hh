#ifndef INCLUDED_SUPPORT_CONCEPTS_ALLOCATOR_HH
#define INCLUDED_SUPPORT_CONCEPTS_ALLOCATOR_HH

#include <Common.h>

namespace Support::concepts::allocator
{
	namespace detail::has_size_type
	{
		template <class...>
		using void_t = void;
		
		template <class T, class = void>
		struct check
		{
			constexpr static bool value = false;
		};
		
		template <class T>
		struct check<T, void_t<typename T::size_type>>
		{
			constexpr static bool value = true;
		};
		
		
		template <class T, bool value>
		struct type_or_default;
		
		template <class T>
		struct type_or_default<T, true>
		{
			typedef typename T::size_type type;
		};
		
		
		template <class T>
		struct type_or_default<T, false>
		{
			typedef size_t type;
		};
	}
	
	
	template <class T>
	struct has_size_type
	{
		constexpr static bool value = detail::has_size_type::check<T>::value;
		typedef typename detail::has_size_type::type_or_default<T, value>::type type;
	};
	
	
	
	namespace detail::has_max_size_fn
	{
		template <class T, class size_type>
		struct SFINAE_check
		{
			template <class U, size_type (U::*)() const>
			struct SFINAE {};
			
			typedef long one;
			typedef char two;
			
			template <class U>
			static one check(SFINAE<U, &U::max_size>*);
			
			template <class U>
			static two check(...);
			
			constexpr static bool value = (sizeof(check<T>(0)) == sizeof(one));
		};
	}
	
	template <class T>
	struct has_max_size_fn
	{
		constexpr static bool value = detail::has_max_size_fn::SFINAE_check<T, typename has_size_type<T>::type>::value;
	};
}

#endif