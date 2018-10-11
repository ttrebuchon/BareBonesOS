#include "EXT2DirectoryNode.hh"
#include "EXT2FileNode.hh"
#include "EXT2SymLinkNode.hh"

namespace Kernel::FS
{
	
	EXT2DirectoryNode::EXT2DirectoryNode(DirectoryNode* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index) : EXT2Node(fs, node, name, inode_index), DirectoryNode()
	{
		init(parent);
	}
	
	EXT2DirectoryNode::EXT2DirectoryNode(DirectoryNode* parent, EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, ent), DirectoryNode()
	{
		init(parent);
	}
	
	void EXT2DirectoryNode::init(DirectoryNode* parent)
	{
		this->_name = this->EXT2Node::inode_name;
		
		has_read = has_pending = false;
		this->_parent = parent;
		
	}
	
	bool EXT2DirectoryNode::initial_read()
	{
		assert(children.size() == 0);
		assert(!has_read);
		assert(!has_pending);
		
		auto blk_sz = fs->block_size();
		auto blks = node->size_low;
		if (blks % blk_sz)
		{
			blks = blks / blk_sz + 1;
		}
		else
		{
			blks = blks / blk_sz;
		}
		
		
		auto data = fs->read_inode(this->node, 0, blks);
		if (!data)
		{
			return (this->node->direct[0] == 0);
		}
		
		using namespace detail::EXT2;
		
		size_t data_sz = blk_sz * blks;
		
		auto ents = (dirent_t*)data;
		
		auto current = next_dirent(ents, data_sz, nullptr);
		
		auto parent = get_parent();
		
		while (current)
		{
			auto n = fs->parse_node(this, current);
			if (n && n != this && n != parent)
			{
				children.push_back(n);
			}
			
			
			
			/*auto next_n = fs->get_inode(current->inode);
			assert(next_n);
			if (!next_n)
			{
				delete[] data;
				return false;
			}
			
			
			
			if (next_n->type_perms.type == EXT2_INODE_TYPE_DIR)
			{
				children.push_back(new EXT2DirectoryNode(fs, next_n, (const char*)current->name));
			}
			else if (next_n->type_perms.type == EXT2_INODE_TYPE_BLOCK_DEV)
			{
				assert(NOT_IMPLEMENTED);
			}
			else if (next_n->type_perms.type == EXT2_INODE_TYPE_FILE)
			{
				children.push_back(new EXT2FileNode(fs, next_n, (const char*)current->name));
			}
			else if (next_n->type_perms.type == EXT2_INODE_TYPE_SYMLINK)
			{
				//TRACE((const char*)current->name);
				//assert(NOT_IMPLEMENTED);
			}
			else
			{
				TRACE(next_n->type_perms.type);
				assert(NOT_IMPLEMENTED);
			}*/
			
			current = next_dirent(ents, data_sz, current);
		}
		
		delete[] data;
		
		has_read = true;
		
		#ifdef DEBUG
		for (auto child : children)
		{
			assert(child);
			assert(((DirectoryNode_v*)this)->at(child->name) == child);
		}
		#endif
		
		return true;
	}
	
	
	node_ptr<> EXT2DirectoryNode::add(const node_ptr<>& n)
	{
		assert(n);
		if (unlikely(!has_read))
		{
			if (!this->initial_read())
			{
				// TODO: Handle error
				assert(false);
			}
		}
		
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			if (*it == n)
			{
				return n;
			}
		}
		
