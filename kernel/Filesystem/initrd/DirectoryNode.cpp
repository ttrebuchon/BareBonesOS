#include "DirectoryNode.hh"
#include <kernel/Filesystem/initrd.hh>

namespace Kernel { namespace FS { namespace Init_RD {


	DirectoryNode::DirectoryNode(InitRD_FS* fs, const char* name) : FS::DirectoryNode(NodeType::RAMFile), fs(fs)
	{
		this->_name = name;
	}
	
	DirectoryNode::~DirectoryNode()
	{
		
	}




	uint64_t DirectoryNode::read(uint64_t, uint64_t, void*)
	{
		// TODO
		ASSERT(false);
	}


	uint64_t DirectoryNode::write(uint64_t, uint64_t, const void*)
	{
		// TODO
		ASSERT(false);
	}


	void DirectoryNode::open()
	{
		// TODO
		ASSERT(false);
	}


	void DirectoryNode::close()
	{
		// TODO
		ASSERT(false);
	}


	DirEnt* DirectoryNode::readDir(uint32_t)
	{
		// TODO
		ASSERT(false);
	}


	node_ptr<> DirectoryNode::findDir(const char* name)
	{
		// TODO
		ASSERT(false);
	}


	node_ptr<> DirectoryNode::add(const node_ptr<>& child)
	{
		if (child)
		{
			children.push_back(child);
			if (child->get_filesystem() == this->get_filesystem())
			{
				auto rn = dynamic_cast<RefNode*>(child.get());
				assert(rn);
				rn->new_ref();
			}
		}
		return child;
	}
	
	bool DirectoryNode::remove(const node_ptr<>& child)
	{
		if (unlikely(!child))
		{
			return false;
		}
		
		auto it = children.begin();
		for (; it != children.end(); ++it)
		{
			if (*it == child)
			{
				break;
			}
		}
		
		if (unlikely(it == children.end()))
		{
			return false;
		}
		
		children.erase(it);
		
		if (child->get_filesystem() == this->get_filesystem())
		{
			auto rn = dynamic_cast<RefNode*>(child.get());
			assert(rn);
			if (rn->remove_ref())
			{
				//delete child;
			}
		}
		else
		{
			assert(NOT_IMPLEMENTED);
		}
		
		return true;
	}
	
	Filesystem* DirectoryNode::get_filesystem() const noexcept
	{
		return fs;
	}



}
}
}