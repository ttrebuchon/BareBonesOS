#include "DirectoryNode.hh"

namespace Kernel { namespace Filesystem
{
	
	DirectoryNode::DirectoryNode(const NodeType t) : Node(t | NodeType::Directory)
	{
		
	}
	
}
}