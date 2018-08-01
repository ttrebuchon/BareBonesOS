#include "EXT2Node.hh"

#define DIVU(X, Y) (((X) / (Y)) + (((X) % (Y) == 0) ? 0 : 1))

namespace Kernel::FS
{
	
	EXT2Node::EXT2Node(EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index) : fs(fs), node(node), inode_name(name), inode_index(inode_index)
	{
		assert(inode_index > 0);
	}
	
	EXT2Node::EXT2Node(EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, fs->get_inode(ent->inode), (const char*)ent->name, ent->inode)
	{
		
	}
	
	uint64_t EXT2Node::write_blocks_to_node(EXT2* ext2, detail::EXT2::inode_t* node, uint64_t start, uint64_t len, const uint8_t* buf)
	{
		assert(ext2);
		assert(node);
		if (!ext2 || !node || !buf || len == 0)
		{
			return 0;
		}
		
		auto block_sz = ext2->block_size();
		assert(block_sz);
		
		//auto block_count = 
		//DIVU(start, block_sz) - DIVU(start+len, block_sz);
		
		auto first_block = start/block_sz;
		auto last = (start + len)/block_sz;
		TRACE_VAL(first_block);
		TRACE_VAL(last);
		TRACE_VAL(last - first_block + 1);
		
		auto blocks = get_file_blocks(ext2, node, first_block, last - first_block + 1);
		assert(blocks.size() <= (last - first_block + 1));
		TRACE_VAL(blocks.size());
		
		{
			size_t null_count = 0;
			for (auto block : blocks)
			{
				if (!block)
				{
					++null_count;
				}
			}
			
			TRACE_VAL(last - first_block + 1 - blocks.size());
			null_count += (last - first_block + 1 - blocks.size()); 
			TRACE_VAL(null_count);
			
			if (ext2->inode_size(node) < start + len)
			{
				/*if (expand_to(start + len) != (start + len))
				{
					return 0;
				}
				return write_blocks(ext2, node, start, len, buf);*/
				
				//null_count = (start+len)/block_sz - ext2->inode_size(node)/block_sz + 1;
			}
			
			
			#ifdef DEBUG
			size_t add_index, last_index;
			bool first = true;
			#endif
			for (size_t i = 0; i < null_count; ++i)
			{
				size_t bindex;
				auto blk = ext2->reserve_block(bindex);
				if (!blk)
				{
					return 0;
				}
				size_t expanded;
				if (!EXT2Node::add_block_to_node(ext2, node, bindex
				#ifdef DEBUG
				, &add_index
				#endif
				))
				{
					ext2->release_block(bindex);
					return 0;
				}
				#ifdef DEBUG
				if (likely(!first))
				{
					if (last_index+1 != add_index)
					{
						TRACE("\n\n\n");
						TRACE_VAL(last_index);
						TRACE_VAL(add_index);
					}
					
					assert(last_index+1 == add_index);
				}
				first = false;
				last_index = add_index;
				#endif
			}
			
			if (null_count)
			{
				return write_blocks_to_node(ext2, node, start, len, buf);
				blocks = get_file_blocks(ext2, node, first_block, last - first_block + 1);
			}
		}
		
		
		
		if (blocks.size() != last - first_block + 1)
		{
			TRACE_VAL(last - first_block + 1);
			TRACE_VAL(blocks.size());
			assert(false);
			return 0;
		}
		
		
		size_t block_indx = first_block;
		size_t block_off = start - (block_indx * block_sz);
		auto block_it = blocks.begin();
		
		if (block_off > 0)
		{
			memcpy((*block_it)->data + block_off, buf + block_off, (len > block_sz ? block_sz : len) - block_off);
			block_off += (len > block_sz ? block_sz : len) - block_off;
			(*block_it)->modified = true;
			
			if (block_off % block_sz == 0)
			{
				block_off = 0;
				block_indx += 1;
				++block_it;
			}
		}
		
		while (block_indx+1 < last)
		{
			
			memcpy((*block_it)->data, buf + (block_indx-first_block)*block_sz, block_sz);
			(*block_it)->modified = true;
			++block_indx;
			++block_it;
		}
		
		
		if ((len + start) % block_sz == 0)
		{
			memcpy((*block_it)->data, buf + (block_indx-first_block)*block_sz, block_sz);
			(*block_it)->modified = true;
			++block_indx;
			++block_it;
			
			if ((len + start) > node->size_low)
			{
				node->size_low = len + start;
			}
			
			return len;
		}
		else
		{
			
			block_off = (len + start) % block_sz;
			memcpy((*block_it)->data, buf + (block_indx*block_sz), block_off);
			(*block_it)->modified = true;
			
			if ((len + start) > node->size_low)
			{
				node->size_low = len + start;
			}
			
			return len;
		}
		
		
	}
	
