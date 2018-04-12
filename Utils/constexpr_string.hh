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
	
	namespace detail
	{
		template <class T>
		struct const_string_extensions
		{
			const T obj;
			constexpr const_string_extensions(const T& obj) : obj(obj)
			{
				
			}
			
			constexpr const_string_extensions(const const_string_extensions& e) : obj(e.obj)
			{}
			
			constexpr const_string_extensions(const_string_extensions&& e) : obj(e.obj)
			{}
			
			//constexpr const_string_extensions& operator=(const const_string_extensions&) = default;
			
			constexpr auto getN_int(const unsigned n) const
			{
				return ((unsigned)obj.getN(n)) - 48;
			}
			
			constexpr auto parse_int() const
			{
				return (obj.length() == 1) ? obj.getN_int(0) : obj.getN_int(0)*pow(10, obj.length()-1) + obj.template substr<1>().parse_int();
			}
		};
	}
	
	template <unsigned Size, unsigned Start = 0, unsigned End = Size-1>
	struct const_string
	{
		private:
		typedef detail::const_string_extensions<const_string> Exts;
		
		constexpr auto exts() const noexcept
		{
			return detail::const_string_extensions<const_string<Size, Start, End>>(const_string(str));
		}
		
		public:
		const char (&str) [Size];
		
		constexpr static unsigned Length = End - Start;
		
		static_assert(Start < Size-1);
		static_assert(Size > 0);
		
		constexpr const_string() = delete;
		
		constexpr const_string(const char (&arr) [Size]) : str(arr)
		{
			REQUIRES(End < Size);
			REQUIRES(Start <= End);
			//static_assert(Start <= End);
			//static_assert(End < Size);
		}
		
		constexpr const_string(const const_string& c) : str(c.str)
		{
			REQUIRES(End < Size);
			REQUIRES(Start <= End);
			//static_assert(Start <= End);
			//static_assert(End < Size);
		}
		
		//constexpr const_string(const_string&& o) = delete;
		
		constexpr const_string(const_string&& o) : str((const char (&) [Size])o.str)
		{
		}
		
		constexpr const_string& operator=(const const_string&) = delete;
		constexpr const_string& operator=(const_string&&) = delete;
		
		constexpr unsigned inRange(unsigned n) const
		{
			return requires_InRange(n, length()+1);
		}
		
		constexpr char getN(const unsigned n) const
		{
			return inRange(n), str[n+Start];
		}
		
		constexpr unsigned getN_int(const unsigned n) const
		{
			return exts().getN_int(n);
			return ((unsigned)getN(n)) - 48;
		}
		
		constexpr unsigned parse_int() const
		{
			return exts().parse_int();
		}
		
		
		template <unsigned start, unsigned newlen = Length - start>
		constexpr auto substr() const
		{
			static_assert(Start + Length == End);
			static_assert(newlen >= 0);
			static_assert(newlen <= Length);
			constexpr unsigned nStart = Start + start;
			constexpr unsigned nEnd = nStart + newlen;
			static_assert(nStart <= nEnd);
			static_assert(nEnd <= Size);
			return /*inRange(start), inRange(start + ), */const_string<Size, nStart, nEnd>(str);
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
		private:
		typedef detail::const_string_extensions<const_string> Exts;
		
		constexpr auto exts() const noexcept
		{
			return detail::const_string_extensions<const_string<Size, Start, Start>>(const_string(str));
		}
		
		public:
		
		constexpr static unsigned Length = 0;
		const char (&str) [Size];
		
		constexpr const_string(const char (&arr) [Size]) : str(arr)
		{
			
		}
		
		constexpr const_string(const const_string& c) : str(c.str)
		{
			
		}
		
		//constexpr const_string(const_string&& o) = delete;
		
		constexpr const_string(const_string&& o) : str((const char (&) [Size])o.str)
		{
		}
		
		constexpr const_string& operator=(const const_string&) = delete;
		constexpr const_string& operator=(const_string&&) = delete;
		
		
		
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
	
	
	
	
	template <class T1, class T2, unsigned Start = 0, unsigned End = T1::Length + T2::Length>
	struct concat_const_string
	{
		private:
		
		
		constexpr auto exts() const noexcept
		{
			return detail::const_string_extensions<concat_const_string>(concat_const_string(str1, str2));
		}
		
		public:
		T1 str1;
		T2 str2;
		
		constexpr static unsigned Length = End - Start;
		
		constexpr concat_const_string(T1 s1, T2 s2) : str1(s1), str2(s2)
		{
			
		}
		
		constexpr char getN(const unsigned n) const
		{
			return (n + Start < str1.length()) ? str1.getN(n+Start) : str2.getN(n + Start - str1.length());
		}
		
		constexpr unsigned getN_int(const unsigned n) const
		{
			return exts().getN_int(n);
		}
		
		constexpr unsigned parse_int() const
		{
			return exts().parse_int();
		}
		
		constexpr unsigned length() const
		{
			return Length;
		}
		
		template <unsigned start, unsigned newlen = Length - start>
		constexpr auto substr() const
		{
			static_assert(Start + Length == End);
			static_assert(newlen >= 0);
			static_assert(newlen <= Length);
			constexpr unsigned nStart = Start + start;
			constexpr unsigned nEnd = nStart + newlen;
			static_assert(nStart <= nEnd);
			static_assert(nEnd <= End);
			return /*inRange(start), inRange(start + ), */concat_const_string<T1, T2, nStart, nEnd>(str1, str2);
		}
	};
	
	
	
	template <class T1, class T2, unsigned Start>
	struct concat_const_string<T1, T2, Start, Start>
	{
		private:
		
		
		constexpr auto exts() const noexcept
		{
			return detail::const_string_extensions<concat_const_string>(concat_const_string(str1, str2));
		}
		
		public:
		T1 str1;
		T2 str2;
		
		constexpr static unsigned Length = 0;
		
		constexpr concat_const_string(T1 s1, T2 s2) : str1(s1), str2(s2)
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
		
		constexpr unsigned length() const
		{
			return Length;
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
	
	
	static_assert(cstring("foo", "bar").getN(0) == 'f');
	static_assert(cstring("foo", "bar").getN(1) == 'o');
	static_assert(cstring("foo", "bar").getN(2) == 'o');
	static_assert(cstring("foo", "bar").getN(3) == 'b');
	static_assert(cstring("foo", "bar").getN(4) == 'a');
	static_assert(cstring("foo", "bar").getN(5) == 'r');
	static_assert(cstring("foo", "bar").length() == 6);
	static_assert(cstring("foo", "bar").substr<0>().length() == 6);
	static_assert(cstring("foo", "bar").substr<1>().length() == 5);
	static_assert(cstring("foo", "bar").substr<2>().length() == 4);
	static_assert(cstring("foo", "bar").substr<3>().length() == 3);
	static_assert(cstring("foo", "bar").substr<4>().length() == 2);
	static_assert(cstring("foo", "bar").substr<5>().length() == 1);
	static_assert(cstring("foo", "bar").substr<6>().length() == 0);
	
	static_assert(cstring("100", "2").parse_int() == 1002);
	static_assert(cstring("100", "").parse_int() == 100);
}
}

#undef REQUIRES

#endif