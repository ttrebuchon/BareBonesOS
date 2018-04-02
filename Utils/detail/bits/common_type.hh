#ifndef INCLUDED_BITS_COMMON_TYPE_HH
#define INCLUDED_BITS_COMMON_TYPE_HH

#include "decay.hh"
#include <Utils/utility>

namespace Utils
{
namespace detail
{
	template <class...>
	struct __common_type;
	
	
	template <class T, class T2, bool b = can_decay<T>::value || can_decay<T2>::value>
	struct __common_type_2;
	
	template <class T, class T2>
	struct __common_type_2<T, T2, true> : public __common_type<decay_t<T>, decay_t<T2>>
	{};
	
	template <class T, class T2>
	struct __common_type_2<T, T2, false>
	{
		typedef typename  decay<decltype(false ? declval<T>() : declval<T2>())>::type type;
	};
	
	
	
	
	template <>
	struct __common_type<>
	{};
	
	template <class T>
	struct __common_type<T>
	{
		typedef typename __common_type<T, T>::type type;
	};
	
	template <class T, class T2>
	struct __common_type<T, T2> : public __common_type_2<T, T2>
	{
	};
	
	template <class T, class T2, class... R>
	struct __common_type<T, T2, R...>
	{
		typedef typename __common_type<typename __common_type<T, T2>::type, R...>::type type;
	};
	
	
	
	
	
	
	
}
	
	template <class... T>
	struct common_type
	{
		typedef typename detail::__common_type<T...>::type type;
	};
	
	
	template <class... T>
	using common_type_t = typename common_type<T...>::type;
}

#endif