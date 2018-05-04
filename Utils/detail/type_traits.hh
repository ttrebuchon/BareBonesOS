#ifndef INCLUDED_TYPE_TRAITS_HH
#define INCLUDED_TYPE_TRAITS_HH

#include "type_traits_arithmetic.hh"
#include "bits/decay.hh"
#include <Utils/utility>

#define _DEFINE_SPEC_BODY(_Value) : public integral_constant<bool, _Value> {};

#define _DEFINE_SPEC_0_HELPER(_Spec, _Value) \
template <> \
struct _Spec \
_DEFINE_SPEC_BODY(_Value)

#define _DEFINE_SPEC(_Order, _Trait, _Type, _Value) \
_DEFINE_SPEC_##_Order##_HELPER(_Trait<_Type>, _Value) \
_DEFINE_SPEC_##_Order##_HELPER(_Trait<_Type const>, _Value) \
_DEFINE_SPEC_##_Order##_HELPER(_Trait<_Type volatile>, _Value) \
_DEFINE_SPEC_##_Order##_HELPER(_Trait<_Type const volatile>, _Value) \

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
	
	
	
	
	// is_trivial
	template <class T>
	struct is_trivial : public integral_constant<bool, __is_trivial(T)>
	{};
	
	
	
	
	// is_integral
	template <class T>
	struct is_integral : public false_type
	{};
	
	_DEFINE_SPEC(0, is_integral, bool, true);
	_DEFINE_SPEC(0, is_integral, char, true);
	_DEFINE_SPEC(0, is_integral, signed char, true);
	_DEFINE_SPEC(0, is_integral, unsigned char, true);
	_DEFINE_SPEC(0, is_integral, wchar_t, true);
	_DEFINE_SPEC(0, is_integral, short, true);
	_DEFINE_SPEC(0, is_integral, unsigned short, true);
	_DEFINE_SPEC(0, is_integral, int, true);
	_DEFINE_SPEC(0, is_integral, unsigned int, true);
	_DEFINE_SPEC(0, is_integral, long, true);
	_DEFINE_SPEC(0, is_integral, unsigned long, true);
	_DEFINE_SPEC(0, is_integral, long long, true);
	_DEFINE_SPEC(0, is_integral, unsigned long long, true);
	
	
	
	
	// is_floating
	template <class T>
	struct is_floating : public false_type
	{};
	
	_DEFINE_SPEC(0, is_floating, float, true);
	_DEFINE_SPEC(0, is_floating, double, true);
	_DEFINE_SPEC(0, is_floating, long double, true);
	
	
	
	// is_class
	template <class T>
	struct is_class : public integral_constant<bool, __is_class(T)>
	{};
	
	
	
	// is_volatile
	template <class T>
	struct is_volatile : public integral_constant<bool, false>
	{};
	
	template <class T>
	struct is_volatile<volatile T> : public integral_constant<bool, true>
	{};
	
	
	
	// is_const
	template <class T>
	struct is_const : public integral_constant<bool, false>
	{};
	
	template <class T>
	struct is_const<const T> : public integral_constant<bool, true>
	{};
	
	template <class T>
	struct is_const<const T*> : public integral_constant<bool, true>
	{};
	
	template <class T>
	struct is_const<T* const> : public integral_constant<bool, true>
	{};
	
	template <class T>
	struct is_const<const T* const> : public integral_constant<bool, true>
	{};
	
	
	
	// is_polymorphic
	template <class T>
	struct is_polymorphic : public integral_constant<bool, __is_polymorphic(T)>
	{ };
	
	
	
	// is_abstract
	template <class T>
	struct is_abstract : public integral_constant<bool, __is_abstract(T)>
	{ };
	
	
	
	// remove_pointer
	template <class T>
	struct remove_pointer
	{
		typedef T type;
	};
	
	template <class T>
	struct remove_pointer<T*>
	{
		typedef T type;
	};
	
	template <class T>
	struct remove_pointer<T* const>
	{
		typedef T type;
	};
	
	template <class T>
	struct remove_pointer<T* volatile>
	{
		typedef T type;
	};
	
	template <class T>
	struct remove_pointer<T* const volatile>
	{
		typedef T type;
	};
	
	
	
	// is_base_of
	namespace detail
	{
		
		template <class T, class Y>
		struct is_base_of : __sfinae_t
		{
			private:
			
			static __one __test(const volatile T*);
			
			static __two __test(const volatile void*);
			
			public:
			
			constexpr static bool value = (sizeof(decltype(__test(Utils::declval<Y*>()))) == 1);
		};
	}
	
	template <class T, class Y>
	struct is_base_of : public integral_constant<bool, detail::is_base_of<typename Utils::decay<T>::type, typename Utils::decay<Y>::type>::value>
	{ };
}

#undef _DEFINE_SPEC
#undef _DEFINE_SPEC_0_HELPER
#undef _DEFINE_SPEC_BODY

#endif