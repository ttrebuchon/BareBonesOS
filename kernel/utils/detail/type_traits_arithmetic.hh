#ifndef INCLUDED_TYPE_TRAITS_ARITHMETIC_HH
#define INCLUDED_TYPE_TRAITS_ARITHMETIC_HH

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
	
	
	template <class T>
	struct is_arithmetic;
	
	template <class T>
	struct is_arithmetic<const T> : public is_arithmetic<T> {};
	template <class T>
	struct is_arithmetic<volatile T> : public is_arithmetic<T> {};
	template <class T>
	struct is_arithmetic<const volatile T> : public is_arithmetic<T> {};
	
	template <class T>
	struct is_arithmetic : public false_type
	{};
	
	
	
	
	
	template <>
	struct is_arithmetic<bool> : public true_type
	{};
	
	template <>
	struct is_arithmetic<char> : public true_type
	{};
	
	template <>
	struct is_arithmetic<char16_t> : public true_type
	{};
	
	template <>
	struct is_arithmetic<char32_t> : public true_type
	{};
	
	template <>
	struct is_arithmetic<wchar_t> : public true_type
	{};
	
	template <>
	struct is_arithmetic<signed char> : public true_type
	{};
	
	template <>
	struct is_arithmetic<short> : public true_type
	{};
	
	template <>
	struct is_arithmetic<int> : public true_type
	{};
	
	template <>
	struct is_arithmetic<long> : public true_type
	{};
	
	template <>
	struct is_arithmetic<long long> : public true_type
	{};
	
	template <>
	struct is_arithmetic<unsigned char> : public true_type
	{};
	
	template <>
	struct is_arithmetic<unsigned short> : public true_type
	{};
	
	template <>
	struct is_arithmetic<unsigned int> : public true_type
	{};
	
	template <>
	struct is_arithmetic<unsigned long> : public true_type
	{};
	
	template <>
	struct is_arithmetic<unsigned long long> : public true_type
	{};
	
	template <>
	struct is_arithmetic<float> : public true_type
	{};
	
	template <>
	struct is_arithmetic<double> : public true_type
	{};
	
	template <>
	struct is_arithmetic<long double> : public true_type
	{};
}


#endif