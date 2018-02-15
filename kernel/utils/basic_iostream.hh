#ifndef INCLUDED_BASIC_IOSTREAM_HH
#define INCLUDED_BASIC_IOSTREAM_HH

#include "istream.hh"
#include "ostream.hh"

namespace Utils {
	
	
	template <class T, class Traits = char_traits<T>>
	class basic_iostream : public basic_istream<T, Traits>, public basic_ostream<T, Traits>
	{
		protected:
		
		public:
		typedef T char_type;
		typedef Traits traits_type;
		typedef typename Traits::int_type int_type;
		typedef typename Traits::pos_type pos_type;
		typedef typename Traits::off_type off_type;
		
	};
	
	typedef basic_iostream<char> iostream;
	typedef basic_iostream<wchar_t> wiostream;
	
	
	
	
}

#endif