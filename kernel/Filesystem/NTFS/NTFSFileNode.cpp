#include "NTFSFileNode.hh"
#include "NTFS.hh"

namespace Kernel::FS
{
	NTFSFileNode::NTFSFileNode(NTFS* ntfs, ptr<ntfs_mft_record_t> mrec, ptr<ntfs_stdinfo_t> info) : NTFSNode(ntfs, mrec, info), FileNode()
	{
		if (mrec)
		{
			this->_name = get_name();
		}
	}
	
	
	
	uint64_t NTFSFileNode::read(uint64_t start, uint64_t len, uint8_t* buf)
	{
		return n_read(start, len, buf);
	}
	
	uint64_t NTFSFileNode::write(uint64_t start, uint64_t len, const uint8_t* buf)
	{
		return n_write(start, len, buf);
	}
	
	void NTFSFileNode::open()
	{
		if (!_data)
			init_data();
	}
	
	void NTFSFileNode::close()
	{
		TRACE("NOT YET IMPLEMENTED");
	}
	
	size_t NTFSFileNode::size() const noexcept
	{
		return this->n_size();
	}
	
	DirectoryNode_v* NTFSFileNode::get_parent() const noexcept
	{
		auto ntfs_n = this->open_parent();
		if (ntfs_n)
		{
			auto n = ntfs_n->get_inode();
			if (n)
			{
				return n->as_directory();
			}
		}
		return nullptr;
	}
	
	Filesystem* NTFSFileNode::get_filesystem() const noexcept
	{
		return volume();
	}
}