#ifndef INCLUDED_CONSTEXPR_STRING_HH
#define INCLUDED_CONSTEXPR_STRING_HH

#include <Common.h>

namespace Utils { namespace compile
{
	/*template <unsigned...>
	struct num_pack;
	
	template <class...>
	struct type_pack;*/
	
	constexpr unsigned pow(unsigned base, unsigned p)
	{
		if (p == 0)
		{
			return 1;
		}
		return (p == 0) ? 1 : base*pow(base, p-1);
	}
	
	constexpr unsigned requires_InRange(unsigned i, unsigned len)
	{
		return i >= len ? *(unsigned*)nullptr : i;
	}
	
	#define REQUIRES(X) { (!(X) ? *(unsigned*)nullptr : 1); }
	
	template <unsigned, unsigned, unsigned>
	struct const_string;
	
	/*template <int N, int Start = 0, int End = N-1>
	using const_string = _const_string<num_pack<N, Start, End>, type_pack<>>;*/
	
	template <unsigned Size, unsigned Start = 0, unsigned End = Size-1>
	struct const_string
	{
		const char (&str) [Size];
		
		constexpr static unsigned Length = End - Start;
		
		
		constexpr const_string(const char (&arr) [Size]) : str(arr)
		{
			REQUIRES(End < Size);
			REQUIRES(Start <= End);
			//static_assert(Start <= End);
			//static_assert(End < Size);
		}
		
		constexpr unsigned inRange(unsigned n) const
		{
			return requires_InRange(n, length()+1);
		}
		
		constexpr char getN(unsigned n) const
		{
			return inRange(n), str[n+Start];
		}
		
		constexpr unsigned getN_int(unsigned n) const
		{
			return ((unsigned)getN(n)) - 48;
		}
		
		constexpr unsigned parse_int() const
		{
			return (length() == 1) ? getN_int(0) : getN_int(0)*pow(10, length()-1) + substr<1>().parse_int();
		}
		
		
		template <unsigned start, unsigned end = End - Start>
		constexpr auto substr() const
		{
			constexpr unsigned nStart = Start + start;
			constexpr unsigned nEnd = Start + end;
			static_assert(nStart <= nEnd);
			static_assert(nEnd <= Size);
			return inRange(start), inRange(end), const_string<Size, nStart, nEnd>(str);
		}
		
		constexpr bool foo() const
		{ return true; }
		
		constexpr unsigned length() const
		{
			// return End - Start;
			return Length;
		}
		
		constexpr unsigned absolute(unsigned i) const
		{
			return i + Start;
		}
		
		constexpr unsigned relative(unsigned i) const
		{
			return requires_InRange(i, Start), i - Start;
		}
		//static_assert(const_string().foo());
	};
	
	
	template <unsigned Size, unsigned Start>
	struct const_string<Size, Start, Start>
	{
		constexpr static unsigned Length = 0;
		
		
		constexpr const_string(const char (&arr) [Size])
		{
			
		}
		
		
		
		constexpr char getN(unsigned n) const
		{
			return *(char*)nullptr;
		}
		
		constexpr unsigned getN_int(unsigned n) const
		{
			return ((unsigned)getN(n)) - 48;
		}
		
		constexpr unsigned parse_int() const
		{
			return 0;
		}
		
		
		template <unsigned start, unsigned end = Start>
		constexpr auto substr() const
		{
			return *(unsigned*)nullptr;
		}
		
		constexpr bool foo() const
		{ return true; }
		
		constexpr unsigned length() const
		{
			return 0;
		}
	};
	
	
	template <class, class>
	struct concat_const_string;
	
	template <class T1, class T2>
	struct concat_const_string
	{
		T1 str1;
		T2 str2;
		
		constexpr concat_const_string(T1 s1, T2 s2) : str1(s1), str2(s2)
		{
			
		}
		
		
	};
	
	
	
	template <class T>
	struct cstring_verifier
	{
		typedef T type;
		
		constexpr static unsigned Length = type::Length;
	};
	
	typedef typename cstring_verifier<const_string<4, 1, 2>>::type ggvt;
	
	//typedef _concat_const_string<num_pack<8, 2, 8>, type_pack<const_string<4, 1, 2>, const_string<4, 1, 2>>> ggyhf;
	
	template <unsigned N>
	using raw_string = const char (&) [N];
	
	
	template <int N>
	constexpr const_string<N> cstring(const char (&arr) [N])
	{
		return const_string<N>(arr);
	}
	
	
	template <class T1, class T2>
	constexpr auto cstring(T1 t1, T2 t2)
	{
		return concat_const_string<T1, T2>(t1, t2);
	}
	
	
	template <class T1, class T2, class T3, class... Ts>
	constexpr auto cstring(T1 t1, T2 t2, T3 t3, Ts... ts)
	{
		return cstring(cstring(t1, t2), t3, ts...);
	}
	
	template <unsigned N1, unsigned N2, unsigned... Ns>
	constexpr auto cstring(raw_string<N1> arr1, raw_string<N2> arr2, raw_string<Ns>... arrs)
	{
		typedef const_string<N1> T1;
		typedef const_string<N2> T2;
		return cstring(T1(arr1), T2(arr2), const_string<Ns>(arrs)...);
	}
	
	template <unsigned n>
	struct constexpr_launder
	{
		constexpr static unsigned value = n;
	};
	
	
	
	
	static_assert(const_string<4>("foo").foo());
	static_assert(cstring("foo").substr<2>().getN(0) == 'o');
	
	static_assert(cstring("foo").getN(0) == 'f');
	static_assert(cstring("foo").getN(1) == 'o');
	static_assert(cstring("foo").getN(2) == 'o');
	static_assert(cstring("foo").length() == 3);
	
	static_assert(cstring("foo").substr<0>().getN(0) == 'f');
	static_assert(cstring("foo").substr<0>().getN(1) == 'o');
	static_assert(cstring("foo").substr<0>().getN(2) == 'o');
	static_assert(cstring("foo").substr<0>().length() == 3);
	
	static_assert(cstring("foo").substr<1>().getN(0) == 'o');
	static_assert(cstring("foo").substr<1>().getN(1) == 'o');
	static_assert(cstring("foo").substr<1>().length() == 2);
	
	
	static_assert(cstring("foo").substr<2>().getN(0) == 'o');
	static_assert(cstring("foo").substr<2>().length() == 1);
	
	
	static_assert(cstring("").parse_int() == 0);
	
	static_assert(cstring("1").parse_int() == 1);
	static_assert(cstring("10").parse_int() == 10);
	static_assert(cstring("100").parse_int() == 100);
	
	
	static_assert(cstring(__DATE__).substr<7>().parse_int() == 2018);
}
}

#undef REQUIRES

#endif