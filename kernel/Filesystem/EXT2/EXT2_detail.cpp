#include "EXT2.hh"
#include <Utils/mutex>


namespace Kernel::FS::detail::EXT2
{
	block_t::block_t() : group(nullptr), modified(false), data()
	{
		
	}
	
	block_t::~block_t()
	{
		assert(group);
		if (group)
		{
			__try
			{
				uint16_t npos15 = (uint16_t)block_group_t::npos;
				npos15 >>= 1;
				assert(npos15 != index);
				if (npos15 != index && ((modified) || group->block_is_modified(index)))
				{
					group->commit_block(this);
				}
			}
			__catch(...)
			{
				
			}
		}
	}
	
	
	
	
	
	
	block_group_t::block_group_t(FS::EXT2& fs, block_group_desc_t& gd, const uint32_t group) : 
			group_index(group),
			mut(),
			cache_m(),
			fs(&fs),
			gd(&gd),
			block_usage_blk(nullptr),
			block_usage(nullptr, 0),
			node_usage_blk(nullptr),
			node_usage(nullptr, 0),
			modified_map(),
			blocks_per_group(fs.sb.blocks_per_group),
			nodes_per_group(fs.sb.nodes_per_group),
			cached_blocks(),
			nodes(),
			block_ptr_cache(),
			initted(false)
	{
		
	}
	
	block_group_t::~block_group_t()
	{
		cached_blocks.clear();
		block_ptr_cache.reset();
		node_usage.bits = nullptr;
		node_usage_blk = nullptr;
		
		block_usage_blk = nullptr;
		block_usage.bits = nullptr;
	}
	
	
	void block_group_t::init()
	{
		write_lock lock(mut);
		__sync_synchronize();
		if (initted)
		{
			return;
		}
		
		assert(modified_map.size() == 0);
		
		size_t bpg = fs->sb.blocks_per_group;
		assert(bpg > 0);
		
		size_t bmap_count = bpg/8;
		if (bpg % 8)
		{
			++bpg;
		}
		assert(bpg > 0);
		
		size_t bmap_blocks = bmap_count/fs->block_size();
		if (bmap_count % fs->block_size())
		{
			++bmap_blocks;
		}
		
		assert(bmap_blocks == 1);
		block_usage_blk = get_block_internal_no_lock(gd->block_usage_map, lock);
		block_usage = Utils::Bitset_Ptr<uint8_t>(block_usage_blk->data, fs->block_size(), bpg, false);
		
		node_usage_blk = get_block_internal_no_lock(gd->inode_usage_map, lock);
		node_usage = Utils::Bitset_Ptr<uint8_t>(node_usage_blk->data, fs->block_size(), fs->sb.nodes_per_group, false);
		modified_map.resize(bpg);
		assert(modified_map.size() == bpg);
		assert(bpg > 0);
		
		assert(!modified_map[0]);
		assert(modified_map.size() > 0);
		initted = true;
		__sync_synchronize();
	}
	
	void block_group_t::init() const
	{
		const_cast<block_group_t*>(this)->init();
	}
	
	Utils::shared_ptr<block_t> block_group_t::get_block_internal(const size_t index) noexcept
	{
		read_lock lock(mut);
		return get_block_internal_no_lock(index, lock);
	}
	
	Utils::shared_ptr<block_t> block_group_t::get_block_internal_no_lock(const size_t index, read_lock& lock) noexcept
	{
		assert(lock.owns_lock());
		if (!lock.owns_lock())
		{
			lock.lock();
		}
		auto group_n = index / blocks_per_group;
		if (group_n != group_index)
		{
			return fs->get_block(index);
		}
		
		auto rindex = index % blocks_per_group;
		
		Utils::shared_ptr<block_t> block = nullptr;
		
		if (cached_blocks.count(rindex))
		{
			block = cached_blocks.at(rindex).lock();
			
			if (block)
			{
				cache_block(block, lock);
				return block;
			}
		}
		
		
		{
			lock.unlock();
			write_lock wlock(mut);
			block = get_block_internal_no_lock(index, wlock);
		}
		lock.lock();
		return block;
	}
	
