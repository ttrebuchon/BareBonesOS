#ifndef INCLUDED_STREAMFILE_HH
#define INCLUDED_STREAMFILE_HH

#include <Common.h>
#include "../File.hh"
#include "File_streambuf.hh"
#include <Utils/iostream>

namespace Kernel { namespace FS
{
	
	
	class StreamFile : public File
	{
		protected:
		Utils::basic_iostream<unsigned char> stream;
		
		public:
		
		
	};
	
	
	
	
}
}

#endif