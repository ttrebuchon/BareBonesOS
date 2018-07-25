#include "EXT2Factory.hh"
#include "EXT2.hh"
#include "EXT2DirectoryNode.hh"
#include "EXT2FileNode.hh"

#define DIVU(X, Y) (((X) / (Y)) + (unlikely(((X) % (Y) == 0)) ? 0 : 1))

namespace Kernel::FS
{
	using namespace detail::EXT2;
	
	
	static dirent_t* find_dirent_hole(EXT2*, Utils::shared_ptr<block_t> block, const size_t name_len, const size_t block_size) noexcept;
	
	
	
	
	EXT2Factory::EXT2Factory(EXT2* fs) : ext2(fs)
	{
		assert(fs);
	}
	
	
	
	DirectoryNode_v* EXT2Factory::create_directory(DirectoryNode_v* parent, const Utils::string& name) noexcept
	{
		assert(parent);
		if (!parent)
		{
			return nullptr;
		}
		#ifdef __cpp_rtti
		auto ext2_parent = dynamic_cast<EXT2DirectoryNode*>(parent);
		assert(ext2_parent);
		if (!ext2_parent)
		{
			return nullptr;
		}
		#else
		auto ext2_parent = (EXT2DirectoryNode*)parent;
		#endif
		
		
		
		
		Utils::shared_ptr<inode_type> inode = nullptr;
		size_t n_index;
		for (size_t i = 0; i < ext2->group_count(); ++i)
		{
			auto gr = ext2->get_group(i);
			assert(gr);
			if (!gr)
			{
				continue;
			}
			
			inode = gr->allocate_directory(n_index);
			if (inode)
			{
				assert(n_index > 2);
				assert(n_index != ext2_parent->inode_index);
				break;
			}
		}
		
		if (!inode)
		{
			return nullptr;
		}
		
		assert(n_index != ext2_parent->inode_index);
		
		auto ent = add_child_inode(ext2_parent, n_index, name);
		if (!ent)
		{
			ext2->release_inode(n_index);
			inode = nullptr;
			return nullptr;
		}
		
		auto node = ext2->parse_node(ext2_parent, ent.get());
		assert(node);
		assert(node->isKind(NodeType::Directory));
		auto dir_node = node->as_directory();
		assert(dir_node);
		if (ext2_parent->has_read)
		{
			ext2_parent->children.push_back(dir_node);
		}
		return dir_node;
	}
	
	FileNode_v* EXT2Factory::create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept
	{
		assert(parent);
		if (!parent)
		{
			return nullptr;
		}
		#ifdef __cpp_rtti
		auto ext2_parent = dynamic_cast<EXT2DirectoryNode*>(parent);
		assert(ext2_parent);
		if (!ext2_parent)
		{
			return nullptr;
		}
		#else
		auto ext2_parent = (EXT2DirectoryNode*)parent;
		#endif
		
		
		
		
		Utils::shared_ptr<inode_type> inode = nullptr;
		size_t n_index;
		for (size_t i = 0; i < ext2->group_count(); ++i)
		{
			auto gr = ext2->get_group(i);
			assert(gr);
			if (!gr)
			{
				continue;
			}
			
			inode = gr->allocate_file(n_index);
			if (inode)
			{
				assert(n_index > 2);
				assert(n_index != ext2_parent->inode_index);
				break;
			}
		}
		
		if (!inode)
		{
			return nullptr;
		}
		
		assert(n_index != ext2_parent->inode_index);
		
		auto ent = add_child_inode(ext2_parent, n_index, name);
		if (!ent)
		{
			ext2->release_inode(n_index);
			inode = nullptr;
			return nullptr;
		}
		
		auto node = ext2->parse_node(ext2_parent, ent.get());
		assert(node);
		assert(node->isKind(NodeType::File));
		auto file_node = node->as_file();
		assert(file_node);
		if (ext2_parent->has_read)
		{
			ext2_parent->children.push_back(file_node);
		}
		return file_node;
	}
	
