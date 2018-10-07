#include "FunctionNode.hh"

namespace Kernel { namespace FS
{
	
	FunctionNode::FunctionNode(const NodeType t) : Node(t | NodeType::Function)
	{
		this->_type |= (t | NodeType::Function);
	}
}
}