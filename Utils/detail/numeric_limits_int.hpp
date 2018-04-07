#ifndef INCLUDED_NUMERIC_LIMITS_INT_HPP
#define INCLUDED_NUMERIC_LIMITS_INT_HPP

namespace Utils
{

template <>
class numeric_limits<int32_t>
{
	public:
	constexpr static bool is_specialized = true;
	typedef int32_t type;
	
	constexpr static type min() noexcept
	{
		if (is_integer)
		{
			if (is_signed)
			{
				return -detail::Num_LimitFinder<int64_t, sizeof(type)/2>::value;
			}
			else
			{
				return 0;
			}
		}
	}
	
	constexpr static type max() noexcept
	{
		if (is_integer)
		{
			if (is_signed)
			{
				return detail::Num_LimitFinder<int64_t, sizeof(type)/2>::value;
			}
			else
			{
				return detail::Num_LimitFinder<uint64_t, sizeof(type)>::value;
			}
		}
	}
	
	constexpr static type lowest() noexcept
	{
		if (!is_signed)
		{
			return 0;
		}
		else if (is_integer)
		{
			return min();
		}
		else
		{
			return type();
		}
	}
	
	constexpr static int digits = 0;
	constexpr static int digits10 = 0;
	
	constexpr static bool is_signed = true;
	constexpr static bool is_integer = true;
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










template <>
class numeric_limits<int64_t>
{
	public:
	constexpr static bool is_specialized = true;
	typedef int64_t type;
	
	constexpr static type min() noexcept
	{
		if (is_integer)
		{
			if (is_signed)
			{
				return -detail::Num_LimitFinder<int64_t, sizeof(type)/2>::value;
			}
			else
			{
				return 0;
			}
		}
	}
	
	constexpr static type max() noexcept
	{
		if (is_integer)
		{
			if (is_signed)
			{
				return detail::Num_LimitFinder<int64_t, sizeof(type)/2>::value;
			}
			else
			{
				return detail::Num_LimitFinder<uint64_t, sizeof(type)>::value;
			}
		}
	}
	
	constexpr static type lowest() noexcept
	{
		if (!is_signed)
		{
			return 0;
		}
		else if (is_integer)
		{
			return min();
		}
		else
		{
			return type();
		}
	}
	
	constexpr static int digits = 0;
	constexpr static int digits10 = 0;
	
	constexpr static bool is_signed = true;
	constexpr static bool is_integer = true;
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