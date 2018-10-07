#include "FunctionNode.hh"
#include <kernel/Filesystem/initrd.hh>

namespace Kernel { namespace FS { namespace Init_RD {
	


	FunctionNode::FunctionNode(InitRD_FS* fs, const char* name, void* data, uint64_t size) noexcept : FS::FunctionNode(NodeType::RAMFile), fs(fs)
	{
		this->_name = name;
		assert(data);
		memcpy(fn_name, data, size);
	}

	uint64_t FunctionNode::read(uint64_t pos, uint64_t len, void* buf)
	{
		assert(buf);
		if (pos >= this->size())
		{
			return 0;
		}
		
		uint64_t adj_len = len;
		if (pos + len > size())
		{
			adj_len = size() - pos;
		}
		
		memcpy(buf, fn_name, adj_len);
		memset(((uint8_t*)buf) + adj_len, 0, len - adj_len);
		return len;
		
		// TODO
		ASSERT(false);
	}


	uint64_t FunctionNode::write(uint64_t, uint64_t, const void*)
	{
		// TODO
		ASSERT(false);
	}


	void FunctionNode::open()
	{
		// TODO
		ASSERT(false);
	}


	void FunctionNode::close()
	{
		// TODO
		ASSERT(false);
	}
	
	const char* FunctionNode::function() const noexcept
	{
		return fn_name;
	}
	
	size_t FunctionNode::size() const noexcept
	{
		return function_name_max;
	}
	
	Filesystem* FunctionNode::get_filesystem() const noexcept
	{
		return fs;
	}




}
}
}