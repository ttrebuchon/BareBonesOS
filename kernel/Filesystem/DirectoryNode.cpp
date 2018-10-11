#include "DirectoryNode.hh"
#include "Path.hh"
#include "Filesystem.hh"
#include "NodeFactory.hh"
#include <kernel/Filesystem.hh>

namespace Kernel { namespace FS
{
	
	DirectoryNode_v::DirectoryNode_v(const NodeType t) : Node(t | NodeType::Directory)
	{
		this->_type |= (t | NodeType::Directory);
	}
	
	
	
	
	
	
	/*Node* DirectoryNode_v::findChild(const Utils::string& path)
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
	}*/
	
	node_ptr<> DirectoryNode_v::findChild(const Utils::string& path)
	{
		ASSERT(!Path::IsAbsolute(path));
		
		Path_t p(path);
		
		node_ptr<> n = nullptr;
		size_t i = 0;
		node_ptr<DirectoryNode_v> next;
		if (p.parts_length() > 0)
		{
			n = this->at(p.part(0));
			next = n.as_directory();
			++i;
		}
		
		
		while (next && i < p.parts_length())
		{
			auto& part = p.part(i);
			n = next->at(part);
			if (n)
			{
				next = n.as_directory();
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
	
	DirEnt* DirectoryNode_v::readDir(uint32_t)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	node_ptr<> DirectoryNode_v::findDir(const char*)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	/*Node* DirectoryNode_v::at(const Utils::string& str) const
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
	}*/
	
	node_ptr<> DirectoryNode_v::at(const Utils::string& str) const
	{
		node_ptr n = nullptr;
		for (size_t i = 0, s = size(); i < s; ++i)
		{
			n = at(i);
			assert(n);
			if (n->name == str)
			{
				return n;
			}
		}
		return nullptr;
	}
	
	
	/*FileNode_v* DirectoryNode_v::add_file(const Utils::string& name)
	{
		assert(this->at(name) == nullptr);
    	if (this->at(name))
    	{
    		return nullptr;
    	}
		auto fs = this->get_filesystem();
		if (fs)
		{
			auto& factory = fs->factory();
			return factory.create_file(this, name);
		}
		return nullptr;
	}*/
	
	node_ptr<FileNode_v> DirectoryNode_v::add_file(const Utils::string& name)
	{
		assert(this->at(name) == nullptr);
    	if (this->at(name))
    	{
    		return nullptr;
    	}
		auto fs = this->get_filesystem();
		if (fs)
		{
			auto& factory = fs->factory();
			auto n = factory.create_file(this, name);
			if (n)
			{
				n->set_parent(this);
				this->add(n);
			}
			return n;
		}
		return nullptr;
	}
	
    node_ptr<DirectoryNode_v> DirectoryNode_v::add_directory(const Utils::string& name)
    {
    	assert(this->at(name) == nullptr);
    	if (this->at(name))
    	{
    		return nullptr;
    	}
    	auto fs = this->get_filesystem();
		if (fs)
		{
			auto& factory = fs->factory();
			auto n = factory.create_directory(this, name);
			if (n)
			{
				n->set_parent(this);
				this->add(n);
			}
			return n;
		}
		return nullptr;
    }
    
    node_ptr<BlockDeviceNode> DirectoryNode_v::add_block_device(const Utils::string& name, const Utils::shared_ptr<DeviceTarget>& dev)
    {
    	assert(this->at(name) == nullptr);
    	if (this->at(name))
    	{
    		return nullptr;
    	}
    	auto fs = get_filesystem();
    	if (fs)
    	{
    		auto& factory = fs->factory();
			auto n = factory.create_block_device(this, name, dev);
			if (n)
			{
				n->set_parent(this);
				this->add(n);
			}
			return n;
    	}
    	return nullptr;
    }
    
    node_ptr<CharDeviceNode> DirectoryNode_v::add_char_device(const Utils::string& name, const Utils::shared_ptr<DeviceTarget>& dev)
    {
    	assert(this->at(name) == nullptr);
    	if (this->at(name))
    	{
    		return nullptr;
    	}
    	auto fs = get_filesystem();
    	if (fs)
    	{
    		auto& factory = fs->factory();
    		auto n = factory.create_char_device(this, name, dev);
			if (n)
			{
				n->set_parent(this);
				this->add(n);
			}
			return n;
    	}
    	return nullptr;
    }
    
    node_ptr<LinkNode> DirectoryNode_v::add_link(const Utils::string& name, const node_ptr<const Node>& target)
    {
    	assert(this->at(name) == nullptr);
    	if (this->at(name))
    	{
    		return nullptr;
    	}
    	if (target)
    	{
    		auto fs = get_filesystem();
    		if (fs)
    		{
    			auto n = fs->factory().create_link(this, name, target);
    			if (n)
    			{
    				n->set_parent(this);
    				this->add(n);
    			}
    			return n;
    		}
    	}
    	
    	assert(NOT_IMPLEMENTED);
    }
    
    node_ptr<LinkNode> DirectoryNode_v::add_link(const Utils::string& name, const Path_t& target)
    {
    	return add_link(name, target.str());
    }
    
    node_ptr<LinkNode> DirectoryNode_v::add_link(const Utils::string& name, const Utils::string& target)
    {
    	assert(this->at(name) == nullptr);
    	if (this->at(name))
    	{
    		return nullptr;
    	}
    	auto fs = get_filesystem();
    	if (fs)
    	{
    		auto n = fs->factory().create_link(this, name, target);
			if (n)
			{
				n->set_parent(this);
				this->add(n);
			}
			return n;
    	}
    	
    	return nullptr;
    }
    
    bool DirectoryNode_v::has_child(const node_ptr<>& n) const
    {
    	assert(n);
    	if (unlikely(!n))
    	{
    		return false;
    	}
    	
    	for (size_t i = 0; i < size(); ++i)
    	{
    		if (this->at(i) == n)
    		{
    			return true;
    		}
    	}
    	
    	return false;
    }
    
    bool DirectoryNode_v::remove(const Utils::string& name)
    {
    	auto n = this->at(name);
    	if (n)
    	{
    		return this->remove(n);
    	}
    	else
    	{
    		return false;
    	}
    }
    
    bool DirectoryNode_v::remove(size_t i)
    {
    	auto n = this->at(i);
    	if (n)
    	{
    		return this->remove(n);
    	}
    	else
    	{
    		return false;
    	}
    }
	
	node_ptr<> DirectoryNode_v::find_node(const Path_t& path)
	{
		if (path.empty())
		{
			return nullptr;
		}
		
		node_ptr<DirectoryNode_v> it = nullptr;
		size_t i = 0;
		if (path.parts_length() - 1 > 0)
		{
			auto next = this->at(path.part(0));
			if (next)
			{
				it = next.as_directory();
			}
			else
			{
				return nullptr;
			}
			++i;
		}
		
		for (; i < path.parts_length() - 1 && it; ++i)
		{
			auto next = it->at(path.part(i));
			if (next)
			{
				it = next.as_directory();
			}
			else
			{
				return nullptr;
			}
		}
		
		if (it)
		{
			return it->at(path.back());
		}
		
		return nullptr;
	}
	
	
	
	
	DirectoryNode::DirectoryNode(const NodeType t) : DirectoryNode_v(t)
	{
		
	}
	
	
	
	
}
}