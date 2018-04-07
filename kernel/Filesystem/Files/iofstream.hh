#ifndef INCLUDED_IOFSTREAM_HH
#define INCLUDED_IOFSTREAM_HH

#include <Utils/iostream>

namespace Kernel { namespace Filesystem
{
	template <class T, class Traits = Utils::char_traits<T>>
	class basic_iofstream : public virtual Utils::basic_ostream<T, Traits>, public virtual Utils::basic_istream<T, Traits>
	{
		
	};
	
	
	typedef basic_iofstream<char> iofstream;
}
}

#endif