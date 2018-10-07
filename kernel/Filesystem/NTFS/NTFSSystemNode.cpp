#include "NTFSSystemNode.hh"
#include "NTFS.hh"
#include <Std/errno.h>

namespace Kernel::FS
{
	
	NTFSSystemNode::NTFSSystemNode(NTFS* ntfs, ptr<ntfs_mft_record_t> mrec, ptr<ntfs_stdinfo_t> info) : NTFSNode(ntfs, mrec, info), Node(NodeType::FSSpecific)
	{
		if (mft_rec)
		{
			this->_name = this->get_name();
		}
	}
	
	
	
	uint64_t NTFSSystemNode::read(uint64_t start, uint64_t len, void*)
	{
		tset_error(EOPNOTSUPP, "Cannot raw read from NTFS system node.");
		return 0;
	}
	
	uint64_t NTFSSystemNode::write(uint64_t, uint64_t, const void*)
	{
		tset_error(EOPNOTSUPP, "Cannot raw write to NTFS system node.");
		return 0;
	}
	
	
	Filesystem* NTFSSystemNode::get_filesystem() const noexcept
	{
		return volume();
	}
}