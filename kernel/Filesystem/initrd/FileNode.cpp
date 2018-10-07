#include "FileNode.hh"
#include <kernel/Filesystem/initrd.hh>

namespace Kernel { namespace FS { namespace Init_RD {
	


	FileNode::FileNode(InitRD_FS* fs, const char* name, void* data, uint64_t size) noexcept : FS::FileNode(NodeType::RAMFile), fs(fs), _size(size), _data(data)
	{
		this->_name = name;
	}

	uint64_t FileNode::read(uint64_t pos, uint64_t len, void* buf)
	{
		assert(buf);
		if (pos >= this->size())
		{
			return 0;
		}
		
		uint64_t adj_len = len;
		if (pos + len > size())
		{
			adj_len = size() - pos;
		}
		
		memcpy(buf, _data, adj_len);
		memset(((uint8_t*)buf) + adj_len, 0, len - adj_len);
		return len;
		
		// TODO
		ASSERT(false);
	}


	uint64_t FileNode::write(uint64_t, uint64_t, const void*)
	{
		// TODO
		ASSERT(false);
	}


	void FileNode::open()
	{
		// TODO
		ASSERT(false);
	}


	void FileNode::close()
	{
		// TODO
		ASSERT(false);
	}
	
	size_t FileNode::size() const noexcept
	{
		return _size;
	}
	
	Filesystem* FileNode::get_filesystem() const noexcept
	{
		return fs;
	}




}
}
}