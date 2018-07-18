#include "DirectoryNode.hh"
#include "Path.hh"

namespace Kernel { namespace FS
{
	
	DirectoryNode_v::DirectoryNode_v(const NodeType t) : Node(t | NodeType::Directory)
	{
		this->_type |= (t | NodeType::Directory);
	}
	
	
	
	
	
	
	Node* DirectoryNode_v::findChild(const Utils::string& path)
	{
		ASSERT(!Path::IsAbsolute(path));
		
		Path_t p(path);
		
		auto next = this;
		Node* n = nullptr;
		size_t i = 0;
		while (next && i < p.parts_length())
		{
			auto& part = p.part(i);
			n = next->at(part);
			if (n)
			{
				next = n->as_directory();
			}
			else
			{
				next = nullptr;
			}
			++i;
		}
		
		if (i == p.parts_length())
		{
			return n;
		}
		else
		{
			return nullptr;
		}
		
		// TODO
		ASSERT(false);
	}
	
	Node* DirectoryNode_v::at(const Utils::string& str) const
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
	
	
	
	DirectoryNode::DirectoryNode(const NodeType t) : DirectoryNode_v(t)
	{
		
	}
	
}
}