#ifndef INCLUDED_BITS_IS_LVALUE_REFERENCE_HH
#define INCLUDED_BITS_IS_LVALUE_REFERENCE_HH

#include <Utils/detail/type_traits_arithmetic.hh>

namespace Utils
{
	// is_lvalue_reference
	template <class>
	struct is_lvalue_reference : public false_type
	{};
	
	template <class T>
	struct is_lvalue_reference<T&> : public true_type
	{};
}

#endif