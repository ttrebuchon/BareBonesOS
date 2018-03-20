#ifndef INCLUDED_SELECTN_HH
#define INCLUDED_SELECTN_HH

#include <Common.h>
#include "tuple.hh"
#include "tuple.hpp"

namespace Utils
{
	namespace detail
	{
	
	template <class P>
	auto& Select1st(P& p);
	template <class P>
	auto& Select2nd(P& p);
	
	template <class T1, class T2>
	T1& Select1st(pair<T1, T2>& p)
	{
		return p.first;
	}
	
	template <class T1, class T2>
	const T1& Select1st(const pair<T1, T2>& p)
	{
		return p.first;
	}
	
	template <class T1, class... T>
	T1& Select1st(tuple<T1, T...>& p)
	{
		return get<0>(p);
	}
	
	template <class T1, class... T>
	const T1& Select1st(const tuple<T1, T...>& p)
	{
		return get<0>(p);
	}
	
	
	
	
	
	template <class T1, class T2>
	T2& Select2nd(pair<T1, T2>& p)
	{
		return p.second;
	}
	
	template <class T1, class T2>
	const T2& Select2nd(const pair<T1, T2>& p)
	{
		return p.second;
	}
	
	template <class T1, class T2, class... T>
	T2& Select2nd(tuple<T1, T2, T...>& p)
	{
		return get<1>(p);
	}
	
	template <class T1, class T2, class... T>
	const T2& Select2nd(const tuple<T1, T2, T...>& p)
	{
		return get<1>(p);
	}
	
	}
	
	
	struct Select1st
	{
		template <class T>
		constexpr auto& operator()(T& t) const noexcept
		{
			return detail::Select1st(forward<T&>(t));
		}
		
		template <class T>
		constexpr auto& operator()(const T& t) const noexcept
		{
			return detail::Select1st(forward<const T&>(t));
		}
	};
	
	struct Select2nd
	{
		template <class T>
		constexpr auto& operator()(T& t) const noexcept
		{
			return detail::Select2nd(forward<T&>(t));
		}
	};
}

#endif