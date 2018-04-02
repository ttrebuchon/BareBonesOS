#ifndef INCLUDED_BITS_STATIC_NUMERIC_HH
#define INCLUDED_BITS_STATIC_NUMERIC_HH

#include "integral_constant.hh"

namespace Utils { namespace detail
{
	template <intmax_t N, bool b = N < 0>
	struct static_abs;
	
	template <intmax_t N>
	struct static_abs<N, false>
	{
		static constexpr intmax_t value = N;
	};
	
	template <intmax_t N>
	struct static_abs<N, true>
	{
		static constexpr intmax_t value = -N;
	};
	
	
	template <intmax_t N1, intmax_t N2>
	struct static_gcd;
	
	
	template <intmax_t N1, intmax_t N2>
	struct static_gcd : public static_gcd<N2, (N1 % N2)>
	{
		
	};
	
	template <intmax_t N1>
	struct static_gcd<N1, 0> : integral_constant<intmax_t, static_abs<N1>::value>
	{};
	
	template <intmax_t N1>
	struct static_gcd<0, N1> : integral_constant<intmax_t, static_abs<N1>::value>
	{};
	
}
}
#endif