	Utils::shared_ptr<block_t> block_group_t::get_block_internal_no_lock(size_t index, write_lock& lock, bool no_unlock) noexcept
	{
		assert(lock.owns_lock());
		if (!lock.owns_lock())
		{
			lock.lock();
		}
		
		Utils::shared_ptr<block_t> block = nullptr;
		
		auto group_n = index / blocks_per_group;
		if (group_n != group_index)
		{
			block = fs->get_block(index);
			return block;
		}
		
		
		
		index %= blocks_per_group;
		
		if (cached_blocks.count(index))
		{
			block = cached_blocks.at(index).lock();
			
			if (block)
			{
				cache_block(block, lock, no_unlock);
				return block;
			}
		}
		
		auto block_size = fs->block_size();
		
		auto raw = new uint8_t[sizeof(block_t) + block_size];
		memset(raw, 0, sizeof(block_t) + block_size);
		
		block = Utils::shared_ptr<block_t>((block_t*)raw);
		new (block.get()) block_t();
		memset(block->data, 0, block_size);
		
		block->group = this;
		block->index = index;
		
		
		uint8_t* cached = &block->data[0];
		
		auto read = fs->read_block(index + blocks_per_group*group_index, cached);
		if (read != block_size)
		{
			TRACE("Invalid block.");
			assert(false);
			return nullptr;
		}
		
		
		cached_blocks[index] = block;
		cache_block(block, lock, no_unlock);
		
		
		
		return block;
	}
	
	Utils::shared_ptr<inode_t> block_group_t::get_node_internal(const size_t index) noexcept
	{
		read_lock lock(mut);
		return get_node_internal_no_lock(index, lock);
	}
	
	/*Utils::shared_ptr<inode_t> block_group_t::get_node_internal_no_lock(const size_t index, read_lock& lock) noexcept
	{
		assert(lock.owns_lock());
		if (!lock.owns_lock())
		{
			lock.lock();
		}
		auto group_n = index / nodes_per_group;
		if (group_n != group_index)
		{
			return fs->get_inode(index);
		}
		
		Utils::shared_ptr<inode_t> node = nullptr;
		
		auto bindex = inode_block_index(index);
		auto block = get_block_internal_no_lock(bindex, lock);
		if (!block)
		{
			return nullptr;
		}
		
		auto rindex = ((index - 1) % inodes_size()) % fs->inodes_per_block();
		auto raw_block = const_cast<uint8_t*>(block->data);
		raw_block += (rindex * fs->inode_size());
		auto raw_node = (inode_t*)raw_block;
		return Utils::shared_ptr<inode_t>(block, raw_node);
	}*/
	
	Utils::shared_ptr<inode_t> block_group_t::get_node_internal_no_lock(const size_t index, read_lock& lock) noexcept
	{
		assert(lock.owns_lock());
		if (!lock.owns_lock())
		{
			lock.lock();
		}
		auto group_n = index / nodes_per_group;
		if (group_n != group_index)
		{
			return fs->get_inode(index);
		}
		
		Utils::shared_ptr<inode_t> node = nullptr;
		
		auto bindex = inode_block_index(index);
		auto block = get_block_internal_no_lock(bindex, lock);
		if (!block)
		{
			return nullptr;
		}
		
		modified_map[bindex] = true;
		
		auto rindex = ((index - 1) % inodes_size()) % fs->inodes_per_block();
		auto raw_block = const_cast<uint8_t*>(block->data);
		raw_block += (rindex * fs->inode_size());
		auto raw_node = (inode_t*)raw_block;
		return Utils::shared_ptr<inode_t>(block, raw_node);
	}
	
	Utils::shared_ptr<inode_t> block_group_t::get_node_internal_no_lock(const size_t index, write_lock& lock, bool no_unlock) noexcept
	{
		assert(lock.owns_lock());
		if (!lock.owns_lock())
		{
			lock.lock();
		}
		auto group_n = (index-1) / nodes_per_group;
		if (group_n != group_index)
		{
			return fs->get_inode(index);
		}
		
		Utils::shared_ptr<inode_t> node = nullptr;
		
		auto bindex = inode_block_index(index);
		auto block = get_block_internal_no_lock(bindex, lock, no_unlock);
		if (!block)
		{
			return nullptr;
		}
		
		modified_map[bindex] = true;
		
		auto rindex = ((index - 1) % inodes_size()) % fs->inodes_per_block();
		auto raw_block = const_cast<uint8_t*>(block->data);
		raw_block += (rindex * fs->inode_size());
		auto raw_node = (inode_t*)raw_block;
		return Utils::shared_ptr<inode_t>(block, raw_node);
	}
	
	size_t block_group_t::inode_block_index(const size_t nindex) const noexcept
	{
		if (nindex == 0 || ((nindex-1) / inodes_size() != group_index))
		{
			TRACE_VAL(nindex);
			assert(false);
			return npos;
		}
		
		return (((nindex-1) % inodes_size())*fs->inode_size())/fs->block_size() + gd->inode_table;
	}
	
	
	
	
	
	
	
