#ifndef INCLUDED_STRINGFWD_HH
#define INCLUDED_STRINGFWD_HH

namespace Utils {
	
	template <class>
	struct Char_Traits;
	template <class>
	class Allocator;
	
	template <class, class, class>
	class Basic_String;
	typedef Basic_String<char, Char_Traits<char>, Allocator<char>> String;
}

#endif