	LinkNode* EXT2Factory::create_link(DirectoryNode_v* parent, const Utils::string&, Node* target) noexcept
	{
		assert(NOT_IMPLEMENTED);
	}
	
	
	Utils::shared_ptr<dirent_t> EXT2Factory::add_child_inode(directory_type* ext2_parent, const size_t n_index, const Utils::string& name) const noexcept
	{
		assert(n_index > 2);
		auto parent_sz = ext2_parent->node_size();
		
		// TODO: Setup lock system for concurrency
		
		using namespace detail::EXT2;
		Utils::shared_ptr<block_t> block = nullptr;
		size_t block_index;
		auto name_len = name.length();
		dirent_t* dirent = nullptr;
		for (block_index = 0; block_index < ext2_parent->block_count(); ++block_index)
		{
			block = ext2_parent->get_block(block_index, false);
			assert(block);
			dirent = allocate_dirent((dirent_t*)block->data, ext2->block_size(), name_len, false);
			if (dirent)
			{
				break;
			}
		}
		
		if (!dirent)
		{
			block = ext2->reserve_block(block_index);
			assert(block->index == block_index);
			
			if (!block)
			{
				return nullptr;
			}
			else
			{
				memset(block->data, 0, ext2->block_size());
				dirent = (dirent_t*)block->data;
				
				size_t tmp;
				if (!ext2_parent->add_block(block_index, &tmp))
				{
					ext2->release_block(block_index);
					return nullptr;
				}
				block_index = tmp;
				assert(block_index == 0);
				parent_sz = ext2_parent->node_size();
				dirent->entry_size = ext2->block_size();
			}
		}
		
		
		assert(dirent->inode == 0);
		
		{
			// Make sure the relevant
			// block is marked as written
			auto block2 = ext2_parent->get_block(block_index, true);
			volatile auto vblock2 = block2.get();
			assert(vblock2 == block.get());
			assert(block == block2);
			assert(ext2_parent->group()->block_is_modified(block->index));
		}
		
		assert(dirent);
		memcpy(dirent->name, name.c_str(), name_len);
		dirent->name_len_lsb = name_len;
		assert(size_t(dirent->name_len_lsb) == name_len);
		dirent->inode = n_index;
		ext2_parent->group()->mark_node_modified(ext2_parent->inode_index);
		
		
		
		return Utils::shared_ptr<dirent_t>(block, dirent);
	}
	
	
	
	
	
	
	
	
	// Protected internal methods
	
	
	
	
	
	
	
	// Static Functions
	
	static dirent_t* find_dirent_hole(EXT2* ext2, Utils::shared_ptr<block_t> block, const size_t name_len, const size_t block_size) noexcept
	{
		auto ents = (dirent_t*)block->data;
		dirent_t* prev = nullptr;
		dirent_t* ent = nullptr;
		while ((ent = detail::EXT2::next_dirent(ents, block_size, ent, true)))
		{
			if (ent->inode == 0)
			{
				if (ent->entry_size == 0)
				{
					return nullptr;
					if ((block_size - ((uint8_t*)ent - block->data)) >= sizeof(dirent_t) + name_len)
					{
						return ent;
					}
				}
				else if (ent->entry_size >= (sizeof(dirent_t) + name_len))
				{
					return ent;
				}
			}
			prev = ent;
		}
		
		if (prev)
		{
			auto expected = ext2->dirent_name_len(prev) + sizeof(dirent_t);
			expected = DIVU(expected, 4)*4;
			if (prev->entry_size > expected)
			{
				if (prev->entry_size - expected >= sizeof(dirent_t) + name_len)
				{
					ent = (dirent_t*)((uint8_t*)prev + expected);
					prev->entry_size = expected;
					ent->entry_size = block_size - ((uint8_t*)ent - (uint8_t*)ents);
					ent->inode = 0;
					ent->name_len_lsb = 0;
					ent->name_len_msb = 0;
					return ent;
				}
			}
		}
		
		return nullptr;
	}
	
	
	namespace detail::EXT2
	{
		dirent_t* allocate_dirent(dirent_t* array, size_t array_size, size_t name_len, bool type_field)
		{
			assert(array);
			assert(array_size > 0);
			
			if (sizeof(dirent_t) + name_len > array_size)
			{
				return nullptr;
			}
			
			auto needed = DIVU(sizeof(dirent_t) + name_len, 4)*4;
			
			dirent_t* prev = nullptr;
			dirent_t* it = array;
			do
			{
				assert(it->entry_size);
				if (it->inode)
				{
					size_t expected = DIVU(sizeof(dirent_t) + it->name_len_lsb, 4)*4;
					assert(it->entry_size >= expected);
					if (expected + needed <= it->entry_size)
					{
						static_assert(sizeof(dirent_t) % 4 == 0);
						auto next = (dirent_t*)((uint8_t*)it + expected);
						next->inode = 0;
						next->entry_size = it->entry_size - expected;
						next->name_len_lsb = name_len;
						next->name_len_msb = 0;
						it->entry_size = expected;
						return next;
					}
				}
				else
				{
					if (it->entry_size >= needed)
					{
						static_assert(sizeof(dirent_t) % 4 == 0);
						if (it->entry_size - needed >= sizeof(dirent_t))
						{
							auto next = (dirent_t*)((uint8_t*)it + needed);
							next->inode = 0;
							next->entry_size = it->entry_size - needed;
							next->name_len_lsb = 0;
							next->name_len_msb = 0;
							it->entry_size = needed;
							
						}
						it->name_len_lsb = name_len;
						return it;
					}
				}
				
				it = (dirent_t*)((uint8_t*)it + it->entry_size);
			}
			while (it && (uint8_t*)it - (uint8_t*)array < array_size - name_len - sizeof(dirent_t));
			
			return nullptr;
		}
	}
}