	uint64_t EXT2Node::write_blocks(uint64_t start, uint64_t len, const uint8_t* buf)
	{
		assert(buf);
		if (unlikely(!buf))
		{
			return 0;
		}
		
		
		if (start + len > fs->inode_size(node.get()))
		{
			if (expand_to(start + len) != (start + len))
			{
				return 0;
			}
		}
		
		auto block_sz = fs->block_size();
		assert(block_sz);
		
		auto first = start/block_sz;
		auto last = (start + len)/block_sz;
		
		auto blocks = get_file_blocks(fs, node.get(), first, last - first + 1);
		assert(blocks.size() == (last - first + 1));
		
		
		size_t block_indx = first;
		size_t block_off = start - (block_indx * block_sz);
		auto block_it = blocks.begin();
		
		if (block_off > 0)
		{
			memcpy((*block_it)->data + block_off, buf + block_off, (len > block_sz ? block_sz : len) - block_off);
			block_off += (len > block_sz ? block_sz : len) - block_off;
			(*block_it)->modified = true;
			
			if (block_off % block_sz == 0)
			{
				block_off = 0;
				block_indx += 1;
				++block_it;
			}
		}
		
		while (block_indx+1 < last)
		{
			
			memcpy((*block_it)->data, buf + (block_indx-first)*block_sz, block_sz);
			(*block_it)->modified = true;
			++block_indx;
			++block_it;
		}
		
		
		if ((len + start) % block_sz == 0)
		{
			memcpy((*block_it)->data, buf + (block_indx-first)*block_sz, block_sz);
			(*block_it)->modified = true;
			++block_indx;
			++block_it;
			
			if ((len + start) > node->size_low)
			{
				node->size_low = len + start;
			}
			
			return len;
		}
		else
		{
			
			block_off = (len + start) % block_sz;
			memcpy((*block_it)->data, buf + (block_indx*block_sz), block_off);
			(*block_it)->modified = true;
			
			if ((len + start) > node->size_low)
			{
				node->size_low = len + start;
			}
			
			return len;
		}
	}
	
