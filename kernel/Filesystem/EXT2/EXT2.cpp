#include "EXT2.hh"
#include <drivers/Disk/Disk.hh>
#include "EXT2DirectoryNode.hh"
#include "EXT2FileNode.hh"
#include "EXT2SymLinkNode.hh"
#include "EXT2BlockDevice.hh"
#include "EXT2CharDevice.hh"
#include <kernel/Filesystem/BlockDeviceNode.hh>
#include <kernel/Filesystem/Devices/DeviceTarget.hh>
#include <kernel/Filesystem/MBR.hh>
#include <kernel/Filesystem/MBR_System_IDs.h>
#include <Utils/BitSet.hh>


#define EXT2_SUPERBLOCK_OFFSET (::Kernel::FS::detail::EXT2::superblock_constants::offset)
//#define EXT2_SUPERBLOCK_OFFSET 1024
#define EXT2_ROOT_INODE 0x2



#define DIVU(X, Y) (((X) / (Y)) + (((X) % (Y) == 0) ? 0 : 1))


static bool is_big_endian()
{
	union
	{
		uint32_t x;
		uint8_t y[4];
	} val = {0x01020304};
	
	static_assert(sizeof(uint32_t) == 4*sizeof(uint8_t));
	
	return val.y[0] == 1;
}

namespace Kernel::FS
{
	namespace detail::EXT2
	{
		dirent_t* next_dirent(dirent_t* array, size_t array_size, dirent_t* current, bool allow_empty)
		{
			assert(array);
			assert(array_size);
			if (!current)
			{
				if (unlikely(allow_empty))
				{
					return array;
				}
				current = array;
			}
			else
			{
				if (unlikely(!current->entry_size))
				{
					return nullptr;
				}
				current = (dirent_t*)((uint8_t*)current + current->entry_size);
			}
			while (!(current->inode || allow_empty))
			{
				if (current->entry_size == 0)
				{
					return nullptr;
				}
				if (!(current->entry_size > 0))
				{
					TRACE("EMPTY:");
					TRACE(current->inode);
					TRACE((void*)current);
				}
				assert(current->entry_size > 0);
				
				current = (dirent_t*)((uint8_t*)current + current->entry_size);
				if ((uint8_t*)current >= ((uint8_t*)array + array_size))
				{
					return nullptr;
				}
			}
			
			if ((uint8_t*)current >= ((uint8_t*)array + array_size))
			{
				return nullptr;
			}
			
			return current;
		}
		
		
		
		
		size_t dirent_name_len(const dirent_t* ent, bool dirent_type_field) noexcept
		{
			assert(likely(ent != nullptr));
			if (unlikely(!ent))
			{
				return 0;
			}
			
			if (!dirent_type_field)
			{
				uint16_t len = ent->name_len_msb;
				len = len << 8;
				len |= ent->name_len_lsb;
				return len;
			}
			else
			{
				return ent->name_len_lsb;
			}
		}
		
		
		
	}
	
	
	
	
	
	
	
	EXT2::EXT2(Drivers::Disk& disk, int partition) noexcept : Filesystem(),
			_factory(this),
			disk(&disk),
			sb(),
			bg_table(nullptr),
			block_group_count(0),
			cached_blocks(),
			max_node_blocks(0),
			nodes(),
			block_ptr_cache(),
			groups()
	{
		assert(!is_big_endian());
		
		if (partition >= 0)
		{
			assert(partition < 4);
			/*MBR_PTable_t mbr;
			auto mbr_res = disk.read(MBR_DISK_OFFSET, sizeof(MBR_PTable_t), (uint8_t*)&mbr);
			assert(mbr_res == sizeof(MBR_PTable_t));*/
			
			MBR_PTable_t mbr;
			MBR_ptable_read(&disk, &mbr);
			
			
			auto sys = mbr.partitions[partition].system_ID;
			TRACE((int)sys);
			assert(sys == MBR_SYS_EXT);
			
			
			// TODO
			assert(NOT_IMPLEMENTED);
			
		}
		
		
		auto res = disk.read(EXT2_SUPERBLOCK_OFFSET, sizeof(sb), (uint8_t*)&sb);
		assert(res == sizeof(sb));
		
		if (sb.major_ver > 0)
		{
			TRACE_VAL(sb.major_ver);
			TRACE_VAL(sb.minor_ver);
			//assert(false);
		}
		
		assert(sb.signature == detail::EXT2::superblock_constants::ext2_signature);
		
		const size_t block_sz = (1024 << sb.block_size_modifier);
		
		size_t bgdt_off = (EXT2_SUPERBLOCK_OFFSET + sizeof(sb))/block_sz;
		if ((EXT2_SUPERBLOCK_OFFSET + sizeof(sb)) % block_sz)
		{
			++bgdt_off;
		}
		
		bgdt_off *= block_sz;
		
		
		this->block_group_count = sb.blocks / sb.blocks_per_group;
		if (sb.blocks % sb.blocks_per_group)
		{
			++this->block_group_count;
		}
		
		
		
		{
			size_t groups_inodes = sb.nodes / sb.nodes_per_group;
			if (sb.nodes % sb.nodes_per_group)
			{
				++groups_inodes;
			}
			assert(block_group_count == groups_inodes);
		}
		
		
		
		
		bg_table = new detail::EXT2::block_group_desc_t[block_group_count];
		
		res = disk.read(bgdt_off, block_group_count*sizeof(detail::EXT2::block_group_desc_t), (uint8_t*)bg_table);
		assert(res == block_group_count*sizeof(detail::EXT2::block_group_desc_t));
		
		using namespace detail::EXT2;
		
		
		uint32_t reserved_nodes = superblock_constants::first_non_rsv_node;
		
		if (sb.major_ver >= superblock_constants::ext_major_ver)
		{
			reserved_nodes = sb.first_non_rsv_node;
		}
		
		assert(reserved_nodes >= 2);
		
		// Was index, make it a count
		reserved_nodes -= 1;
		
		
		// Calculate max_node_blocks
		{
			auto ptrs_per_blk = block_size() / 4;
			auto indir1 = ptrs_per_blk;
			auto indir2 = ptrs_per_blk * indir1;
			auto indir3 = ptrs_per_blk * indir2;
			max_node_blocks = 12 + indir1 + indir2 + indir3;
		}
		
		
		// Build block groups
		assert(block_group_count > 0);
		groups.resize(block_group_count);
		assert(groups.size() == block_group_count);
		for (size_t i = 0; i < block_group_count; ++i)
		{
			groups[i] = Utils::shared_ptr<block_group_t>(new block_group_t(*this, bg_table[i], i));
		}
		
		
		
		
		
		
		
		
		
		
		auto root = get_inode(2);
		assert(root);
		assert(root->type == EXT2_INODE_TYPE_DIR);
		auto root_n = new EXT2DirectoryNode(nullptr, this, root, "", 2);
		_root = root_n;
		nodes[2] = _root;
		assert(this->root() == root_n);
		assert(this->root()->as_directory()->name.length() == 0);
	}
	
