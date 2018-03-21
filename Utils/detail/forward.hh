#pragma once
#ifndef INCLUDED_FORWARD_HH
#define INCLUDED_FORWARD_HH

#include "enable_if.hh"
#include "bits/is_lvalue_reference.hh"
#include "bits/is_rvalue_reference.hh"
#include <Utils/utility>

namespace Utils
{
	/*template <class T>
	constexpr T&& forward(typename remove_reference<T>::type& t) noexcept
	{
		return static_cast<typename remove_reference<T>::type&&>(t);
	}
	
	template <class T>
	constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept
	{
		return static_cast<typename remove_reference<T>::type&&>(t);
	}*/
	
	
	template <class T>
	struct identity
	{
		typedef T type;
	};
	
	
	template <class T>
	constexpr typename enable_if<!is_lvalue_reference<T>::value, T&&>::type forward(typename identity<T>::type& t)
	{
		return static_cast<T&&>(t);
	}
	
	template <class T>
	constexpr typename enable_if<!is_lvalue_reference<T>::value, T&&>::type forward(typename identity<T>::type&& t)
	{
		return static_cast<T&&>(t);
	}
	
	template <class T>
	constexpr typename enable_if<is_lvalue_reference<T>::value, T>::type forward(typename identity<T>::type t)
	{
		return t;
	}
	
	template <class T>
	constexpr typename enable_if<is_lvalue_reference<T>::value, T>::type forward(typename remove_reference<T>::type&& t) = delete;
}

#endif