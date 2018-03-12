#ifndef INCLUDED_TYPE_TRAITS_ENUM_HH
#define INCLUDED_TYPE_TRAITS_ENUM_HH

#include "type_traits.hh"
#include "type_traits_arithmetic.hh"

namespace Utils
{
	template <class T>
	struct is_enum : public integral_constant<bool, !(
		is_fundamental<T>::value ||
		is_array<T>::value ||
		is_pointer<T>::value ||
		is_reference<T>::value ||
		is_member_pointer<T>::value ||
		is_function<T>::value ||
		is_union_or_class<T>::value
	)>
	{};
	
	
}

#endif