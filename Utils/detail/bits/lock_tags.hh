#ifndef INCLUDED_BITS_LOCK_TAGS_HH
#define INCLUDED_BITS_LOCK_TAGS_HH

#include <Common.h>

namespace Utils
{
	struct try_to_lock_t
	{
		constexpr explicit try_to_lock_t() noexcept = default;
	};
	
	struct adopt_lock_t
	{
		constexpr explicit adopt_lock_t() noexcept = default;
	};
	
	struct defer_lock_t
	{
		constexpr explicit defer_lock_t() noexcept = default;
	};
	
	
	IFCPP17(inline) constexpr try_to_lock_t try_to_lock {};
	
	IFCPP17(inline) constexpr adopt_lock_t adopt_lock {};
	
	IFCPP17(inline) constexpr defer_lock_t defer_lock {};
	
}

#endif