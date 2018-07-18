#ifndef INCLUDED_BITS_IS_CONSTRUCTIBLE_HH
#define INCLUDED_BITS_IS_CONSTRUCTIBLE_HH

#include <Utils/detail/type_traits_arithmetic.hh>
#include <Utils/void_t.hh>

namespace Utils
{
	namespace detail::is_constr
	{
		
		
		
		template <class T, class... Args>
		struct is_constructible_helper
		{
			typedef char _one;
			typedef int _two;
			
			template <class T1, class... Args1>
			static decltype(T1(declval<Args1>()...), _one()) test(int);
			
			template <class...>
			static _two test(...);
			
			constexpr static bool value = (sizeof(decltype(test<T, Args...>(0))) == sizeof(_one));
		};
		
		template <class T, class Arg>
		struct is_constructible_helper<T, Arg>
		{
			typedef char _one;
			typedef int _two;
			
			template <class T1, class Arg1>
			static decltype(static_cast<T1>(declval<Arg1>()), _one()) test(int);
			
			template <class, class>
			static _two test(...);
			
			constexpr static bool value = (sizeof(decltype(test<T, Arg>(0))) == sizeof(_one));
		};
		
		
		
	
	}
	
	template <class T, class... Args>
	struct is_constructible : public integral_constant<bool, detail::is_constr::is_constructible_helper<T, Args...>::value>
	{
		
	};
	
	
	/*template <class T, class... Args>
	struct is_nothrow_constructible : public integral_constant<bool, detail::is_constr::is_constructible_helper<T, Args...>::value>
	{
		
	};
	
	
	
	template <class T>
	struct is_nothrow_move_constructible : public is_nothrow_constructible<T, typename add_rvalue_reference<T>::type>
	{
		
	};*/
}

#endif