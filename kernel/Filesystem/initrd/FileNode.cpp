#include "FileNode.hh"

namespace Kernel { namespace Filesystem { namespace Init_RD {
	


	FileNode::FileNode(const char* name, void* data, uint32_t size) noexcept : Filesystem::FileNode(NodeType::RAMFile), _size(size), _data(data)
	{
		this->_name = name;
	}

	uint32_t FileNode::read(uint32_t, uint32_t, uint8_t*)
	{
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