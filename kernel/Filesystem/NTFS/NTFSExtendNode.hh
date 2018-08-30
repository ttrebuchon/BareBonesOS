#ifndef INCLUDED_NTFSEXTENDNODE_HH
#define INCLUDED_NTFSEXTENDNODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSExtendNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSExtendNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif