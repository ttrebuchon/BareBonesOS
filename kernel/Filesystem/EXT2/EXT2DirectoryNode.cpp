#include "EXT2DirectoryNode.hh"
#include "EXT2FileNode.hh"

namespace Kernel::FS
{
	
	EXT2DirectoryNode::EXT2DirectoryNode(DirectoryNode* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name) : EXT2Node(fs, node, name), Node(NodeType::Directory), DirectoryNode()
	{
		init(parent);
	}
	
	EXT2DirectoryNode::EXT2DirectoryNode(DirectoryNode* parent, EXT2* fs, detail::EXT2::dirent_t* ent) : EXT2Node(fs, ent), Node(NodeType::Directory), DirectoryNode()
	{
		init(parent);
	}
	
	void EXT2DirectoryNode::init(DirectoryNode* parent)
	{
		this->_name = this->EXT2Node::inode_name;
		
		has_read = has_pending = false;
		this->parent = parent;
		
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
		assert(data);
		if (!data)
		{
			return false;
		}
		
		using namespace detail::EXT2;
		
		size_t data_sz = blk_sz * blks;
		
		auto ents = (dirent_t*)data;
		
		auto current = next_dirent(ents, data_sz, nullptr);
		
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
		
		
		return true;
	}
	
	
	void EXT2DirectoryNode::addChild(Node*)
	{
		assert(NOT_IMPLEMENTED);
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
	
	Node* EXT2DirectoryNode::at(size_t index) const
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
	
		
	uint64_t EXT2DirectoryNode::read(uint64_t start, uint64_t len, uint8_t* buf)
	{
		return EXT2Node::read(start, len, buf);
	}
	
	uint64_t EXT2DirectoryNode::write(uint64_t start, uint64_t len, const uint8_t* buf)
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
	
	Node* EXT2DirectoryNode::findDir(const char* name)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	
}