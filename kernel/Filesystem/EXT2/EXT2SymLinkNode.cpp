#include "EXT2SymLinkNode.hh"

namespace Kernel::FS
{
	
	
	
	
	/*EXT2DirectorySymLinkNode::EXT2DirectorySymLinkNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name) : EXT2Node(fs, node, name), DirectoryNode_v()
	{
		init(parent);
	}
	
	EXT2DirectorySymLinkNode::EXT2DirectorySymLinkNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, ent), DirectoryNode_v()
	{
		init(parent);
	}
	
	void EXT2DirectorySymLinkNode::init(DirectoryNode_v* parent)
	{
		this->_name = this->EXT2Node::inode_name;
		this->parent = parent;
	}
	
	
	
	
	void EXT2DirectorySymLinkNode::addChild(Node*)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	size_t EXT2DirectorySymLinkNode::size() const noexcept
	{
		assert(target());
		return target()->size();
	}
	
	Node* EXT2DirectorySymLinkNode::at(size_t index) const
	{
		assert(target());
		return target()->at(index);
	}
	
		
	uint32_t EXT2DirectorySymLinkNode::read(uint32_t start, uint32_t len, uint8_t* buf)
	{
		assert(target());
		return target()->read(start, len, buf);
	}
	
	uint32_t EXT2DirectorySymLinkNode::write(uint32_t start, uint32_t len, const uint8_t* buf)
	{
		assert(target());
		return target()->write(start, len, buf);
	}
	
	void EXT2DirectorySymLinkNode::open()
	{
		assert(target());
		target()->open();
	}
	
	void EXT2DirectorySymLinkNode::close()
	{
		assert(target());
		target()->close();
	}
	
	DirEnt* EXT2DirectorySymLinkNode::readDir(uint32_t n)
	{
		assert(target());
		return target()->readDir(n);
	}
	
	Node* EXT2DirectorySymLinkNode::findDir(const char* name)
	{
		assert(target());
		return target()->findDir(name);
	}
	
	
	DirectoryNode_v* EXT2DirectorySymLinkNode::target() const noexcept
	{
		assert(get_target());
		return get_target()->as_directory();
	}
	
	
	
	
	
	
	
	EXT2FileSymLinkNode::EXT2FileSymLinkNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name) : EXT2Node(fs, node, name), FileNode_v()
	{
		init(parent);
	}
	
	EXT2FileSymLinkNode::EXT2FileSymLinkNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, ent), FileNode_v()
	{
		init(parent);
	}
	
	void EXT2FileSymLinkNode::init(DirectoryNode_v* parent)
	{
		this->_name = this->EXT2Node::inode_name;
		this->parent = parent;
	}
	
	
	size_t EXT2FileSymLinkNode::size() const noexcept
	{
		assert(target());
		return target()->size();
	}
	
	
	FileNode_v* EXT2FileSymLinkNode::target() const noexcept
	{
		assert(get_target());
		return get_target()->as_file();
	}
	
	*/
	
	
	
	
	
	EXT2SymLinkNode::EXT2SymLinkNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name) : EXT2Node(fs, node, name), LinkNode(), _target(nullptr)
	{
		init(parent);
	}
	
	EXT2SymLinkNode::EXT2SymLinkNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, ent), LinkNode(), _target(nullptr)
	{
		init(parent);
	}
	
	
	
	void EXT2SymLinkNode::init(DirectoryNode_v* parent) noexcept
	{
		if (parent)
		{
			this->parent = parent;
		}
		
		this->_name = this->EXT2Node::inode_name;
	}
	
	
	
	
	
	
	
	/*NodeType EXT2SymLinkNode::type() const noexcept
	{
		auto t = target();
		if (t)
		{
			return (this->_type = (t->type() | NodeType::Link));
		}
		return this->_type;
	}*/
	
	
	const Utils::string EXT2SymLinkNode::target_path() const
	{
		if (node->size_low <= 60)
		{
			return node->data;
		}
		else
		{
			assert(NOT_IMPLEMENTED);
		}
	}
	
	Node* EXT2SymLinkNode::target() const noexcept
	{
		if (!_target)
		{
			_target = fs->getNode(target_path());
		}
		return _target;
	}
}