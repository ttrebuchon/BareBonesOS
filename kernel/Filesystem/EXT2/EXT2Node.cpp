#include "EXT2Node.hh"

namespace Kernel::FS
{
	
	EXT2Node::EXT2Node(EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name) : fs(fs), node(node), inode_name(name)
	{
		
	}
	
	EXT2Node::EXT2Node(EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, fs->get_inode(ent->inode), (const char*)ent->name)
	{
		
	}


	
	uint64_t EXT2Node::node_size() const noexcept
	{
		assert(node);
		return fs->inode_size(node.get());
	}
	
	
	
	
	
	
	uint64_t EXT2Node::read(uint64_t start, uint64_t len, uint8_t* buf)
	{
		assert(buf);
		assert(start < node_size());
		assert(node_size() - start >= len);
		
		size_t nstart = start;
		size_t nend = start + len;
		if ((start % fs->block_size()) || (len % fs->block_size()))
		{
			nstart = start - (start % fs->block_size());
			
			nend = start + len;
			if (nend % fs->block_size())
			{
				nend += (fs->block_size() - (nend % fs->block_size()));
			}
			
		}
		
		auto ptr = fs->read_inode(node, nstart/fs->block_size(), nend/fs->block_size());
		assert(ptr);
		if (!ptr)
		{
			return 0;
		}
		
		memcpy(buf, ptr + (start - nstart), len);
		delete[] ptr;
		
		return len;
		
		assert(NOT_IMPLEMENTED);
	}
	
	uint64_t EXT2Node::write(uint64_t, uint64_t, const uint8_t*)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	void EXT2Node::open()
	{
		assert(NOT_IMPLEMENTED);
	}
	
	void EXT2Node::close()
	{
		assert(NOT_IMPLEMENTED);
	}
	
	
}