	// Private Methods
	
	
	void block_group_t::cache_block(const Utils::shared_ptr<block_t>& block, write_lock& lock, bool no_unlock) noexcept
	{
		if (no_unlock)
		{
			Utils::lock_guard<Utils::mutex> calock(cache_m);
			block_ptr_cache.cache(block);
		}
		else
		{
			lock.unlock();
			cache_m.lock();
			block_ptr_cache.cache(block);
			cache_m.unlock();
			lock.lock();
		}
	}
	
	void block_group_t::cache_block(const Utils::shared_ptr<block_t>& block, read_lock& lock, bool no_unlock) noexcept
	{
		if (no_unlock)
		{
			Utils::lock_guard<Utils::mutex> calock(cache_m);
			block_ptr_cache.cache(block);
		}
		else
		{
			lock.unlock();
			cache_m.lock();
			block_ptr_cache.cache(block);
			cache_m.unlock();
			lock.lock();
		}
	}
	
	size_t block_group_t::free_block_no_lock() const
	{
		uint32_t index;
		if (block_usage.firstFalse(&index))
		{
			return index;
		}
		
		return npos;
		
	}
	
	size_t block_group_t::free_node_no_lock() const
	{
		uint32_t index;
		if (node_usage.firstFalse(&index))
		{
			return index+1;
		}
		
		return npos;
	}
	
	
	
	
	
	
	
	
	
	
	// Public methods
	
	
	bool block_group_t::node_is_free(const size_t i) const
	{
		if (!initted)
		{
			init();
		}
		read_lock lock(mut);
		assert(i < node_usage.size() + 1);
		assert(i > 0);
		if (i <= 0)
		{
			return false;
		}
		return !node_usage.get(i-1);
	}
	
	bool block_group_t::block_is_free(const size_t i) const
	{
		if (!initted)
		{
			init();
		}
		read_lock lock(mut);
		assert(i < block_usage.size());
		return !block_usage.get(i);
	}
	
	size_t block_group_t::blocks_size() const noexcept
	{
		return fs->sb.blocks_per_group;
	}
	
	size_t block_group_t::inodes_size() const noexcept
	{
		return fs->sb.nodes_per_group;
	}
	
	
	size_t block_group_t::free_node() const
	{
		if (!initted)
		{
			init();
		}
		read_lock lock(mut);
		uint32_t index;
		if (node_usage.firstFalse(&index))
		{
			return index+1;
		}
		
		return npos;
	}
	
	size_t block_group_t::free_block() const
	{
		if (!initted)
		{
			init();
		}
		read_lock lock(mut);
		return free_block_no_lock();
	}
	
	
	size_t block_group_t::free_node_count() const noexcept
	{
		return gd->unallocated_inodes;
	}
	
	size_t block_group_t::free_block_count() const noexcept
	{
		return gd->unallocated_blocks;
	}
	
	uint16_t block_group_t::directories() const noexcept
	{
		return gd->directories;
	}
	
	Utils::shared_ptr<block_t> block_group_t::reserve_block(size_t& index)
	{
		if (!initted)
		{
			init();
		}
		
		if (free_block_count() == 0)
		{
			return nullptr;
		}
		
		{
			write_lock lock(mut);
			if (free_block_count() == 0)
			{
				return nullptr;
			}
			index = free_block_no_lock();
			if (index == npos)
			{
				return nullptr;
			}
			block_usage.set(index, true);
			modified_map[index] = true;
			modified_map[gd->block_usage_map - group_index*fs->sb.blocks_per_group] = true;
			--gd->unallocated_blocks;
			assert(gd->block_usage_map < modified_map.size() + group_index*fs->sb.blocks_per_group);
			index += group_index*fs->sb.blocks_per_group;
			auto blk = get_block_internal_no_lock(index, lock);
			assert(blk->index == index % blocks_per_group);
			assert(blk->group == this);
			memset(blk->data, 0, fs->block_size());
			return blk;
		}
	}
	
	
	bool block_group_t::release_block(const size_t index)
	{
		if (!initted)
		{
			init();
		}
		
		if (index/fs->sb.blocks_per_group != group_index)
		{
			return false;
		}
		
		write_lock lock(mut);
		
		if (!block_usage.get(index))
		{
			return false;
		}
		
		block_usage.set(index, false);
		++gd->unallocated_blocks;
		modified_map[index] = true;
		modified_map[gd->block_usage_map] = true;
		
		return true;
	}
	
	
	Utils::shared_ptr<inode_t> block_group_t::reserve_node(size_t& index)
	{
		if (!initted)
		{
			init();
		}
		
		if (free_node_count() == 0)
		{
			return nullptr;
		}
		
		{
			write_lock lock(mut);
			if (free_node_count() == 0)
			{
				return nullptr;
			}
			index = free_node_no_lock();
			if (index == npos)
			{
				return nullptr;
			}
			node_usage.set(index-1, true);
			
			auto inode_block = inode_block_index(group_index*fs->sb.nodes_per_group + index);
			if (inode_block != npos)
			{
				modified_map[inode_block] = true;
			}
			modified_map[gd->inode_usage_map] = true;
			--gd->unallocated_inodes;
			assert(gd->inode_usage_map < modified_map.size());
			index += (group_index*fs->sb.nodes_per_group);
			return get_node_internal_no_lock(index, lock);
		}
	}
	
