#ifndef INCLUDED_NTFSLOGFILENODE_HH
#define INCLUDED_NTFSLOGFILENODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSLogFileNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSLogFileNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif