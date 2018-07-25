#include "EXT2Node.hh"

namespace Kernel::FS
{
	
	EXT2Node::EXT2Node(EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index) : fs(fs), node(node), inode_name(name), inode_index(inode_index)
	{
		assert(inode_index > 0);
	}
	
	EXT2Node::EXT2Node(EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, fs->get_inode(ent->inode), (const char*)ent->name, ent->inode)
	{
		
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
	
	uint64_t EXT2Node::node_size() const noexcept
	{
		assert(node);
		assert(fs);
		return fs->inode_size(node.get());
	}
	
	bool EXT2Node::add_block(size_t block_index, size_t* placement) noexcept
	{
		assert(node);
		for (size_t i = 0; i < 12; ++i)
		{
			if (node->direct[i] == 0)
			{
				node->direct[i] = block_index;
				if (placement)
				{
					*placement = i;
				}
				node->size_low += fs->block_size();
				return true;
			}
		}
		assert(NOT_IMPLEMENTED);
	}
	
	Utils::shared_ptr<detail::EXT2::block_group_t> EXT2Node::group() const noexcept
	{
		return fs->get_group(inode_index / fs->sb.nodes_per_group);
	}
	
	Utils::shared_ptr<detail::EXT2::block_t> EXT2Node::get_block(const unsigned int index, bool write) noexcept
	{
		if (index < 12)
		{
			auto block = node->direct[index];
			assert(block > 0);
			if (block == 0)
			{
				return nullptr;
			}
			return group()->get_block(block, write);
		}
		
		assert(NOT_IMPLEMENTED);
	}
	
	size_t EXT2Node::block_count() const noexcept
	{
		auto n = node_size();
		auto b = fs->block_size();
		return (n/b) + (n % b != 0);
	}
	
	size_t EXT2Node::unused_space() const noexcept
	{
		auto n = node_size();
		auto b = fs->block_size();
		if (n % b)
		{
			return b - (n % b);
		}
		else
		{
			return 0;
		}
	}
	
	
}