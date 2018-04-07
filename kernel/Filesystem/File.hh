#ifndef INCLUDED_FILE_HH
#define INCLUDED_FILE_HH

#include <Common.h>
#include <Utils/vector>
#include "Files/iofstream.hh"

namespace Kernel { namespace Filesystem
{
	class FileNode;
	
	class File
	{
		protected:
		FileNode* __node;
		
		
		public:
		iofstream stream;
		
	};
	
	
	
	
}
}

#endif