	Utils::list<Utils::shared_ptr<detail::EXT2::block_t>> EXT2Node::get_file_blocks(EXT2* ext2, const detail::EXT2::inode_t* node, size_t start, size_t count, bool ignore_zeros)
	{
		Utils::list<Utils::shared_ptr<detail::EXT2::block_t>> blocks;
		
		assert(start + count >= start);
		if (count == 0)
		{
			return blocks;
		}
		
		size_t j = 0;
		for (size_t i = start; i < count && i < 12; ++i, ++j)
		{
			if (node->direct[i])
			{
				blocks.push_back(ext2->get_block(node->direct[i]));
			}
			else if (!ignore_zeros)
			{
				blocks.push_back(nullptr);
			}
		}
		
		count -= j;
		if (start >= 12)
		{
			start -= 12;
		}
		else
		{
			start = 0;
		}
		
		if (count == 0)
		{
			return blocks;
		}
		
		const auto ptr_per_block = ext2->block_size()/sizeof(uint32_t);
		
		if (likely(start < ptr_per_block))
		{
			if (node->indirect_1)
			{
				j = ext2->expand_indirect_1(ext2->get_block(node->indirect_1), blocks, start, count, !ignore_zeros);
			}
			else if (!ignore_zeros)
			{
				j = 0;
				for (size_t k = start; k < ptr_per_block && k < count + start; ++k)
				{
					blocks.push_back(nullptr);
					++j;
				}
			}
			
			count -= j;
			start = 0;
		}
		else
		{
			start -= ptr_per_block;
		}
		
		if (likely(count == 0))
		{
			return blocks;
		}
		
		if (start < ptr_per_block*ptr_per_block)
		{
			if (node->indirect_2)
			{
				j = ext2->expand_indirect_2(ext2->get_block(node->indirect_2), blocks, start, count, !ignore_zeros);
			}
			else if (!ignore_zeros)
			{
				j = 0;
				for (size_t k = start; k < ptr_per_block*ptr_per_block && k < count + start; ++k)
				{
					blocks.push_back(nullptr);
					++j;
				}
			}
			count -= j;
			start = 0;
		}
		else
		{
			start -= ptr_per_block*ptr_per_block;
		}
		
		if (count == 0)
		{
			return blocks;
		}
		
		if (start < ptr_per_block*ptr_per_block*ptr_per_block)
		{
			if (node->indirect_3)
			{
				j = ext2->expand_indirect_3(ext2->get_block(node->indirect_3), blocks, start, count, !ignore_zeros);
			}
			else if (!ignore_zeros)
			{
				j = 0;
				for (size_t k = start; k < ptr_per_block*ptr_per_block*ptr_per_block && k < count + start; ++k)
				{
					blocks.push_back(nullptr);
					++j;
				}
			}
			count -= j;
			start = 0;
		}
		else
		{
			start -= ptr_per_block*ptr_per_block*ptr_per_block;
		}
		
		return blocks;
		
		
		
		
		
		if ((start + count > 12) && (start + count < 12 + ext2->block_size()/sizeof(uint32_t)))
		{
			ext2->expand_indirect_1(ext2->get_block(node->indirect_1), blocks, count);
			blocks.clear();
		}
		
		size_t i;
		for (i = start; i < 12 && i < start+count; ++i)
		{
			if (node->direct[i])
			{
				blocks.push_back(ext2->get_block(node->direct[i]));
			}
			else if (!ignore_zeros)
			{
				blocks.push_back(nullptr);
			}
		}
		
		if (blocks.size() == count || (ignore_zeros && (i >= start+count)))
		{
			return blocks;
		}
		
		
		
		
		if (start+count >= 12 && start < 12+ptr_per_block)
		{
			if (node->indirect_1)
			{
				auto indirect = ext2->get_block(node->indirect_1);
				const uint32_t* ptrs = (const uint32_t*)indirect->data;
				
				
				for (; i < count+start && i < ptr_per_block+12; ++i)
				{
					blocks.push_back(ext2->get_block(ptrs[i-12]));
				}
			}
			else if (!ignore_zeros)
			{
				for (; i < count+start && i < ptr_per_block+12; ++i)
				{
					blocks.push_back(nullptr);
				}
			}
			else if (i < 12 + ptr_per_block)
			{
				i = 12 + ptr_per_block;
			}
		}
		
		if (blocks.size() == count || (ignore_zeros && (i >= start+count)))
		{
			return blocks;
		}
		
		assert(NOT_IMPLEMENTED);
	}
	
