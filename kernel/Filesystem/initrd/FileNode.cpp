#include "FileNode.hh"

namespace Kernel { namespace Filesystem { namespace Init_RD {
	


	FileNode::FileNode(const char* name, void* data, uint32_t size) noexcept : Filesystem::FileNode(NodeType::RAMFile), _size(size), _data(data)
	{
		this->_name = name;
	}

	uint32_t FileNode::read(uint32_t, uint32_t, uint8_t*)
	{
		// TODO
		assert(false);
	}


	uint32_t FileNode::write(uint32_t, uint32_t, const uint8_t*)
	{
		// TODO
		assert(false);
	}


	void FileNode::open()
	{
		// TODO
		assert(false);
	}


	void FileNode::close()
	{
		// TODO
		assert(false);
	}


	DirEnt* FileNode::readDir(uint32_t)
	{
		// TODO
		assert(false);
	}


	Node* FileNode::findDir(const char* name)
	{
		// TODO
		assert(false);
	}
	
	size_t FileNode::size() const noexcept
	{
		return _size;
	}




}
}
}