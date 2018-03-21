#ifndef INCLUDED_BITS_IS_RVALUE_REFERENCE_HH
#define INCLUDED_BITS_IS_RVALUE_REFERENCE_HH

#include <Utils/detail/type_traits_arithmetic.hh>

namespace Utils
{
	// is_rvalue_reference
	template <class>
	struct is_rvalue_reference : public false_type
	{};
	
	template <class T>
	struct is_rvalue_reference<T&&> : public true_type
	{};
}

#endif