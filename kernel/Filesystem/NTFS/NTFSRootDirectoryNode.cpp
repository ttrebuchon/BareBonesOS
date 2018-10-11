#include "NTFSRootDirectoryNode.hh"

namespace Kernel::FS
{
	NTFSRootDirectoryNode::NTFSRootDirectoryNode(NTFS* ntfs, ptr<ntfs_mft_record_t> mrec, ptr<ntfs_stdinfo_t> info) :
		NTFSDirectoryNode(ntfs, mrec, info),
		MFT(nullptr),
		MFT_m(nullptr),
		upCase(nullptr),
		lowCase(nullptr),
		attrDefs(nullptr),
		logFile(nullptr),
		_volume(nullptr),
		bitmap(nullptr),
		boot(nullptr),
		secure(nullptr),
		extend(nullptr),
		badClus(nullptr)
	{
		
	}
	
	
	bool NTFSRootDirectoryNode::init()
	{
		if (NTFSDirectoryNode::init())
		{
			//assert(children.size() >= sys_node_count);
			if (children.size() < sys_node_count)
			{
				assert(NOT_IMPLEMENTED);
			}
			
			auto is_sys = [&](auto node)
			{
				if (node->isKind(NodeType::FSSpecific, true))
				{
					NTFSNode* n = nullptr;
					#ifdef __cpp_rtti
					n = dynamic_cast<NTFSNode*>(node.get());
					if (n)
					{
					#else
					n = (NTFSSystemNode*)node.get();
					#endif
					
					if (n->volume() == this->volume())
					{
						if (n->mft_index() < sys_node_count)
						{
							return true;
						}
					}
					
					#ifdef __cpp_rtti
					}
					#else
					
					#endif
				}
				
				return false;
			};
			
			for (size_t i = 0; i < children.size(); ++i)
			{
				if (is_sys(children[i]) && i >= sys_node_count)
				{
					for (size_t j = 0; j < sys_node_count; ++j)
					{
						if (!is_sys(children[j]))
						{
							auto tmp = children[j];
							children[j] = children[i];
							children[i] = tmp;
							TRACE_VAL(i);
							TRACE_VAL(j);
							break;
						}
					}
				}
			}
			
			
			return true;
		}
		else
		{
			return false;
		}
	}
	
	
	
	size_t NTFSRootDirectoryNode::size() const noexcept
	{
		return NTFSDirectoryNode::size() - sys_node_count;
	}
	
	node_ptr<> NTFSRootDirectoryNode::at(size_t index) const
	{
		return NTFSDirectoryNode::at(index+sys_node_count);
	}
	
	DirectoryNode_v* NTFSRootDirectoryNode::get_parent() const noexcept
	{
		DirectoryNode_v* p;
		if ((p = Node::get_parent()))
		{
			return p;
		}
		else
		{
			auto ntfs_n = this->open_parent();
			if (unlikely(ntfs_n && ntfs_n != this))
			{
				auto n = ntfs_n->get_inode();
				if (n)
				{
					return n->as_directory();
				}
			}
			return nullptr;
		}
	}
	
	Path_t NTFSRootDirectoryNode::get_path() const
	{
		auto parent = get_parent();
		Path_t p;
		assert(parent != this);
		if (parent && parent != this)
		{
			p = parent->get_path();
		}
		return p;
	}
	
}