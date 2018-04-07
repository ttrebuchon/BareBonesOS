#ifndef INCLUDED_TYPE_TRAITS_ARITHMETIC_HH
#define INCLUDED_TYPE_TRAITS_ARITHMETIC_HH

#include "bits/integral_constant.hh"

namespace Utils
{
	
	
	
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