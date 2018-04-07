#include "Node.hh"

namespace Kernel { namespace Filesystem {
	
	uint32_t Node::counter = 1;
	
	Node::Node(const NodeType t) : _type(t), parent(nullptr), _name(), inode(counter++), name(_name), type(_type)
	{
		
	}
}
}