	EXT2::~EXT2()
	{
		for (auto& gr : groups)
		{
			gr->write_back();
		}
		
		for (auto& kv : nodes)
		{
			delete kv.second;
		}
		
		nodes.clear();
		_root = nullptr;
		cached_blocks.clear();
		block_ptr_cache.reset();
		
		
		
		groups.clear();
		assert(bg_table);
		
		
		size_t bgdt_off = (EXT2_SUPERBLOCK_OFFSET + sizeof(sb))/block_size();
		if ((EXT2_SUPERBLOCK_OFFSET + sizeof(sb)) % block_size())
		{
			++bgdt_off;
		}
		
		bgdt_off *= block_size();
		
		auto res = disk->write(bgdt_off, block_group_count*sizeof(detail::EXT2::block_group_desc_t), (const uint8_t*)bg_table);
		delete[] bg_table;
		bg_table = nullptr;
		
		//assert(NOT_IMPLEMENTED);
	}
	
	
	
	size_t EXT2::inode_size() const noexcept
	{
		using namespace detail::EXT2;
		
		if (sb.major_ver >= superblock_constants::ext_major_ver)
		{
			return sb.node_struct_size;
		}
		
		return superblock_constants::node_struct_size;
	}
	
	size_t EXT2::block_size() const noexcept
	{
		assert(sb.block_size_modifier <= (sizeof(size_t)*8 - 11));
		return (1024 << sb.block_size_modifier);
	}
	
	size_t EXT2::group_size() const noexcept
	{
		return sb.blocks_per_group * block_size();
	}
	
	size_t EXT2::fragment_size() const noexcept
	{
		assert(sb.fragment_size_modifier <= (sizeof(size_t)*8 - 11));
		return (1024 << sb.fragment_size_modifier);
	}
	
