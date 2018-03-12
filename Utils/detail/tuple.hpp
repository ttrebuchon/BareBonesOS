#ifndef INCLUDED_TUPLE_HPP
#define INCLUDED_TUPLE_HPP

#include "tuple.hh"

namespace Utils
{
	template <size_t N, class... Types>
	constexpr typename tuple_element<N, tuple<Types...>>::type& get(tuple<Types...>& tup) noexcept
	{
		return tuple_element<N, tuple<Types...>>::get(tup);
	}
	
	template <size_t N, class... Types>
	constexpr const typename tuple_element<N, tuple<Types...>>::type& get(const tuple<Types...>& tup) noexcept
	{
		return tuple_element<N, tuple<Types...>>::get(tup);
	}
}

#endif