	bool EXT2Node::add_block_to_node(EXT2* ext2, detail::EXT2::inode_t* node, size_t block_index, size_t* placement) noexcept
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
				//node->size_low += ext2->block_size();
				return true;
			}
		}
		
		if (node->indirect_1 == 0)
		{
			size_t bindex;
			auto blk = ext2->reserve_block(bindex);
			if (!blk)
			{
				return false;
			}
			node->indirect_1 = bindex;
		}
		
		auto indirect_1 = ext2->get_block(node->indirect_1);
		assert(indirect_1);
		if (!indirect_1)
		{
			return false;
		}
		
		const auto pperblk = ext2->block_size()/sizeof(uint32_t);
		
		uint32_t* ptrs = (uint32_t*)indirect_1->data;
		
		for (size_t i = 0; i < pperblk; ++i)
		{
			if (ptrs[i] == 0)
			{
				ptrs[i] = block_index;
				indirect_1->modified = true;
				if (placement)
				{
					*placement = 12 + i;
				}
				//node->size_low += ext2->block_size();
				return true;
			}
		}
		
		
		if (node->indirect_2 == 0)
		{
			size_t bindex;
			auto blk = ext2->reserve_block(bindex);
			if (!blk)
			{
				return false;
			}
			node->indirect_2 = bindex;
		}
		
		auto indirect_2 = ext2->get_block(node->indirect_2);
		assert(indirect_2);
		if (!indirect_2)
		{
			return false;
		}
		
		auto ptrs2 = (uint32_t*)indirect_2->data;
		
		
		for (size_t j = 0; j < pperblk; ++j)
		{
			if (ptrs2[j] == 0)
			{
				size_t bindex;
				auto blk = ext2->reserve_block(bindex);
				if (!blk)
				{
					return false;
				}
				ptrs2[j] = bindex;
				indirect_2->modified = true;
			}
			indirect_1 = ext2->get_block(ptrs2[j]);
			ptrs = (uint32_t*)indirect_1->data;
			
			for (size_t i = 0; i < pperblk; ++i)
			{
				if (ptrs[i] == 0)
				{
					ptrs[i] = block_index;
					indirect_1->modified = true;
					if (placement)
					{
						*placement = 12 + pperblk + j*pperblk + i;
					}
					//node->size_low += ext2->block_size();
					return true;
				}
			}
		}
		
		
		
		if (node->indirect_3 == 0)
		{
			size_t bindex;
			auto blk = ext2->reserve_block(bindex);
			if (!blk)
			{
				return false;
			}
			node->indirect_3 = bindex;
		}
		
		auto indirect_3 = ext2->get_block(node->indirect_3);
		assert(indirect_3);
		if (!indirect_3)
		{
			return false;
		}
		
		auto ptrs3 = (uint32_t*)indirect_3->data;
		
		
		for (size_t k = 0; k < pperblk; ++k)
		{
			if (ptrs3[k] == 0)
			{
				size_t bindex;
				auto blk = ext2->reserve_block(bindex);
				if (!blk)
				{
					return false;
				}
				ptrs3[k] = bindex;
				indirect_3->modified = true;
			}
			indirect_2 = ext2->get_block(ptrs3[k]);
			ptrs2 = (uint32_t*)indirect_2->data;
			for (size_t j = 0; j < pperblk; ++j)
			{
				if (ptrs2[j] == 0)
				{
					size_t bindex;
					auto blk = ext2->reserve_block(bindex);
					if (!blk)
					{
						return false;
					}
					ptrs2[j] = bindex;
					indirect_2->modified = true;
				}
				indirect_1 = ext2->get_block(ptrs2[j]);
				ptrs = (uint32_t*)indirect_1->data;
				
				for (size_t i = 0; i < pperblk; ++i)
				{
					if (ptrs[i] == 0)
					{
						ptrs[i] = block_index;
						indirect_1->modified = true;
						if (placement)
						{
							*placement = 12 + pperblk + pperblk*pperblk + i + j*pperblk + k*pperblk*pperblk;
						}
						//node->size_low += ext2->block_size();
						return true;
					}
				}
			}
		}
		
		assert(false);
		return false;
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
	
	uint64_t EXT2Node::write(uint64_t start, uint64_t len, const uint8_t* buf)
	{
		return write_blocks(start, len, buf);
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
		return add_block_to_node(fs, node.get(), block_index, placement);
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
	
	
	size_t EXT2Node::expand_to(const size_t sz)
	{
		const auto previous = fs->inode_size(node.get());
		assert(sz >= previous);
		if (sz < previous)
		{
			return previous;
		}
		
		const auto bsize = fs->block_size();
		auto blks = DIVU(previous, bsize);
		
		if (sz <= blks*bsize)
		{
			node->size_low = sz;
			return sz;
		}
		
		node->size_low = blks*bsize;
		
		Utils::list<size_t> indexes;
		
		__try
		{
		while (indexes.size() + blks < DIVU(sz, bsize))
		{
			size_t bindex;
			if (!fs->reserve_block(bindex))
			{
				goto revert;
			}
			else
			{
				indexes.push_back(bindex);
			}
		}
		}
		__catch(...)
		{
			goto revert;
		}
		
		for (auto it = indexes.begin(); it != indexes.end();)
		{
			if (!add_block(*it))
			{
				TRACE("Couldn't add block");
				goto revert;
			}
			else
			{
				it = indexes.erase(it);
				node->size_low += bsize;
			}
		}
		
		assert(node->size_low >= sz);
		node->size_low = sz;
		return node->size_low;
		
		
		revert:
		
		for (auto it = indexes.begin(); it != indexes.end(); ++it)
		{
			fs->release_block(*it);
		}
		
		return node->size_low;
		
		assert(NOT_IMPLEMENTED);
	}
	
}