	uint32_t EXT2::free_blocks() const noexcept
	{
		return sb.unallocated_blocks;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	auto EXT2::calc_inode_group(inode_index_t n) const -> group_index_t
	{
		assert(n > 0);
		assert(sb.nodes_per_group > 0);
		return (n - 1) / sb.nodes_per_group;
	}
	
	auto EXT2::get_inode(inode_index_t i) noexcept -> Utils::shared_ptr<inode_type>
	{
		return __get_inode(i);
	}
	
	auto EXT2::get_inode(inode_index_t i) const noexcept -> Utils::shared_ptr<const inode_type>
	{
		return __get_inode(i);
	}
	
	auto EXT2::inodes_per_block() const noexcept -> inode_index_t
	{
		assert(block_size() % inode_size() == 0);
		return block_size() / inode_size();
	}
	
	auto EXT2::get_block(block_index_type n) -> Utils::shared_ptr<block_t>
	{
		assert(groups.size() == block_group_count);
		assert(n / sb.blocks_per_group < groups.size());
		if (n <= sb.superblock_block_num)
		{
			return nullptr;
		}
		auto gr = groups[n / sb.blocks_per_group];
		assert(gr);
		//assert(n != 70);
		return gr->get_block(n);
		
		/*if (cached_blocks.count(n))
		{
			auto cached = cached_blocks.at(n).lock();
			
			if (cached)
			{
				block_ptr_cache.cache(cached);
				return cached;
			}
		}
		
		static_assert(sizeof(block_t) == sizeof(uint16_t) + sizeof(void*));
		auto raw = new uint8_t[sizeof(block_t) + block_size()];
		memset(raw, 0, sizeof(block_t) + block_size());
		
		block_t* block = (block_t*)raw;
		new (block) block_t();
		block->group = gr.get();
		block->index = n % sb.blocks_per_group;
		
		//block->len = block_size();
		
		uint8_t* cached = &block->data[0];//new uint8_t[block_size()];
		
		auto read = disk->read(lba_from_block(n), block_size(), cached);
		if (read != block_size())
		{
			delete[] raw;
			assert(false);
		}
		
		
		Utils::shared_ptr<block_t> ptr(block);
		//Utils::shared_ptr<block_t> block(new block_t{block_size(), cached});
		
		cached_blocks[n] = ptr;
		block_ptr_cache.cache(ptr);
		return ptr;*/
	}
	
	auto EXT2::get_block(block_index_type n) const -> Utils::shared_ptr<const block_t>
	{
		return const_cast<EXT2*>(this)->get_block(n);
		/*if (cached_blocks.count(n))
		{
			auto cached = cached_blocks.at(n).lock();
			if (cached)
			{
				return cached;
			}
		}
		
		auto raw = new uint8_t[sizeof(size_t) + block_size()];
		
		block_t* block = (block_t*)raw;
		block->len = block_size();
		
		uint8_t* cached = &block->data[0];//new uint8_t[block_size()];
		
		
		auto read = disk->read(lba_from_block(n), block_size(), cached);
		if (read != block_size())
		{
			delete[] cached;
			assert(false);
		}
		
		Utils::shared_ptr<block_t> ptr(block);
		//Utils::shared_ptr<block_t> block(new block_t{block_size(), cached});
		
		cached_blocks[n] = ptr;
		return ptr;*/
	}
	
	uint8_t* EXT2::read_inode(const inode_type* node, size_t first, size_t count)
	{
		assert(node);
		if (count == 0)
		{
			return nullptr;
		}
		assert(max_node_blocks > first);
		assert((max_node_blocks - first) > count);
		
		uint8_t* buffer = new uint8_t[block_size() * count];
		
		size_t n = 0;
		size_t s = first;
		size_t i = 0;
		size_t block_count;
		
		TRACE_VAL(first);
		TRACE_VAL(count);
		uint32_t ptrs[count+first];
		if (find_valid_block_ptrs(ptrs, node, count + first) < count)
		{
			delete[] buffer;
			return nullptr;
		}
		else
		{
			for (size_t g = 0; g < count+first; ++g)
			{
				TRACE_VAL(g);
				TRACE_VAL(ptrs[g]);
			}
			TRACE("\n\n");
			for (size_t j = first; j < count+first; ++j)
			{
				TRACE_VAL(ptrs[j]);
				assert(ptrs[j] > 0);
				auto blk = get_block(ptrs[j]);
				assert(blk);
				if (!blk)
				{
					delete[] buffer;
					return nullptr;
				}
				const uint8_t* res = blk->data;
				memcpy(buffer + (j-first)*block_size(), res, block_size());
			}
			return buffer;
		}
		
		
		if (first < 12)
		{
			n = 12 - first;
			if (count < n)
			{
				n = count;
			}
			
			if (!read_indirect(&node->direct[s], n, buffer))
			{
				delete[] buffer;
				return nullptr;
			}
			else
			{
				i += n;
			}
		}
		
		count -= n;
		if (first > 0)
		{
			if (n < first)
			{
				first -= n;
			}
			else
			{
				first = 0;
			}
		}
		
		
		if (!count)
		{
			return buffer;
		}
		
		if (first < block_size()/4)
		{
			auto sptr = get_block(node->indirect_1);
			assert(sptr);
			if (!sptr)
			{
				delete[] buffer;
				return nullptr;
			}
			auto ptrs = (uint32_t*)sptr->data;
			//auto ptrs = (uint32_t*)get_block(node->indirect_1);
			
			
			s = first;
			n = block_size()/4;
			if (count < n)
			{
				n = count;
			}
			
			if (!read_indirect(&ptrs[s], n, buffer + i*block_size()))
			{
				delete[] buffer;
				return nullptr;
			}
			
			i += n;
		}
		
		
		count -= n;
		if (first > 0)
		{
			if (n < first)
			{
				first -= n;
			}
			else
			{
				first = 0;
			}
		}
		
		if (!count)
		{
			return buffer;
		}
		
		n = count;
		
		auto sptr2 = get_block(node->indirect_2);
		assert(sptr2);
		if (!sptr2)
		{
			delete[] buffer;
			return nullptr;
		}
		auto ptrs2 = (uint32_t*)sptr2->data;
		//auto ptrs2 = (uint32_t*)get_block(node->indirect_2);
		
		
		
		size_t j = first/(block_size()/4);
		first -= j*(block_size()/4);
		
		for (; j < block_size()/4 && count > 0; ++j)
		{
			auto sptr = get_block(ptrs2[j]);
			assert(sptr);
			if (!sptr)
			{
				delete[] buffer;
				return nullptr;
			}
			auto ptrs = (uint32_t*)sptr->data;
			/*auto ptrs = (uint32_t*)get_block(ptrs2[j]);
			assert(ptrs);
			if (!ptrs)
			{
				delete[] buffer;
				return nullptr;
			}*/
			
			s = first;
			
			n = block_size() / 4 - s;
			if (n > count)
			{
				n = count;
			}
			
			if (!read_indirect(&ptrs[s], n, buffer + i*block_size()))
			{
				delete[] buffer;
				return nullptr;
			}
			
			i += n;
			first -= s;
			count -= n;
		}
		
		return buffer;
	}
	
	uint8_t* EXT2::read_inode(Utils::shared_ptr<const inode_type> node, size_t first, size_t count)
	{
		const inode_type* rawptr = node.get();
		return read_inode(rawptr, first, count);
	}
	
	bool EXT2::read_indirect(const uint32_t* ptrs, const size_t count, uint8_t* target) const
	{
		assert(ptrs);
		assert(target);
		if (count == 0)
		{
			return false;
		}
		
		for (size_t i = 0; i < count; ++i)
		{
			assert(ptrs[i] > 0);
			auto sres = get_block(ptrs[i]);
			assert(sres);
			if (!sres)
			{
				return false;
			}
			const uint8_t* res = sres->data;
			memcpy(target + i*block_size(), res, block_size());
		}
		
		return true;
	}
	
	uint32_t EXT2::valid_block_ptrs(const uint32_t* block, const size_t start) const noexcept
	{
		assert(block);
		const size_t n = block_size()/sizeof(uint32_t);
		size_t k = 0;
		for (size_t j = start; j < n; ++j)
		{
			if (block[j] > 0)
			{
				++k;
			}
		}
		return k;
	}
	
	uint32_t EXT2::find_valid_block_ptrs(uint32_t* ptrs, const inode_type* node, size_t count, size_t start_index, bool keep_zeros) const noexcept
	{
		assert(ptrs);
		assert(node);
		
		size_t write_index = 0;
		size_t total_index = 0;
		const size_t ptr_per_blk = block_size()/sizeof(uint32_t);
		size_t i; // Generic index
		
		
		memset(ptrs, 0, sizeof(uint32_t)*count);
		
		for (i = 0; i < 12 && write_index < count; ++i)
		{
			if (node->direct[i] > 0 || keep_zeros)
			{
				if (total_index >= start_index)
				{
					ptrs[write_index] = node->direct[i];
					++write_index;
				}
				++total_index;
			}
		}
		
		if (write_index >= count)
		{
			return write_index;
		}
		
		if (node->indirect_1 > 0)
		{
			auto indirect1 = get_block(node->indirect_1);
			const uint32_t* data = (const uint32_t*)indirect1->data;
			for (i = 0; i < ptr_per_blk && write_index < count; ++i)
			{
				if (data[i] > 0 || keep_zeros)
				{
					if (total_index >= start_index)
					{
						ptrs[write_index] = data[i];
						++write_index;
					}
					++total_index;
				}
			}
		}
		
		if (write_index >= count)
		{
			return write_index;
		}
		
		if (node->indirect_2 > 0)
		{
			auto indirect2 = get_block(node->indirect_2);
			const uint32_t* data2 = (const uint32_t*)indirect2->data;
			for (size_t j = 0; j < ptr_per_blk && write_index < count; ++j)
			{
				if (data2[j] > 0)
				{
					auto indirect1 = get_block(data2[j]);
					const uint32_t* data = (const uint32_t*)indirect1->data;
					
					for (i = 0; i < ptr_per_blk && write_index < count; ++i)
					{
						if (data[i] > 0 || keep_zeros)
						{
							if (total_index >= start_index)
							{
								ptrs[write_index] = data[i];
								++write_index;
							}
							++total_index;
						}
					}
				}
			}
		}
		
		if (write_index >= count)
		{
			return write_index;
		}
		
		if (node->indirect_3 > 0)
		{
			auto indirect3 = get_block(node->indirect_3);
			const uint32_t* data3 = (const uint32_t*)indirect3->data;
			for (size_t k = 0; k < ptr_per_blk && write_index < count; ++k)
			{
				if (data3[k] > 0)
				{
					auto indirect2 = get_block(data3[k]);
					const uint32_t* data2 = (const uint32_t*)indirect2->data;
					
					for (size_t j = 0; j < ptr_per_blk && write_index < count; ++j)
					{
						if (data2[j] > 0)
						{
							auto indirect1 = get_block(data2[j]);
							const uint32_t* data = (const uint32_t*)indirect1->data;
							
							for (i = 0; i < ptr_per_blk && write_index < count; ++i)
							{
								if (data[i] > 0 || keep_zeros)
								{
									if (total_index >= start_index)
									{
										ptrs[write_index] = data[i];
										++write_index;
									}
									++total_index;
								}
							}
						}
					}
				}
			}
		}
		
		
		return write_index;
	}
	
	Node* EXT2::parse_node(DirectoryNode* parent, detail::EXT2::dirent_t* ent)
	{
		assert(ent);
		Node* n = nodes[ent->inode];
		if (n)
		{
			return n;
		}
		
		auto next_n = get_inode(ent->inode);
		assert(next_n);
		if (!next_n)
		{
			return nullptr;
		}
		
		Utils::string nname((const char*)ent->name, dirent_name_len(ent));
		assert(nname.c_str()[nname.length()] == '\0');
		
		if (next_n->type == EXT2_INODE_TYPE_DIR)
		{
			n = new EXT2DirectoryNode(parent, this, next_n, nname, ent->inode);
		}
		else if (next_n->type == EXT2_INODE_TYPE_BLOCK_DEV || next_n->type == EXT2_INODE_TYPE_CHAR_DEV)
		{
			dev_t dev_id;
			decode_device_signature(*(const uint32_t*)next_n->data, &dev_id);
			
			if (next_n->type == EXT2_INODE_TYPE_BLOCK_DEV)
			{
				auto target = DeviceTarget::find_target(this, DeviceTargetType::Block, dev_id);
				n = new EXT2BlockDeviceNode(parent, this, next_n, nname, ent->inode, target);
			}
			else
			{
				auto target = DeviceTarget::find_target(this, DeviceTargetType::Char, dev_id);
				n = new EXT2CharDeviceNode(parent, this, next_n, nname, ent->inode, target);
			}
		}
		else if (next_n->type == EXT2_INODE_TYPE_FILE)
		{
			n = new EXT2FileNode(parent, this, next_n, nname, ent->inode);
		}
		else if (next_n->type == EXT2_INODE_TYPE_SYMLINK)
		{
			
			n = new EXT2SymLinkNode(parent, this, next_n, nname, ent->inode);
		}
		else
		{
			TRACE_VAL(nname.c_str());
			//TRACE((const char*)ent->name);
			TRACE(next_n->type);
			assert(NOT_IMPLEMENTED);
		}
		
		assert(n);
		if (n)
		{
			n->set_parent(parent);
		}
		nodes[ent->inode] = n;
		return n;
	}
	
	node_ptr<> EXT2::parse_node2(DirectoryNode* parent, detail::EXT2::dirent_t* ent)
	{
		assert(ent);
		node_ptr<> np = nodes2[ent->inode];
		if (np)
		{
			return np;
		}
		
		auto next_n = get_inode(ent->inode);
		assert(next_n);
		if (!next_n)
		{
			return nullptr;
		}
		
		Utils::string nname((const char*)ent->name, dirent_name_len(ent));
		assert(nname.c_str()[nname.length()] == '\0');
		
		Node* n = nullptr;
		if (next_n->type == EXT2_INODE_TYPE_DIR)
		{
			n = new EXT2DirectoryNode(parent, this, next_n, nname, ent->inode);
		}
		else if (next_n->type == EXT2_INODE_TYPE_BLOCK_DEV || next_n->type == EXT2_INODE_TYPE_CHAR_DEV)
		{
			dev_t dev_id;
			decode_device_signature(*(const uint32_t*)next_n->data, &dev_id);
			
			if (next_n->type == EXT2_INODE_TYPE_BLOCK_DEV)
			{
				auto target = DeviceTarget::find_target(this, DeviceTargetType::Block, dev_id);
				n = new EXT2BlockDeviceNode(parent, this, next_n, nname, ent->inode, target);
			}
			else
			{
				auto target = DeviceTarget::find_target(this, DeviceTargetType::Char, dev_id);
				n = new EXT2CharDeviceNode(parent, this, next_n, nname, ent->inode, target);
			}
		}
		else if (next_n->type == EXT2_INODE_TYPE_FILE)
		{
			n = new EXT2FileNode(parent, this, next_n, nname, ent->inode);
		}
		else if (next_n->type == EXT2_INODE_TYPE_SYMLINK)
		{
			
			n = new EXT2SymLinkNode(parent, this, next_n, nname, ent->inode);
		}
		else
		{
			TRACE_VAL(nname.c_str());
			//TRACE((const char*)ent->name);
			TRACE(next_n->type);
			assert(NOT_IMPLEMENTED);
		}
		
		
		
		assert(n);
		if (n)
		{
			n->set_parent(parent);
		}
		np = node_ptr<>(Utils::shared_ptr<Node>(n));
		
		nodes2[ent->inode] = np;
		return np;
	}
	
	bool EXT2::extended_superblock() const noexcept
	{
		return (sb.major_ver >= detail::EXT2::superblock_constants::ext_major_ver);
	}
	
	bool EXT2::has_journal() const noexcept
	{
		if (extended_superblock())
		{
			return ((sb.optional_features & detail::EXT2::superblock_constants::optional_features::has_journal) != 0);
		}
		return false;
	}
	
	size_t EXT2::read_block(const size_t index, uint8_t* buffer) const noexcept
	{
		return disk->read(lba_from_block(index), block_size(), buffer);
	}
	
	size_t EXT2::write_block(const size_t index, const uint8_t* buffer) noexcept
	{
		return disk->write(lba_from_block(index), block_size(), buffer);
	}
	
	auto EXT2::reserve_inode(size_t& index) noexcept -> Utils::shared_ptr<inode_type>
	{
		for (auto group : groups)
		{
			auto node = group->reserve_node(index);
			if (node)
			{
				return node;
			}
		}
		return nullptr;
	}
	
	auto EXT2::reserve_block(size_t& index) noexcept -> Utils::shared_ptr<block_t>
	{
		for (auto group : groups)
		{
			auto block = group->reserve_block(index);
			if (block)
			{
				return block;
			}
		}
		return nullptr;
	}
	
	bool EXT2::release_inode(const size_t index) noexcept
	{
		assert(index > 0);
		
		auto gindex = (index - 1) % sb.nodes_per_group;
		assert(gindex < groups.size());
		
		return groups[gindex]->release_node(index);
	}
	
	bool EXT2::release_block(const size_t index) noexcept
	{
		auto gindex = (index - 1) % sb.blocks_per_group;
		assert(gindex < groups.size());
		
		return groups[gindex]->release_block(index);
	}
	
	size_t EXT2::release_inode_blocks(inode_type* n) noexcept
	{
		assert(n);
		if (unlikely(!n))
		{
			return 0;
		}
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	
	
	
	
	auto EXT2::__get_inode(inode_index_t i) const noexcept -> Utils::shared_ptr<inode_type>
	{
		assert(i > 0);
		auto group_n = calc_inode_group(i);
		assert(group_n < block_group_count);
		
		assert(bg_table);
		auto groupd = bg_table + group_n;
		assert(sb.nodes_per_group > 0);
		auto table_index = (i - 1) % sb.nodes_per_group;
		auto block_n = ((table_index * inode_size()) / block_size() + groupd->inode_table);

		table_index = table_index % inodes_per_block();
		
		auto sblock = get_block(block_n);
		assert(sblock);
		if (!sblock)
		{
			return nullptr;
		}
		
		auto block = const_cast<uint8_t*>(sblock->data);
		block += (table_index * inode_size());
		auto node = (detail::EXT2::inode_t*)block;
		return Utils::shared_ptr<inode_type>(sblock, node);
		//return (detail::EXT2::inode_t*)block;
	}
	
	size_t EXT2::lba_from_block(block_index_t n) const noexcept
	{
		if (n < sb.superblock_block_num)
		{
			TRACE_VAL(n);
			TRACE_VAL(sb.superblock_block_num);
		}
		assert(n >= sb.superblock_block_num);
		return (n - sb.superblock_block_num)*block_size() + EXT2_SUPERBLOCK_OFFSET;
	}
	
	size_t EXT2::dirent_name_len(const detail::EXT2::dirent_t* ent) const noexcept
	{
		if (sb.major_ver >= detail::EXT2::superblock_constants::ext_major_ver)
		{
			if (sb.required_features & detail::EXT2::superblock_constants::required_features::dir_entries_type_field)
			{
				return detail::EXT2::dirent_name_len(ent, true);
			}
		}
		return detail::EXT2::dirent_name_len(ent, false);
	}
	
	uint64_t EXT2::inode_size(const inode_type* n) const noexcept
	{
		assert(n);
		if (!n)
		{
			return 0;
		}
		
		if (extended_superblock())
		{
			if (n->type != EXT2_INODE_TYPE_DIR)
			{
				static_assert((((uint64_t)((uint32_t)1 << 31)) << 32) > 0);
				
				uint64_t sz = n->size_low;
				sz <<= 32;
				sz |= n->size_high;
				return sz;
				
			}
		}
		
		
		return n->size_low;
	}
	
	const uint8_t* EXT2::filesystem_ID() const noexcept
	{
		if (extended_superblock())
		{
			return sb.FS_ID;
		}
		else
		{
			return nullptr;
		}
	}
	
	const char* EXT2::volume_name() const noexcept
	{
		if (extended_superblock())
		{
			return (const char*)sb.volume_name;
		}
		else
		{
			return "";
		}
	}
	
	const char* EXT2::last_mount_path() const noexcept
	{
		if (extended_superblock())
		{
			return (const char*)sb.last_mount_path;
		}
		else
		{
			return "";
		}
	}
	
	
	
	
	bool EXT2::decode_device_signature(const uint32_t value, uint32_t* major, uint32_t* minor) noexcept
	{
		if (is_big_endian())
		{
			// TODO
			assert(NOT_IMPLEMENTED);
		}
		else
		{
			if (minor)
			{
				*minor = (value & 0xFF);
			}
			
			if (major)
			{
				*major = (value >> 8) & 0xFF;
			}
			
			return (((value >> 8) & 0xFF) != 0);
		}
	}
	
	bool EXT2::decode_device_signature(const uint32_t value, dev_t* dev) noexcept
	{
		if (dev)
		{
			return decode_device_signature(value, &dev->major, &dev->minor);
		}
		else
		{
			return decode_device_signature(value, nullptr, nullptr);
		}
	}
	
	uint32_t EXT2::encode_device_signature(const uint32_t major, const uint32_t minor) noexcept
	{
		if (is_big_endian())
		{
			// TODO
			assert(NOT_IMPLEMENTED);
		}
		else
		{
			uint32_t value = ((major & 0xFF) << 8);
			value |= (minor & 0xFF);
			return value;
		}
	}
	
	uint32_t EXT2::encode_device_signature(const dev_t dev) noexcept
	{
		return encode_device_signature(dev.major, dev.minor);
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	bool EXT2::Format(Drivers::Disk* part)
	{
		assert(part);
		if (!part)
		{
			return false;
		}
		
		
		using namespace detail::EXT2;
		
		const size_t sz = part->capacity();
		block_group_desc_t* bg_table = nullptr;
		superblock_ext_t sb;
		memset(&sb, 0, sizeof(sb));
		uint8_t* group_block_map_template = nullptr;
		int res;
		int used_count;
		size_t total_unalloc_blocks = 0;
		
		
		
		
		sb.su_rsv_blocks = 10;
		
		
		sb.superblock_block_num = 1;
		sb.block_size_modifier = 0;
		sb.fragment_size_modifier = 0;
		
		
		const size_t block_sz = (1024 << sb.block_size_modifier);
		const size_t node_sz = superblock_constants::node_struct_size;
		
		/*sb.blocks_per_group = 8192;
		sb.fragments_per_group = 8192;
		sb.nodes_per_group = 1024;*/
		sb.blocks_per_group = 4096;
		sb.fragments_per_group = 4096;
		sb.nodes_per_group = 512;
		
		sb.last_mount_time = sb.last_write_time = 0;
		sb.mounts_since_chk = 0;
		sb.mounts_before_chk = 20;
		sb.signature = superblock_constants::ext2_signature;
		sb.state = EXT2_STATE_CLEAN;
		
		
		
		sb.major_ver = 0;
		sb.usr_rsv_block_ID = 0;
		sb.group_rsv_block_ID = 0;
		
		
		
		const size_t group_sz = block_sz*sb.blocks_per_group;
		
		TRACE_VAL(sz);
		
		const size_t groups = sz / group_sz;
		
		TRACE_VAL(groups);
		
		
		uint32_t reserved_nodes = superblock_constants::first_non_rsv_node;
		
		assert(reserved_nodes >= 2);
		
		// Was index, make it a count
		reserved_nodes -= 1;
		
		
		sb.nodes = groups*sb.nodes_per_group;
		sb.unallocated_nodes = sb.nodes - reserved_nodes;
		sb.blocks = groups*sb.blocks_per_group;
		sb.unallocated_blocks = sb.blocks - 1 - (sb.superblock_block_num + 1);
		auto get_block_lba = [&](auto i) -> size_t
		{
			return (i - sb.superblock_block_num)*block_sz + EXT2_SUPERBLOCK_OFFSET;
		};
		
		
		
		
		size_t bgdt_off = (EXT2_SUPERBLOCK_OFFSET + sizeof(sb))/block_sz;
		if ((EXT2_SUPERBLOCK_OFFSET + sizeof(sb)) % block_sz)
		{
			++bgdt_off;
		}
		
		bgdt_off *= block_sz;
		
		
		size_t block_group_count = sb.blocks / sb.blocks_per_group;
		if (sb.blocks % sb.blocks_per_group)
		{
			++block_group_count;
		}
		
		
		{
			size_t groups_inodes = sb.nodes / sb.nodes_per_group;
			if (sb.nodes % sb.nodes_per_group)
			{
				++groups_inodes;
			}
			assert(block_group_count == groups_inodes);
		}
		
		bg_table = new block_group_desc_t[block_group_count];
		memset(bg_table, 0, block_group_count*sizeof(block_group_desc_t));
		
		
		
		assert(sb.blocks_per_group % 8 == 0);
		group_block_map_template = new uint8_t[sb.blocks_per_group/8];
		Utils::Bitset_Ptr<uint8_t> block_map(group_block_map_template, sb.blocks_per_group/8, sb.blocks_per_group);
		block_map.setAll(false);
		
		auto block_map_block_sz = DIVU((sb.blocks_per_group/8), block_sz);
		auto node_map_block_sz = DIVU((sb.nodes_per_group/8), block_sz);
		auto node_table_block_sz = DIVU((sb.nodes_per_group*node_sz),  block_sz);
		
		
		
		inode_t nodes_table[sb.nodes_per_group];
		assert(sizeof(nodes_table) == sb.nodes_per_group*node_sz);
		memset(nodes_table, 0, sizeof(nodes_table));
		assert(sizeof(nodes_table) == block_sz*node_table_block_sz);
		
		
		for (size_t i = 0; i < block_map_block_sz + node_map_block_sz + node_table_block_sz; ++i)
		{
			block_map.set(i, true);
		}
		
		for (size_t i = 1; i < block_group_count; ++i)
		{
			auto blk_start = i*sb.blocks_per_group + sb.superblock_block_num;
			bg_table[i].unallocated_inodes = sb.nodes_per_group;
			bg_table[i].unallocated_blocks = sb.blocks_per_group - block_map_block_sz - node_map_block_sz - node_table_block_sz;
			bg_table[i].block_usage_map = blk_start;
			bg_table[i].inode_usage_map = blk_start + block_map_block_sz;
			bg_table[i].inode_table = blk_start + block_map_block_sz + node_map_block_sz;
			
			
			res = part->write(get_block_lba(blk_start + 0), block_map_block_sz*block_sz, group_block_map_template);
			assert(res == block_map_block_sz*block_sz);
			if (res != block_map_block_sz*block_sz)
			{
				goto failure;
			}
			
			res = part->write(get_block_lba(bg_table[i].inode_usage_map), 0, node_map_block_sz*block_sz);
			assert(res == node_map_block_sz*block_sz);
			if (res != node_map_block_sz*block_sz)
			{
				goto failure;
			}
			
			res = part->write(get_block_lba(bg_table[i].inode_table), 0, node_table_block_sz*block_sz);
			assert(res == node_table_block_sz*block_sz);
			if (res != node_table_block_sz*block_sz)
			{
				goto failure;
			}
			
			total_unalloc_blocks += bg_table[i].unallocated_blocks;
		}
		
		
		
		
		
		// Set up first block group
		
		
		
		block_map.setAll(false);
		used_count = 0;
		
		
		// Reserve boot sector(s) and
		// superblock
		for (int i = 0; i <= sb.superblock_block_num; ++i)
		{
			block_map.set(used_count++, true);
		}
		
		for (int i = 0; i < DIVU(block_group_count*sizeof(block_group_desc_t), block_sz); ++i)
		{
			block_map.set(used_count++, true);
		}
		
		bg_table[0].block_usage_map = used_count;
		
		for (int i = 0; i < block_map_block_sz; ++i)
		{
			block_map.set(used_count++, true);
		}
		
		bg_table[0].inode_usage_map = used_count;
		
		for (int i = 0; i < node_map_block_sz; ++i)
		{
			block_map.set(used_count++, true);
		}
		
		bg_table[0].inode_table = used_count;
		
		for (int i = 0; i < node_table_block_sz; ++i)
		{
			block_map.set(used_count++, true);
		}
		
		
		
		// Reserve block for root
		// directory
		// entries ('.' and '..')
		block_map.set(used_count, true);
		TRACE("Block_map reserved at:");
		TRACE_VAL(used_count);
		
		
		bg_table[0].unallocated_blocks = sb.blocks_per_group - used_count;
		bg_table[0].unallocated_inodes = sb.nodes_per_group - 1;
		bg_table[0].directories = 1;
		
		
		{
			uint8_t val = 1;
			val <<= 6;
			assert(val > 0);
			assert((uint8_t)(val << 2) == 0);
			
			res = part->write(get_block_lba(bg_table[0].inode_usage_map), 0, node_map_block_sz*block_sz);
			assert(res == node_map_block_sz*block_sz);
			if (res != node_map_block_sz*block_sz)
			{
				goto failure;
			}
			
			{
				size_t reserved_bytes = (reserved_nodes / 8) + ((reserved_nodes % 8) ? 1 : 0);
				uint8_t reserved_map[reserved_bytes];
				memset(reserved_map, 0, reserved_bytes);
				for (size_t i = 0; i < reserved_nodes; ++i)
				{
					val = 1;
					val <<= (i % 8);
					reserved_map[i / 8] |= val;
				}
				
				res = part->write(get_block_lba(bg_table[0].inode_usage_map), reserved_bytes, reserved_map);
				assert(res == reserved_bytes);
				if (res != reserved_bytes)
				{
					goto failure;
				}
			}
			
			
			/*res = part->write(get_block_lba(bg_table[0].inode_usage_map), val, 1);
			assert(res == 1);
			if (res != 1)
			{
				goto failure;
			}*/
		}
		
		
		res = part->write(get_block_lba(bg_table[0].block_usage_map), block_map_block_sz*block_sz, group_block_map_template);
		assert(res == block_map_block_sz*block_sz);
		if (res != block_map_block_sz*block_sz)
		{
			goto failure;
		}
		
		
		// Setup root directory node
		memset(nodes_table, 0, sizeof(nodes_table));
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Woverflow"
		nodes_table[1].raw_perms = 0xFFFF;
		#pragma GCC diagnostic pop
		nodes_table[1].perms.set_UID = 0;
		nodes_table[1].perms.set_GID = 0;
		nodes_table[1].perms.sticky = 0;
		nodes_table[1].perms.group_write = 0;
		nodes_table[1].perms.other_write = 0;
		nodes_table[1].type = EXT2_INODE_TYPE_DIR;
		
		assert(block_sz >= 512);
		nodes_table[1].sector_count = block_sz/512;
		nodes_table[1].size_low = block_sz;
		nodes_table[1].direct[0] = used_count;
		
		
		{
			auto dirents_raw = new uint8_t[block_sz];
			memset(dirents_raw, 0, block_sz);
			auto dirent = (dirent_t*)dirents_raw;
			dirent->inode = 2;
			dirent->entry_size = 12;
			dirent->name_len_lsb = 1;
			dirent->name_len_msb = 0;
			dirent->name[0] = '.';
			dirent->name[1] = 0;
			dirent->name[2] = 0;
			dirent->name[3] = 0;
			
			dirent = (dirent_t*)(dirents_raw+12);
			dirent->inode = 2;
			dirent->entry_size = block_sz - 12;
			dirent->name_len_lsb = 2;
			dirent->name_len_msb = 0;
			dirent->name[0] = '.';
			dirent->name[1] = '.';
			dirent->name[2] = 0;
			dirent->name[3] = 0;
			
			res = part->write(get_block_lba(nodes_table[1].direct[0]), block_sz, dirents_raw);
			delete[] dirents_raw;
			assert(res == block_sz);
			if (res != block_sz)
			{
				goto failure;
			}
		}
		
		
		
		res = part->write(get_block_lba(bg_table[0].inode_table), node_table_block_sz*block_sz, (const uint8_t*)nodes_table);
		assert(res == node_table_block_sz*block_sz);
		if (res != node_table_block_sz*block_sz)
		{
			goto failure;
		}
		
		total_unalloc_blocks += bg_table[0].unallocated_blocks;
		
		
		
		
		// Recalculate unallocated_blocks
		// on the superblock
		sb.unallocated_blocks = total_unalloc_blocks;
		res = part->write(EXT2_SUPERBLOCK_OFFSET, sizeof(sb), (const uint8_t*)&sb);
		assert(res == sizeof(sb));
		if (res != sizeof(sb))
		{
			goto failure;
		}
		
		res = part->write(bgdt_off, block_group_count*sizeof(block_group_desc_t), (const uint8_t*)bg_table);
		assert(res == block_group_count*sizeof(block_group_desc_t));
		
		// TODO: Figure out if I'm doing
		// this right
		
		
		
		delete[] bg_table;
		delete[] group_block_map_template;
		
		
		return true;
		
		
		failure:
		if (bg_table)
		{
			delete[] bg_table;
		}
		if (group_block_map_template)
		{
			delete[] group_block_map_template;
		}
		return false;
	}
	
	
	
	
	
	
}


void Kernel::FS::detail::EXT2::block_t::operator delete(void* blk)
{
	delete[] (uint8_t*)blk;
}