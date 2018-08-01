#ifndef INCLUDED_STRINGFWD_HH
#define INCLUDED_STRINGFWD_HH

namespace Utils {
	
	template <class>
	struct Char_Traits;
	template <class>
	class Allocator;
	
	template <class Char_t, class = Char_Traits<Char_t>, class = Allocator<Char_t>>
	class basic_string;
	typedef basic_string<char, Char_Traits<char>, Allocator<char>> string;
}

#endif