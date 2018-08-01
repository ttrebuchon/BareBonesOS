#include "LinkNode.hh"

namespace Kernel { namespace FS
{
	
	LinkNode::LinkNode(const NodeType t) : Node(t | NodeType::Link)
	{
		this->_type |= (t | NodeType::Link);
	}
	
	LinkNode::LinkNode() : LinkNode(NodeType::Unknown)
	{
		
	}
	
	
	
	NodeType LinkNode::type() const noexcept
	{
		auto t = target();
		assert(t != this);
		if (t)
		{
			return (this->_type = (t->type() | NodeType::Link));
		}
		return this->_type;
	}
	
	uint64_t LinkNode::read(uint64_t start, uint64_t len, uint8_t* buf)
	{
		auto t = target();
		assert(t);
		assert(t != this);
		if (!t)
		{
			// TODO: Handle error
			return 0;
		}
		return t->read(start, len, buf);
	}
	
	uint64_t LinkNode::write(uint64_t start, uint64_t len, const uint8_t* buf)
	{
		auto t = target();
		assert(t);
		assert(t != this);
		if (!t)
		{
			// TODO: Handle error
			return 0;
		}
		return t->write(start, len, buf);
	}
	
	void LinkNode::open()
	{
		auto t = target();
		assert(t);
		assert(t != this);
		if (!t)
		{
			// TODO: Handle error
			return;
		}
		t->open();
	}
	
	void LinkNode::close()
	{
		auto t = target();
		assert(t);
		assert(t != this);
		if (!t)
		{
			// TODO: Handle error
			return;
		}
		t->close();
	}
	
	
	
	
	
	
	
	const FileNode_v* LinkNode::as_file() const noexcept
	{
		auto t = target();
		assert(t != this);
		if (t)
		{
			return t->as_file();
		}
		return nullptr;
	}
	
	FileNode_v* LinkNode::as_file() noexcept
	{
		auto t = target();
		assert(t != this);
		if (t)
		{
			return t->as_file();
		}
		return nullptr;
	}
		
	const DirectoryNode_v* LinkNode::as_directory() const noexcept
	{
		auto t = target();
		assert(t != this);
		if (t)
		{
			return t->as_directory();
		}
		return nullptr;
	}
	
	DirectoryNode_v* LinkNode::as_directory() noexcept
	{
		auto t = target();
		assert(t != this);
		if (t)
		{
			return t->as_directory();
		}
		return nullptr;
	}
	
	const BlockDeviceNode* LinkNode::as_block_device() const noexcept
	{
		auto t = target();
		assert(t != this);
		if (t)
		{
			return t->as_block_device();
		}
		return nullptr;
	}
	
	BlockDeviceNode* LinkNode::as_block_device() noexcept
	{
		auto t = target();
		assert(t != this);
		if (t)
		{
			return t->as_block_device();
		}
		return nullptr;
	}
}
}