#ifndef INCLUDED_FS_NTFS_MFT_HH
#define INCLUDED_FS_NTFS_MFT_HH

#include "NTFSSystemNode.hh"


namespace Kernel::FS
{
	class MFTNode : public NTFSSystemNode
	{
		public:
		
		
		protected:
		
		// For the original, base MFT node
		// to be initialized prior to
		// MFT record reading
		// initialization
		MFTNode(NTFS*);
		
		
		void init_mft(ptr<ntfs_mft_record_t>, ptr<ntfs_stdinfo_t>);
		
		
		public:
		MFTNode(NTFS*, ptr<ntfs_mft_record_t>, ptr<ntfs_stdinfo_t>);
		
		
		
		friend class ntfs_setup;
	};
}

#endif