#ifndef INCLUDED_UTILS_IOSTREAM_FWD_HH
#define INCLUDED_UTILS_IOSTREAM_FWD_HH

namespace Utils
{
	template <class>
	struct Char_Traits;
	
	
	template <class T, class Traits>
	class basic_ostream;
	template <class T, class Traits>
	class basic_istream;
	template <class T, class Traits>
	class basic_iostream;
	
	
	typedef basic_ostream<char, Char_Traits<char>> ostream;
	typedef basic_istream<char, Char_Traits<char>> istream;
	typedef basic_iostream<char, Char_Traits<char>> iostream;
}

#endif