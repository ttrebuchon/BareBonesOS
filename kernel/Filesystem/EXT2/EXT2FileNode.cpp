#include "EXT2FileNode.hh"

namespace Kernel::FS
{
	
	
	EXT2FileNode::EXT2FileNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index) : EXT2Node(fs, node, name, inode_index), Node(), FileNode()
	{
		init(parent);
	}
	EXT2FileNode::EXT2FileNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, ent), Node(), FileNode()
	{
		init(parent);
	}
	
	
	void EXT2FileNode::init(DirectoryNode_v* parent)
	{
		this->_name = this->EXT2Node::inode_name;
		this->parent = parent;
	}
	
	
	
	
	
	
	size_t EXT2FileNode::size() const noexcept
	{
		return this->node_size();
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	
	uint64_t EXT2FileNode::read(uint64_t start, uint64_t len, uint8_t* buf)
	{
		return EXT2Node::read(start, len, buf);
	}
	
	uint64_t EXT2FileNode::write(uint64_t start, uint64_t len, const uint8_t* buf)
	{
		return EXT2Node::write(start, len, buf);
	}
	
	void EXT2FileNode::open()
	{
		EXT2Node::open();
	}
	
	void EXT2FileNode::close()
	{
		EXT2Node::close();
	}
	
	DirEnt* EXT2FileNode::readDir(uint32_t)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	Node* EXT2FileNode::findDir(const char* name)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	EXT2* EXT2FileNode::get_filesystem() const noexcept
	{
		return this->EXT2Node::fs;
	}
	
}