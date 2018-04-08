#ifndef INCLUDED_BLOCKFILE_HH
#define INCLUDED_BLOCKFILE_HH

#include <Common.h>
#include <Utils/vector>
#include "File_streambuf.hh"

namespace Kernel { namespace Filesystem
{
	template <size_t BlkSize>
	struct Block
	{
		bool dirty : 1;
		//size_t needsReadTil;
		unsigned char data[BlkSize];
		
		Block() : dirty(false), data()
		{
			
		}
	};
	
	class BlockFile : public File_streambuf
	{
		public:
		constexpr static size_t BlockSize = 512;
		
		protected:
		Utils::vector<Block<BlockSize>*> blocks;
		
		virtual unsigned char* addrForPos(size_t, bool forReading, size_t* remainingContinuous = nullptr, unsigned char** blockBegin = nullptr);
		virtual size_t basePosForPos(const size_t pos) const noexcept;
		virtual int fileSync();
		
		void expandToBlock(size_t index);
		bool readTo(size_t index);
		bool writeTo(size_t index, bool force = false);
		
		
		public:
		
		BlockFile(FileNode*);
	};
	
}
}

#endif