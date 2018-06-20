#include "FileNode.hh"

namespace Kernel { namespace FS { namespace Init_RD {
	


	FileNode::FileNode(const char* name, void* data, uint32_t size) noexcept : FS::FileNode(NodeType::RAMFile), _size(size), _data(data)
	{
		this->_name = name;
	}

	uint32_t FileNode::read(uint32_t pos, uint32_t len, uint8_t* buf)
	{
		assert(buf);
		if (pos >= this->size())
		{
			return 0;
		}
		
		uint32_t adj_len = len;
		if (pos + len > size())
		{
			adj_len = size() - pos;
		}
		
		memcpy(buf, _data, adj_len);
		memset(buf + adj_len, 0, len - adj_len);
		return len;
		
		// TODO
		ASSERT(false);
	}


	uint32_t FileNode::write(uint32_t, uint32_t, const uint8_t*)
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


	DirEnt* FileNode::readDir(uint32_t)
	{
		// TODO
		ASSERT(false);
	}


	Node* FileNode::findDir(const char* name)
	{
		// TODO
		ASSERT(false);
	}
	
	size_t FileNode::size() const noexcept
	{
		return _size;
	}




}
}
}