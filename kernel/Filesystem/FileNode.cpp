#include "FileNode.hh"

namespace Kernel { namespace Filesystem
{
	
	FileNode::FileNode(const NodeType t) : Node(t | NodeType::File)
	{
		
	}
	
}
}