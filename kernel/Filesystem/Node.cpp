#include "Node.hh"
#include "DirectoryNode.hh"

namespace Kernel { namespace FS {
	
	uint32_t Node::counter = 1;
	
	Node::Node(const NodeType t) : _type(t), _parent(nullptr), _name(), inode(counter++), name(_name)
	{
		
	}
	
	Node::Node() : Node(NodeType::Unknown)
	{
		
	}
	
	Node::~Node()
	{
		
	}
	
	
	
	NodeType Node::type() const noexcept
	{
		return _type;
	}
	
	void Node::set_parent(DirectoryNode_v* parent)
	{
		this->_parent = parent;
	}
	
	Path_t Node::get_path() const
	{
		auto parent = get_parent();
		Path_t p;
		assert(parent != this);
		if (parent && parent != this)
		{
			p = parent->get_path();
		}
		p.append(name);
		return p;
	}
}
}