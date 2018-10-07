#include "RAMFile.hh"
#include <kernel/Filesystem/initrd.hh>

namespace Kernel { namespace FS { namespace Init_RD {
	


	RAMFileNode::RAMFileNode(InitRD_FS* fs, const char* name) noexcept : FS::FileNode(NodeType::RAMFile), fs(fs), _size(0), _data(nullptr)
	{
		this->_name = name;
	}
	
	RAMFileNode::~RAMFileNode()
	{
		if (_data)
		{
			delete[] _data;
			_data = nullptr;
			_size = 0;
		}
	}
	
	void RAMFileNode::resize(uint64_t nSize)
	{
		if (!_data)
		{
			_data = new uint8_t[nSize];
			memset(_data, 0, nSize);
			_size = nSize;
		}
		else if (nSize > size())
		{
			auto nData = new uint8_t[nSize];
			memset(nData, 0, nSize);
			memcpy(nData, _data, size());
			delete[] _data;
			_data = nData;
			_size = nSize;
		}
		else if (nSize < size())
		{
			// TODO
			assert(NOT_IMPLEMENTED);
		}
	}

	uint64_t RAMFileNode::read(uint64_t pos, uint64_t len, void* buf)
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
		
		memcpy(buf, _data + pos, adj_len);
		memset(((uint8_t*)buf) + adj_len, 0, len - adj_len);
		return len;
		
		// TODO
		ASSERT(false);
	}


	uint64_t RAMFileNode::write(uint64_t pos, uint64_t len, const void* buf)
	{
		if (len)
		{
			assert(buf);
		}
		else
		{
			return 0;
		}
		
		if (pos + len > this->size())
		{
			resize(pos + len);
		}
		
		memcpy(_data + pos, buf, len);
		return len;
	}


	void RAMFileNode::open()
	{
		// TODO
		ASSERT(false);
	}


	void RAMFileNode::close()
	{
		// TODO
		ASSERT(false);
	}
	
	size_t RAMFileNode::size() const noexcept
	{
		return _size;
	}
	
	Filesystem* RAMFileNode::get_filesystem() const noexcept
	{
		return fs;
	}




}
}
}