#ifndef INCLUDED_FILE_STREAMBUF_HH
#define INCLUDED_FILE_STREAMBUF_HH

#include <Common.h>
#include <Utils/streambuf>

namespace Kernel { namespace Filesystem
{
	
	template <class T, class Traits = Utils::Char_Traits<T>>
	class File_streambuf : public Utils::basic_streambuf<T, Traits>
	{
		
	};
	
	
}
}

#endif