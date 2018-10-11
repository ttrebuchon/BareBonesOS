#include "NativeFile.hh"
#include <kernel/Filesystem/initrd.hh>

#ifdef TESTING

#include <fcntl.h>
#include <errno.h>

namespace Kernel { namespace FS { namespace Init_RD {
	


	NativeFileNode::NativeFileNode(InitRD_FS* fs, const Utils::string& name, const Utils::string& native) noexcept : FS::FileNode(NodeType::RAMFile), fs(fs), fd(-1)
	{
		this->_name = name;
		fd = ::open(native.c_str(), O_CREAT | O_RDWR);
		assert(fd >= 0);
	}
	
	NativeFileNode::~NativeFileNode()
	{
		::close(fd);
	}

	uint64_t NativeFileNode::read(uint64_t pos, uint64_t len, void* buf)
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
		
		lseek(fd, pos, SEEK_SET);
		int res = ::read(fd, buf, adj_len);
		if (res < adj_len)
		{
			return res;
		}
		memset(((uint8_t*)buf) + adj_len, 0, len - adj_len);
		return len;
	}


	uint64_t NativeFileNode::write(uint64_t pos, uint64_t len, const void* buf)
	{
		if (len)
		{
			assert(buf);
		}
		else
		{
			return 0;
		}
		
		lseek(fd, pos, SEEK_SET);
		auto res = ::write(fd, buf, len);
		if (res < 0)
		{
			TRACE_VAL(res);
			TRACE_VAL(fd);
			TRACE_VAL(strerror(errno));
		}
		assert(res >= 0);
		if (res < len)
		{
			TRACE_VAL(res);
		}
		return res;
	}


	void NativeFileNode::open()
	{
		// TODO
		ASSERT(false);
	}


	void NativeFileNode::close()
	{
		// TODO
		ASSERT(false);
	}
	
	size_t NativeFileNode::size() const noexcept
	{
		return ::lseek(fd, 0, SEEK_END);
	}
	
	Filesystem* NativeFileNode::get_filesystem() const noexcept
	{
		return fs;
	}
	
	
	
}
	node_ptr<FileNode_v> InitRD_FS::create_native_file(const Utils::string& spath, const Utils::string& native)
	{
		auto exist = getNode(spath);
		if (exist)
		{
			return nullptr;
		}
		
		Path_t path(spath);
		if (!path.parts_length())
		{
			return nullptr;
		}
		
		Utils::string name;
		node_ptr<DirectoryNode_v> parent = nullptr;
		if (path.parts_length() == 1)
		{
			assert(path.part(0) != "");
			parent = root().as_directory();
			name = path.part(0);
		}
		else
		{
			auto path2 = path.subpath(0, path.parts_length()-1);
			parent = getNode(path2).as_directory();
			name = path.back();
		}
		
		if (!parent)
		{
			return nullptr;
		}
		
		using namespace Init_RD;
		
		
		auto raw_node = new NativeFileNode(this, name, native);
		auto node = node_ptr<FileNode_v>(Utils::shared_ptr<NativeFileNode>(raw_node));
		parent->add(node);
		return node;
	}

}
}

#endif