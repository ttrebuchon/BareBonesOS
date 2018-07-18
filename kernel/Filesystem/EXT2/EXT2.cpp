#include "EXT2.hh"
#include <drivers/Disk/Disk.hh>
#include "EXT2DirectoryNode.hh"
#include "EXT2FileNode.hh"
#include "EXT2SymLinkNode.hh"
#include <kernel/Filesystem/BlockDeviceNode.hh>
#include <kernel/Filesystem/MBR.hh>
#include <kernel/Filesystem/MBR_System_IDs.h>

#define EXT2_SUPERBLOCK_OFFSET 1024
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
		dirent_t* next_dirent(dirent_t* array, size_t array_size, dirent_t* current)
		{
			assert(array);
			assert(array_size);
			if (!current)
			{
				current = array;
			}
			else
			{
				current = (dirent_t*)((uint8_t*)current + current->entry_size);
			}
			while (!current->inode)
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
		
		
		
	}
	
	
	
	
	
	
	
	EXT2::EXT2(Drivers::Disk& disk, int partition) noexcept : Filesystem(), disk(&disk)
	{
		assert(!is_big_endian());
		
		if (partition >= 0)
		{
			assert(partition < 4);
			MBR_t mbr;
			auto mbr_res = disk.read(MBR_DISK_OFFSET, sizeof(MBR_t), (uint8_t*)&mbr);
			assert(mbr_res == sizeof(MBR_t));
			
			
			auto sys = mbr.partitions[partition].system_ID;
			TRACE((int)sys);
			assert(sys == MBR_SYS_EXT);
			
			
			// TODO
			assert(NOT_IMPLEMENTED);
			
		}
		
		
		auto res = disk.read(EXT2_SUPERBLOCK_OFFSET, sizeof(sb), (uint8_t*)&sb);
		assert(res == sizeof(sb));
		
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
		TRACE(sb.blocks);
		TRACE(sb.blocks_per_group);
		TRACE("");
		TRACE(sb.superblock_block_num);
		
		
		
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
		
		
		
		auto root = get_inode(2);
		assert(root);
		_root = new EXT2DirectoryNode(nullptr, this, root, "");
		nodes[2] = _root;
		/*
		size_t root_sz = root->size_low;
		auto dat = read_inode(root, 0, root_sz/block_size());
		root_sz = (root_sz / block_size());
		root_sz *= block_size();
		
		assert(dat);
		
		auto root_ents = (dirent_t*)dat;
		TRACE("\n\n\n\n");
		
		
		
		auto ent = next_dirent(root_ents, root_sz, nullptr);
		while (ent)
		{
			assert(ent);
			
			auto n = get_inode(ent->inode);
			
			
			TRACE(ent->name);
			TRACE(ent->inode);
			assert((uint8_t*)ent < ((uint8_t*)root_ents + root_sz));
			ent = next_dirent(root_ents, root_sz, ent);
		}
		
		delete[] dat;*/
	}
	
	EXT2::~EXT2()
	{
		assert(NOT_IMPLEMENTED);
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
		if (cached_blocks.count(n))
		{
			auto cached = cached_blocks.at(n).lock();
			
			if (cached)
			{
				return cached;
			}
		}
		
		static_assert(sizeof(block_t) == sizeof(size_t));
		auto raw = new uint8_t[sizeof(block_t) + block_size()];
		memset(raw, 0, sizeof(block_t) + block_size());
		
		block_t* block = (block_t*)raw;
		
		block->len = block_size();
		
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
		return ptr;
	}
	
	auto EXT2::get_block(block_index_type n) const -> Utils::shared_ptr<const block_t>
	{
		return const_cast<EXT2*>(this)->get_block(n);
		if (cached_blocks.count(n))
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
		return ptr;
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
		
		if (next_n->type_perms.type == EXT2_INODE_TYPE_DIR)
		{
			n = new EXT2DirectoryNode(parent, this, next_n, Utils::string((const char*)ent->name, dirent_name_len(ent)));
		}
		else if (next_n->type_perms.type == EXT2_INODE_TYPE_BLOCK_DEV)
		{
			n = new BlockDeviceNode(Utils::string((const char*)ent->name, dirent_name_len(ent)));
		}
		else if (next_n->type_perms.type == EXT2_INODE_TYPE_CHAR_DEV)
		{
			// TODO: Write an actual character device node
			n = new BlockDeviceNode(nname);
			/*TRACE(nname.c_str());
			assert(NOT_IMPLEMENTED);*/
		}
		else if (next_n->type_perms.type == EXT2_INODE_TYPE_FILE)
		{
			n = new EXT2FileNode(parent, this, next_n, Utils::string((const char*)ent->name, dirent_name_len(ent)));
		}
		else if (next_n->type_perms.type == EXT2_INODE_TYPE_SYMLINK)
		{
			
			n = new EXT2SymLinkNode(parent, this, next_n, Utils::string((const char*)ent->name, dirent_name_len(ent)));
		}
		else
		{
			TRACE((const char*)ent->name);
			TRACE(next_n->type_perms.type);
			assert(NOT_IMPLEMENTED);
		}
		
		//assert(n);
		if (n)
		{
			n->set_parent(parent);
		}
		nodes[ent->inode] = n;
		return n;
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
		assert(n >= sb.superblock_block_num);
		return (n - sb.superblock_block_num)*block_size() + EXT2_SUPERBLOCK_OFFSET;
	}
	
	size_t EXT2::dirent_name_len(const detail::EXT2::dirent_t* ent) const noexcept
	{
		assert(ent);
		if (unlikely(!ent))
		{
			return 0;
		}
		
		if (sb.major_ver >= detail::EXT2::superblock_constants::ext_major_ver)
		{
			if (sb.required_features & detail::EXT2::superblock_constants::required_features::dir_entries_type_field)
			{
				return ent->name_len_lsb;
			}
		}
		
		uint16_t len = ent->name_len_msb;
		len = len << 8;
		len |= ent->name_len_lsb;
		return len;
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
			if (n->type_perms.type != EXT2_INODE_TYPE_DIR)
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
	
	
	
	
	
	
	
}


void Kernel::FS::detail::EXT2::block_t::operator delete(void* blk)
{
	delete[] (uint8_t*)blk;
}