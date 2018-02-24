#include "Tests.hh"
#include <kernel/utils/limits>
#include <kernel/utils/type_traits>

using namespace Utils;

TEST(Limits)
{
	numeric_limits<const int> n;
	static_assert(is_arithmetic<uint32_t>::value);
	static_assert(is_arithmetic<int>::value);
	static_assert(is_arithmetic<const uint32_t>::value);
	static_assert(is_arithmetic<volatile uint32_t>::value);
	static_assert(is_arithmetic<volatile const uint32_t>::value);
	static_assert(is_arithmetic<int32_t>::value);
	
	static_assert(is_same<int, int>::value);
	
	static_assert(is_same<unsigned int, uint32_t>::value);
	
	
	static_assert(is_array<int[]>::value);
	{
		int x[4];
		static_assert(is_same<decltype(x), int[4]>::value);
		static_assert(is_array<decltype(x)>::value);
	}
	
	{
		typedef const int T1;
		typedef T1* P1;
		typedef const P1 CP1;
		typedef volatile P1 VP1;
		typedef const VP1 CVP1;
		static_assert(is_pointer<T1*>::value);
		static_assert(is_same<T1*, P1>::value);
		static_assert(is_same<T1* const, CP1>::value);
		static_assert(is_pointer<CP1>::value);
		static_assert(is_pointer<VP1>::value);
		static_assert(is_pointer<CVP1>::value);
		static_assert(is_pointer<T1* const volatile>::value);
		
	}
	
	
	{
		typedef int T1;
		typedef volatile int T2;
		typedef const int T3;
		typedef const T2 T4;
		typedef T1& R1;
		typedef T2& R2;
		typedef T3& R3;
		typedef T4& R4;
		
		static_assert(is_reference<T1&>::value);
		static_assert(is_same<T1&, R1>::value);
		static_assert(is_same<T2&, R2>::value);
		static_assert(is_reference<R2>::value);
		static_assert(is_reference<R3>::value);
		static_assert(is_reference<R4>::value);
		static_assert(is_reference<R1&>::value);
		
		static_assert(!is_reference<T1>::value);
		static_assert(!is_reference<T2>::value);
		static_assert(!is_reference<T3>::value);
		static_assert(!is_reference<T4>::value);
	}
	
	
	{
		class Foo
		{
			public:
			int X;
			
		};
		
		auto x = &Foo::X;
		
		static_assert(!is_member_object_pointer<int>::value);
		static_assert(is_member_object_pointer<decltype(x)>::value);
		
		static_assert(!is_member_function_pointer<decltype(x)>::value);
		
		
		struct Bar
		{
			int Y;
			
		};
		
		auto y = &Bar::Y;
		static_assert(is_member_object_pointer<decltype(y)>::value);
		
	}
	
	
	
	
	{
		class Foo
		{
			public:
			int X() { return 0; }
			
		};
		
		auto x = &Foo::X;
		static_assert(!is_member_object_pointer<decltype(x)>::value);
		static_assert(is_member_function_pointer<decltype(x)>::value);
		
		
		struct Bar
		{
			int Y() { return 0; }
			
		};
		
		auto y = &Bar::Y;
		static_assert(is_member_function_pointer<decltype(y)>::value);
		
	}
	
	
	
	
	{
		class Foo
		{
			public:
			
		};
		
		
		static_assert(is_union_or_class<Foo>::value);
		static_assert(!is_union_or_class<int>::value);
		
		
		struct Bar
		{
			
			
		};
		
		static_assert(is_union_or_class<Bar>::value);
		
	}
	
	static_assert(!is_void<int>::value);
	static_assert(is_void<void>::value);
	
	{
		enum T
		{
			
		};
		
		static_assert(!is_enum<int>::value);
		static_assert(is_enum<T>::value);
		
		struct Foo
		{
			
		};
		
		static_assert(!is_enum<Foo>::value);
		
		enum class T2
		{
			x = 0,
		};
		
		static_assert(is_enum<T2>::value);
	}
}