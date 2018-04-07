#ifndef INCLUDED_BITS_INTEGRAL_CONSTANT_HH
#define INCLUDED_BITS_INTEGRAL_CONSTANT_HH

namespace Utils
{
	template <class T, T v>
	struct integral_constant
	{
		constexpr static T value = v;
		typedef T value_type;
		typedef integral_constant<T, v> type;
		
		constexpr operator T() const noexcept
		{ return v; }
		
		constexpr T operator()() const noexcept
		{ return v; }
		
	};
	
	typedef integral_constant<bool, true> true_type;
	typedef integral_constant<bool, false> false_type;
}

#endif