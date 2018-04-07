#ifndef INCLUDED_CHAR_TRAITS_HH
#define INCLUDED_CHAR_TRAITS_HH

#include <Common.h>
#include "cwchar.hh"

namespace Utils
{
	template <class>
	struct Char_Traits;
	
	template <class StateT>
	class fpos
	{
		private:
		StateT _state;
		public:
		StateT state() const
		{ return _state; }
		void state(StateT state)
		{ _state = state; }
	};
	
	typedef fpos<mbstate_t> streampos;
	
	template <>
	struct Char_Traits<char>
	{
		using char_type = char;
		using int_type = int;
		using off_type = unsigned long long;
		using pos_type = streampos;
		using state_type = mbstate_t;
		static constexpr void assign(char_type& c1, const char_type& c2) noexcept
		{
			c1 = c2;
		}
		
		static constexpr bool eq(char_type c1, char_type c2) noexcept;
		static constexpr bool lt(char_type c1, char_type c2) noexcept;
		static constexpr int compare(const char_type* s1, const char_type* s2, size_t n)
		{
			for (size_t i = 0; i < n; ++i)
			{
				if (s1[i] != s2[i])
				{
					if (s1[i] < s2[i])
					{
						return -1;
					}
					else
					{
						return 1;
					}
				}
			}
			return 0;
		}
		
		static constexpr size_t length(const char_type* s)
		{
			size_t i = 0;
			for (i = 0; s[i] != 0; ++i);
			return i;
		}
		static constexpr const char_type* find(const char_type* s, size_t n, const char_type& a);
		static char_type* move(char_type* s1, const char_type* s2, size_t n);
		
		static char_type* copy(char_type* dest, const char_type* src, size_t n)
		{
			for (unsigned int i = 0; i < n; ++i)
			{
				dest[i] = src[i];
			}
			return dest;
		}
		
		static char_type* assign(char_type* s, size_t n, char_type a)
		{
			for (size_t i = 0; i < n; ++i)
			{
				s[i] = a;
			}
			return s;
		}
		
		static constexpr int_type not_eof(int_type c) noexcept
		{
			if (c >= 0) return c;
			
			return 0;
		}
		
		static constexpr char_type to_char_type(int_type c) noexcept;
		
		static constexpr int_type to_int_type(char_type c) noexcept
		{
			return (unsigned char)c;
		}
		
		static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept
		{
			return c1 == c2;
		}
		
		static constexpr int_type eof() noexcept { return -1; }
		
		static constexpr char_type terminal()
		{
			return char_type();
		}
		
	};
	
	template <class T>
	using char_traits = Char_Traits<T>;
}

#endif
#include "Char_Traits.hpp"