#include "BlockFile.hh"
#include "../FileNode.hh"

namespace Kernel { namespace Filesystem
{
	
	BlockFile::BlockFile(FileNode* fnode) : File_streambuf(fnode), blocks()
	{
		
	}
	
	
	
	
	
	unsigned char* BlockFile::addrForPos(size_t pos, bool forReading, size_t* remainingContinuous, unsigned char** blockStart)
	{
		if (remainingContinuous)
		{
			*remainingContinuous = 0;
		}
		
		if (blockStart)
		{
			*blockStart = nullptr;
		}
		
		if (forReading && pos >= size)
		{
			return nullptr;
		}
		
		const size_t blockIndex = pos / BlockSize;
		if (blocks.size() <= blockIndex)
		{
			expandToBlock(blockIndex);
			/*if (forReading)
			{*/
				if (!readTo(blockIndex))
				{
					ASSERT(!forReading);
				}
			//}
		}
		
		if (!blocks[blockIndex])
		{
			if (!readTo(blockIndex))
			{
				ASSERT(!forReading);
			}
		}
		
		const size_t offset = pos % BlockSize;
		
		if (remainingContinuous)
		{
			*remainingContinuous = BlockSize - offset;
		}
		if (blockStart)
		{
			*blockStart = blocks[blockIndex]->data;
		}
		return blocks[blockIndex]->data + offset;
	}
	
	size_t BlockFile::basePosForPos(const size_t pos) const noexcept
	{
		const size_t blockIndex = pos / BlockSize;
		return blockIndex * BlockSize;
	}
	
	int BlockFile::fileSync()
	{
		for (size_t i = 0; i < blocks.size(); ++i)
		{
			if (blocks[i])
			{
				if (blocks[i]->dirty)
				{
					auto wrote = __node->write(i*BlockSize, BlockSize, reinterpret_cast<uint8_t*>(blocks[i]->data));
					blocks[i]->dirty = false;
					ASSERT(wrote == BlockSize);
				}
			}
		}

		// TODO: Figure out what to return
		ASSERT(false);
	}
	
	
	
	
	void BlockFile::expandToBlock(size_t index)
	{
		if (blocks.size() <= index)
		{
			blocks.resize(index+1);
			readTo(index);
			size = blocks.size() * BlockSize;
		}
	}
	
	bool BlockFile::readTo(size_t index)
	{
		ASSERT(blocks.size() > index);
		if (!blocks[index])
		{
			blocks[index] = new Block<BlockSize>();
		}
		else if (blocks[index]->dirty)
		{
			return false;
		}
		
		if (__node->size() < (index + 1)*BlockSize)
		{
			return false;
		}
		
		ASSERT(__node->read(index*BlockSize, BlockSize, reinterpret_cast<uint8_t*>(blocks[index]->data)) == BlockSize);
		return true;
	}
	
	bool BlockFile::writeTo(size_t index, bool force)
	{
		ASSERT(blocks.size() > index);
		Block<BlockSize>* b = blocks[index];
		if (!b)
		{
			return false;
		}
		
		if (b->dirty || force)
		{
			const auto written = __node->write(index*BlockSize, BlockSize, reinterpret_cast<const uint8_t*>(b->data));
			ASSERT(written == BlockSize);
			return true;
		}
		else
		{
			return false;
		}
	}
	
}
}