#ifndef INCLUDED_BLOCKFILE_HH
#define INCLUDED_BLOCKFILE_HH

#include <Common.h>
#include <Utils/vector>

namespace Kernel { namespace Filesystem
{
	
	struct FileBlock
	{
		size_t index;
		unsigned char* data;
	};
	
	class BlockFile
	{
		protected:
		Utils::vector<FileBlock*> blocks;
		
		public:
		
		
	};
	
	
	
	
}
}

#endif