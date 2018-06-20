#include "DirectoryNode.hh"
#include "Path.hh"

namespace Kernel { namespace Filesystem
{
	
	DirectoryNode::DirectoryNode(const NodeType t) : Node(t | NodeType::Directory)
	{
		
	}
	
	
	
	
	
	
	Node* DirectoryNode::findChild(const Utils::string& path)
	{
		assert(!Path::IsAbsolute(path));
		
		// TODO
		assert(false);
	}
	
	Node* DirectoryNode::at(const Utils::string& str) const
	{
		for (size_t i = 0, s = size(); i < s; ++i)
		{
			Node* n = nullptr;
			n = at(i);
			assert(n);
			if (n->name == str)
			{
				return n;
			}
		}
		return nullptr;
	}
	
}
}