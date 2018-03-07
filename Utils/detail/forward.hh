#ifndef INCLUDED_FORWARD_HH
#define INCLUDED_FORWARD_HH

#include <Utils/utility>

namespace Utils
{
	template <class T>
	constexpr T&& forward(typename remove_reference<T>::type& t) noexcept
	{
		return static_cast<typename remove_reference<T>::type&&>(t);
	}
	
	template <class T>
	constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept
	{
		return static_cast<typename remove_reference<T>::type&&>(t);
	}
}

#endif