		children.push_back(n);
		if (!n->isKind(NodeType::RAMFile))
		{
			// TODO
		}
		return n;
		assert(NOT_IMPLEMENTED);
	}
	
	bool EXT2DirectoryNode::remove(const node_ptr<>& n)
	{
		using namespace detail::EXT2;
		assert(n);
		if (unlikely(!n))
		{
			return false;
		}
		
		auto it = children.begin();
    	for (; it != children.end(); ++it)
    	{
    		if (*it == n)
    		{
    			break;
    		}
    	}
    	
    	if (it == children.end())
    	{
    		// Node isn't a child
    		return false;
    	}
		
		if (n->get_filesystem() == this->get_filesystem())
		{
			EXT2Node* ex_node = nullptr;
			
			{
				if (n->isKind(NodeType::Link))
				{
					 ex_node = (EXT2SymLinkNode*)(n.as_link().get());
					 goto found_ex_node;
				}
			}
			
			{
				
				auto type = n->type();
				type &= (NodeType::Directory | NodeType::File | NodeType::Link | NodeType::Block | NodeType::Char | NodeType::Pipe | NodeType::Function);
				
				
				
				switch (type)
				{
					case NodeType::Directory:
					ex_node = (EXT2DirectoryNode*)n.get()->as_directory();
					break;
					
					case NodeType::File:
					ex_node = (EXT2FileNode*)n.get()->as_file();
					break;
					
					
				}
			}
			
			found_ex_node:
			
			assert(ex_node);
			
			auto n_inode = ex_node->get_ext2_inode();
			auto inode = this->inode_index;
			Utils::shared_ptr<detail::EXT2::block_t> dirent_block;
			dirent_t* dir_ent = nullptr;
			const size_t block_size = fs->block_size();
			
			for (size_t i = 0; i < this->block_count(); ++i)
			{
				auto blk = this->get_block(i, false);
				assert(blk);
				
				dirent_t* ent = (dirent_t*)blk->data;
				while (ent)
				{
					if (ent->inode == n_inode)
					{
						dirent_block = blk;
						dir_ent = ent;
						goto found_ent;
					}
					else
					{
						ent = next_dirent((dirent_t*)blk->data, block_size, ent);
					}
				}
			}
			
			found_ent:
			assert(dir_ent);
			assert(dirent_block);
			
			auto ent_len = dir_ent->entry_size;
			
			memset(dir_ent, 0, ent_len);
			dir_ent->entry_size = ent_len;
			dirent_block->modified = true;
			
			if (!fs->release_node(n))
			{
				assert(false);
			}
			
			/*if (ex_node->node->hard_links <= 1)
			{
				if (ex_node->node->hard_links == 1)
				{
					ex_node->node->hard_links = 0;
				}
				
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			else
			{
				--ex_node->node->hard_links;
			}*/
		}
		else if (!n->isKind(NodeType::RAMFile))
		{
			// Not a RAM file and not
			// part of this filesystem
			
			// This really shouldn't
			// be able to happen
			assert(NOT_IMPLEMENTED);
		}
		
		
		children.erase(it);
		return true;
	}
	
	size_t EXT2DirectoryNode::size() const noexcept
	{
		if (unlikely(!has_read))
		{
			auto mut_this = const_cast<EXT2DirectoryNode*>(this);
			if (!mut_this->initial_read())
			{
				// TODO: Handle error
				assert(false);
			}
		}
		
		return children.size();
	}
	
	node_ptr<> EXT2DirectoryNode::at(size_t index) const
	{
		if (unlikely(!has_read))
		{
			auto mut_this = const_cast<EXT2DirectoryNode*>(this);
			if (!mut_this->initial_read())
			{
				// TODO: Handle error
				assert(false);
			}
		}
		
		return children.at(index);
	}
	
		
	uint64_t EXT2DirectoryNode::read(uint64_t start, uint64_t len, void* buf)
	{
		return EXT2Node::read(start, len, buf);
	}
	
	uint64_t EXT2DirectoryNode::write(uint64_t start, uint64_t len, const void* buf)
	{
		return EXT2Node::write(start, len, buf);
	}
	
	void EXT2DirectoryNode::open()
	{
		EXT2Node::open();
	}
	
	void EXT2DirectoryNode::close()
	{
		EXT2Node::close();
	}
	
	DirEnt* EXT2DirectoryNode::readDir(uint32_t)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	node_ptr<> EXT2DirectoryNode::findDir(const char* name)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	EXT2* EXT2DirectoryNode::get_filesystem() const noexcept
	{
		return this->EXT2Node::fs;
	}
	
	
}