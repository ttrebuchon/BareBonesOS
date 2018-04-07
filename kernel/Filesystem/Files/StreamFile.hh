#ifndef INCLUDED_STREAMFILE_HH
#define INCLUDED_STREAMFILE_HH

#include <Common.h>
#include "../File.hh"
#include "File_streambuf.hh"
#include <Utils/Byte_Traits.hh>
#include <Utils/iostream>

namespace Kernel { namespace Filesystem
{
	
	
	class StreamFile : public File
	{
		protected:
		Utils::basic_iostream<uint8_t, Utils::byte_traits> stream;
		
		public:
		
		
	};
	
	
	
	
}
}

#endif