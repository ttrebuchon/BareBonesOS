#ifndef INCLUDED_IOSTREAM_HH
#define INCLUDED_IOSTREAM_HH

#include "ostream.hh"
#include "istream.hh"
#include "streambuf.hh"

namespace Utils
{
	template <class T, class Traits = Char_Traits<T>>
	class basic_iostream : public basic_istream<T, Traits>, public basic_ostream<T, Traits>
	{
		private:
		
		
		protected:
		
		basic_iostream(basic_iostream&&);
		
		
		public:
		#define BASETYPE(X) typedef typename basic_ios<T, Traits>:: X X
		
		BASETYPE(char_type);
		BASETYPE(int_type);
		BASETYPE(pos_type);
		BASETYPE(off_type);
		BASETYPE(traits_type);
		
		#undef BASETYPE
		
		
		
		explicit basic_iostream(basic_streambuf<T, Traits>*);
		
		basic_iostream(const basic_iostream&) = delete;
		
	};
	
	typedef basic_iostream<char> iostream;
	typedef basic_iostream<wchar_t> wiostream;
	
}

#endif