	bool block_group_t::release_node(const size_t index)
	{
		if (!initted)
		{
			init();
		}
		
		assert(index > 0);
		if (index == 0)
		{
			return false;
		}
		
		if ((index-1)/fs->sb.nodes_per_group != group_index)
		{
			return false;
		}
		
		write_lock lock(mut);
		
		if (!node_usage.get(index-1))
		{
			return false;
		}
		
		node_usage.set(index-1, false);
		++gd->unallocated_inodes;
		auto inode_block = inode_block_index(index);
		if (inode_block != npos)
		{
			modified_map[inode_block] = true;
		}
		
		modified_map[gd->inode_usage_map] = true;
		
		return true;
	}
	
	
	bool block_group_t::block_is_modified(size_t index) const
	{
		if (!initted)
		{
			return false;
		}
		
		index %= blocks_per_group;
		read_lock lock(mut);
		assert(index < modified_map.size());
		if (index >= modified_map.size())
		{
			return false;
		}
		
		return modified_map.at(index);
	}
	
	
	void block_group_t::write_back() noexcept
	{
		write_lock lock(mut);
		if (!initted)
		{
			return;
		}
		
		for (size_t i = 0; i < modified_map.size(); ++i)
		{
			if (modified_map.at(i))
			{
				commit_block(get_block_internal_no_lock(i + group_index*blocks_per_group, lock).get());
			}
		}
		
		commit_block(block_usage_blk.get());
		commit_block(node_usage_blk.get());
	}
	
	void block_group_t::commit_block(block_t* blk) noexcept
	{
		
		/*TRACE("Committing block:");
		TRACE_VAL(blk->index);*/
		assert(blk);
		if (unlikely(!blk))
		{
			return;
		}
		
		auto index = size_t(blk->index);
		
		auto written = fs->write_block(index + group_index*blocks_per_group, blk->data);
		if (written != fs->block_size())
		{
			TRACE("Invalid block.");
			assert(false);
		}
		blk->modified = false;
		modified_map[blk->index] = false;
	}
	
	
	Utils::shared_ptr<block_t> block_group_t::get_block(const size_t index, bool write)
	{
		if (!initted)
		{
			init();
		}
		Utils::shared_ptr<block_t> block = nullptr;
		if (write)
		{
			write_lock lock(mut);
			block = get_block_internal_no_lock(index, lock);
			if (block)
			{
				modified_map[index % blocks_per_group] = true;
			}
		}
		else
		{
			block = get_block_internal(index);
		}
		
		return block;
	}
	
	
	Utils::shared_ptr<inode_t> block_group_t::allocate_directory(size_t& index) noexcept
	{
		auto rsv = reserve_node(index);
		if (!rsv)
		{
			return nullptr;
		}
		
		rsv->type = EXT2_INODE_TYPE_DIR;
		rsv->raw_perms = 0;
		rsv->user_ID = 0;
		rsv->size_low = 0;
		memset(rsv->data, 0, 60);
		
		return rsv;
	}
	
	Utils::shared_ptr<inode_t> block_group_t::allocate_file(size_t& index) noexcept
	{
		auto rsv = reserve_node(index);
		if (!rsv)
		{
			return nullptr;
		}
		
		rsv->type = EXT2_INODE_TYPE_FILE;
		rsv->raw_perms = 0;
		rsv->user_ID = 0;
		rsv->size_low = 0;
		memset(rsv->data, 0, 60);
		
		return rsv;
	}
	
	
	bool block_group_t::mark_node_modified(const size_t n)
	{
		modified_map[inode_block_index(n)] = true;
		return true;
		
	}
	
}