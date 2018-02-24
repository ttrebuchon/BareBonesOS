#ifndef INCLUDED_NUMERIC_LIMITS_HH
#define INCLUDED_NUMERIC_LIMITS_HH

namespace Utils {

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
	
	constexpr static T min() noexcept
	{ return T(); }
	
	constexpr static T max() noexcept
	{ return T(); }
	
	constexpr static T lowest() noexcept
	{ return T(); }
	
	constexpr static int digits = 0;
	constexpr static int digits10 = 0;
	
	constexpr static bool is_signed = false;
	constexpr static bool is_integer = false;
	constexpr static bool is_exact = false;
	constexpr static int radix = 0;
	
	constexpr static T epsilon() noexcept
	{ return T(); }
	
	constexpr static T round_error() noexcept
	{ return T(); }
	
	constexpr static int min_exponent = 0;
	constexpr static int min_exponent10 = 0;
	constexpr static int max_exponent = 0;
	constexpr static int max_exponent10 = 0;
	
	constexpr static bool has_infinity = false;
	constexpr static bool has_quiet_NaN = false;
	constexpr static bool has_signaling_NaN = false;
	
	constexpr static float_denorm_style has_denorm = denorm_absent;
	
	constexpr static bool has_denorm_loss = false;
	
	constexpr static T infinity() noexcept
	{ return T(); }
	
	constexpr static T quiet_NaN() noexcept
	{ return T(); }
	
	constexpr static T signaling_NaN() noexcept
	{ return T(); }
	
	constexpr static T denorm_min() noexcept
	{ return T(); }
	
	
	constexpr static bool is_iec559 = false;
	constexpr static bool is_bounded = false;
	constexpr static bool is_modulo = false;
	
	
	constexpr static bool traps = false;
	constexpr static bool tinyness_before = false;
	constexpr static float_round_style round_style = round_toward_zero;
};

}

#endif