#ifndef INCLUDED_IOFSTREAM_HH
#define INCLUDED_IOFSTREAM_HH

#include <Utils/iostream>
#include "File_streambuf.hh"

namespace Kernel { namespace FS
{
	template <class T, class Traits = Utils::char_traits<T>>
	class basic_iofstream : public virtual Utils::basic_ostream<T, Traits>, public virtual Utils::basic_istream<T, Traits>
	{
		public:
		typedef Utils::basic_ostream<T, Traits> out;
		typedef Utils::basic_istream<T, Traits> in;
		typedef typename out::char_type char_type;
		typedef typename out::traits_type traits_type;
		
		private:
		
		public:
		explicit basic_iofstream(File_streambuf<char_type, traits_type>*);
	};
	
	
	typedef basic_iofstream<char> iofstream;
}
}

#endif