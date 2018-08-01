#ifndef INCLUDED_PATH_FWD_HH
#define INCLUDED_PATH_FWD_HH

#include <Utils/Char_Traits.hh>

namespace Kernel { namespace FS
{
	
	template <class Char_t, class Traits = Utils::char_traits<Char_t>, class Alloc = Utils::allocator<Char_t>>
	class basic_path;
	
	typedef basic_path<char> Path_t;
	
	
}
}

#endif