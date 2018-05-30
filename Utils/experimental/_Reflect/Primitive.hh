#ifndef INCLUDED_EXPERIMENTAL_REFLECT_PRIMITIVE_HH
#define INCLUDED_EXPERIMENTAL_REFLECT_PRIMITIVE_HH

#include <Common.h>
#include <Utils/type_traits>
#include <Utils/string>

namespace Utils { namespace experimental {
namespace reflect
{
	
	enum class FundamentalType
	{
		Not = 0,
		Integer,
		Float,
		String
	};
	
	template <class T>
	struct Fundamental
	{
		constexpr static FundamentalType value = FundamentalType::Not;
		typedef T type;
	};
	
	
	#define FUND(T, VAL) \
	template <> \
	struct Fundamental< T > \
	{ \
		constexpr static FundamentalType value = FundamentalType:: VAL; \
		typedef T type; \
	}
	
	FUND(uint16_t, Integer);
	FUND(uint32_t, Integer);
	FUND(uint64_t, Integer);
	FUND(int16_t, Integer);
	FUND(int32_t, Integer);
	FUND(int64_t, Integer);
	
	FUND(float, Float);
	FUND(double, Float);
	FUND(long double, Float);
	
	FUND(const char*, String);
	FUND(char*, String);
	FUND(Utils::string, String);
	
}
}
}

#endif