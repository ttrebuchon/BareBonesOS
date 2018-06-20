#include "DirectoryNode.hh"
#include "Path.hh"

namespace Kernel { namespace FS
{
	
	DirectoryNode::DirectoryNode(const NodeType t) : Node(t | NodeType::Directory)
	{
		
	}
	
	
	
	
	
	
	Node* DirectoryNode::findChild(const Utils::string& path)
	{
		ASSERT(!Path::IsAbsolute(path));
		
		// TODO
		ASSERT(false);
	}
	
	Node* DirectoryNode::at(const Utils::string& str) const
	{
		for (size_t i = 0, s = size(); i < s; ++i)
		{
			Node* n = nullptr;
			n = at(i);
			ASSERT(n);
			if (n->name == str)
			{
				return n;
			}
		}
		return nullptr;
	}
	
}
}