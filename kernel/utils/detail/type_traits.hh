#ifndef INCLUDED_TYPE_TRAITS_HH
#define INCLUDED_TYPE_TRAITS_HH

#include "type_traits_arithmetic.hh"

namespace Utils
{
	namespace detail
	{
		struct __sfinae_t
		{
			private:
			uint8_t one;
			uint16_t two;
			
			public:
			typedef decltype(one) __one;
			typedef decltype(two) __two;
			
			static_assert(sizeof(__one) == 1);
			static_assert(sizeof(__two) == 2);
		};
		
		
		
	}
	
	
	
	
	// remove_cv
	template <class T>
	struct remove_cv
	{
		typedef T type;
	};
	
	template <class T>
	struct remove_cv<const T> : public remove_cv<T>
	{};
	
	template <class T>
	struct remove_cv<volatile T> : public remove_cv<T>
	{ };
	
	
	
	
	// is_same
	template <class, class>
	struct is_same;
	
	template <class T>
	struct is_same<T, T> : public true_type
	{};
	
	template <class T, class G>
	struct is_same : public false_type
	{};
	
	
	
	
	// is_function
	template <class>
	struct is_function : public false_type
	{};
	
	template <class R, class... Args>
	struct is_function<R(Args...)> : public true_type
	{};
	
	template <class R, class... Args>
	struct is_function<R(Args......)> : public true_type
	{};
	
	template <class R, class... Args>
	struct is_function<R(Args...) const> : public true_type
	{};
	
	template <class R, class... Args>
	struct is_function<R(Args......) const> : public true_type
	{};
	
	template <class R, class... Args>
	struct is_function<R(Args...) volatile> : public true_type
	{};
	
	template <class R, class... Args>
	struct is_function<R(Args......) volatile> : public true_type
	{};
	
	template <class R, class... Args>
	struct is_function<R(Args...) const volatile> : public true_type
	{};
	
	template <class R, class... Args>
	struct is_function<R(Args......) const volatile> : public true_type
	{};
	
	
	
	
	// is_array
	template <class>
	struct is_array : public false_type
	{};
	
	template <class T>
	struct is_array<T[]> : public true_type
	{};
	
	template <class T, size_t N>
	struct is_array<T[N]> : public true_type
	{};
	
	
	
	
	// is_pointer
	template <class>
	struct is_pointer : public false_type
	{};
	
	template <class T>
	struct is_pointer<T*> : public true_type
	{};
	
	template <class T>
	struct is_pointer<const T> : public is_pointer<T>
	{};
	
	template <class T>
	struct is_pointer<volatile T> : public is_pointer<T>
	{};
	
	template <class T>
	struct is_pointer<const volatile T> : public is_pointer<T>
	{};
	
	
	
	
	// is_reference
	template <class>
	struct is_reference : public false_type
	{};
	
	template <class T>
	struct is_reference<T&> : public true_type
	{};
	
	
	
	
	namespace detail
	{
		template <class T>
		struct is_member_object_helper : public false_type
		{};
		
		template <class R, class C>
		struct is_member_object_helper<R C::*> : public integral_constant<bool, (!is_function<R>::value)>
		{};
		
		
		
		template <class T>
		struct is_member_function_helper : public false_type
		{};
		
		template <class R, class C, class... Args>
		struct is_member_function_helper<R(C::*)(Args...)> : public true_type
		{};
		
	}
	
	
	
	// is_member_object_pointer
	template <class T>
	struct is_member_object_pointer : public detail::is_member_object_helper<T>
	{};
	
	
	
	
	// is_member_function_pointer
	template <class T>
	struct is_member_function_pointer : public detail::is_member_function_helper<T>
	{};
	
	
	
	
	// is_member_pointer
	template <class T>
	struct is_member_pointer : public integral_constant<bool, (is_member_object_pointer<T>::value || is_member_function_pointer<T>::value)>
	{};
	
	
	
	
	namespace detail
	{
		template <class T>
		struct is_union_or_class_helper : public __sfinae_t
		{
			private:
			
			template <class Up>
			static __one __test(int Up::*);
			
			template <class>
			static __two __test(...);
			
			public:
			
			constexpr static bool value = (sizeof(__test<T>(0)) == 1);
		};
	}
	
	
	
	
	// is_union_or_class
	template <class T>
	struct is_union_or_class : public integral_constant<bool, detail::is_union_or_class_helper<T>::value>
	{};
	
	
	
	
	// is_void
	template <class>
	struct is_void : public false_type
	{};
	
	template <>
	struct is_void<void> : public true_type
	{};
	
	
	
	
	// is_fundamental
	template <class T>
	struct is_fundamental : public integral_constant<bool, (is_arithmetic<T>::value || is_void<T>::value)>
	{};
}

#endif