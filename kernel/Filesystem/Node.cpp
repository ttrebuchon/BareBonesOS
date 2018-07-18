#include "Node.hh"

namespace Kernel { namespace FS {
	
	uint32_t Node::counter = 1;
	
	Node::Node(const NodeType t) : _type(t), parent(nullptr), _name(), inode(counter++), name(_name)
	{
		
	}
	
	Node::Node() : Node(NodeType::Unknown)
	{
		
	}
	
	
	
	NodeType Node::type() const noexcept
	{
		return _type;
	}
	
	void Node::set_parent(DirectoryNode_v* parent)
	{
		this->parent = parent;
	}
}
}