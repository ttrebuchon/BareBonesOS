#include "MFT.hh"


namespace Kernel::FS
{
	MFTNode::MFTNode(NTFS* ntfs, ptr<ntfs_mft_record_t> rec, ptr<ntfs_stdinfo_t> info) : NTFSSystemNode(ntfs, rec, info)
	{
		
	}
	
	MFTNode::MFTNode(NTFS* ntfs) : NTFSSystemNode(ntfs, nullptr, nullptr)
	{
		
	}
	
	
	void MFTNode::init_mft(ptr<ntfs_mft_record_t> rec, ptr<ntfs_stdinfo_t> info)
	{
		if (rec && !this->mft_rec)
		{
			this->mft_rec = rec;
			this->mft_no = rec->mft_record_num;
		}
		
		
		
		if (info && !this->_info)
		{
			this->_info = info;
		}
		
		if (this->_info && this->mft_rec)
		{
			state |= 1;
		}
	}
}