#include "Node.hh"
#include "DirectoryNode.hh"
#include <Utils/weak_ptr>

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
	
	node_ptr<> Node::node_ptr_from_this()
	{
		if (this)
		{
			auto sptr = this->weak_from_this().lock();
			if (sptr)
			{
				return node_ptr<>(sptr);
			}
		}
		return nullptr;
	}
	
	node_ptr<const Node> Node::node_ptr_from_this() const
	{
		if (this)
		{
			auto sptr = this->weak_from_this().lock();
			if (sptr)
			{
				return node_ptr<const Node>(sptr);
			}
		}
		return nullptr;
	}
}
}