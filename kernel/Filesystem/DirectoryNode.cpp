#include "DirectoryNode.hh"
#include "Path.hh"
#include "Filesystem.hh"
#include "NodeFactory.hh"

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
	
	
	FileNode_v* DirectoryNode_v::add_file(const Utils::string& name)
	{
		auto fs = this->get_filesystem();
		if (fs)
		{
			auto& factory = fs->factory();
			return factory.create_file(this, name);
		}
		return nullptr;
	}
	
    DirectoryNode_v* DirectoryNode_v::add_directory(const Utils::string& name)
    {
    	auto fs = this->get_filesystem();
		if (fs)
		{
			auto& factory = fs->factory();
			return factory.create_directory(this, name);
		}
		return nullptr;
    }
	
	
	
	DirectoryNode::DirectoryNode(const NodeType t) : DirectoryNode_v(t)
	{
		
	}
	
	
	
	
}
}