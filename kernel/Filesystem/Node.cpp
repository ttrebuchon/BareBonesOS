#include "Node.hh"
#include "DirectoryNode.hh"

namespace Kernel { namespace FS {
	
	uint32_t Node::counter = 1;
	
	Node::Node(const NodeType t) : _type(t), parent(nullptr), _name(), inode(counter++), name(_name)
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
		this->parent = parent;
	}
	
	Path_t Node::get_path() const
	{
		Path_t p;
		if (parent && parent != this)
		{
			p = parent->get_path();
		}
		p.append(name);
		return p;
	}
}
}