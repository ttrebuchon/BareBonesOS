#ifndef INCLUDED_SEGMENTFILE_HH
#define INCLUDED_SEGMENTFILE_HH

#include <Common.h>
#include <Utils/vector>

namespace Kernel { namespace Filesystem
{
	
	struct FileSegment
	{
		size_t length;
		size_t reserved;
		size_t pos;
		unsigned char* data;
	};
	
	class SegmentFile
	{
		protected:
		Utils::vector<FileSegment*> segments;
		
		public:
		
		
	};
	
	
	
	
}
}

#endif