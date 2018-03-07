#ifndef INCLUDED_NUMERIC_LIMITS_HH
#define INCLUDED_NUMERIC_LIMITS_HH

namespace Utils {
	
namespace detail
{
	template <class T, size_t N>
	struct Num_LimitFinder
	{
		constexpr static T value = 256*(Num_LimitFinder<T, N-1>::value) + 256 - 1;
	};
	
	template <class T>
	struct Num_LimitFinder<T, 1>
	{
		constexpr static T value = 255;
	};
}

enum float_round_style
{
	round_indeterminate = -1,
	round_toward_zero = 0,
	round_to_nearest = 1,
	round_toward_infinity = 2,
	round_toward_negative_infinity = 3,
};

enum float_denorm_style
{
	denorm_indeterminate = -1,
	denorm_absent = 0,
	denorm_present = 1,
};

template <class T>
class numeric_limits;

template <class T>
class numeric_limits<const T> : public numeric_limits<T>
{};


template <class T>
class numeric_limits
{
	public:
	constexpr static bool is_specialized = false;
	typedef T type;
	
	constexpr static type min() noexcept
	{ return type(); }
	
	constexpr static type max() noexcept
	{ return type(); }
	
	constexpr static type lowest() noexcept
	{ return type(); }
	
	constexpr static int digits = 0;
	constexpr static int digits10 = 0;
	
	constexpr static bool is_signed = false;
	constexpr static bool is_integer = false;
	constexpr static bool is_exact = false;
	constexpr static int radix = 0;
	
	constexpr static type epsilon() noexcept
	{ return type(); }
	
	constexpr static type round_error() noexcept
	{ return type(); }
	
	constexpr static int min_exponent = 0;
	constexpr static int min_exponent10 = 0;
	constexpr static int max_exponent = 0;
	constexpr static int max_exponent10 = 0;
	
	constexpr static bool has_infinity = false;
	constexpr static bool has_quiet_NaN = false;
	constexpr static bool has_signaling_NaN = false;
	
	constexpr static float_denorm_style has_denorm = denorm_absent;
	
	constexpr static bool has_denorm_loss = false;
	
	constexpr static type infinity() noexcept
	{ return type(); }
	
	constexpr static type quiet_NaN() noexcept
	{ return type(); }
	
	constexpr static type signaling_NaN() noexcept
	{ return type(); }
	
	constexpr static type denorm_min() noexcept
	{ return type(); }
	
	
	constexpr static bool is_iec559 = false;
	constexpr static bool is_bounded = false;
	constexpr static bool is_modulo = false;
	
	
	constexpr static bool traps = false;
	constexpr static bool tinyness_before = false;
	constexpr static float_round_style round_style = round_toward_zero;
};

}

#endif
#include "numeric_limits_int.hpp"