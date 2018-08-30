#ifndef INCLUDED_NTFSUPCASENODE_HH
#define INCLUDED_NTFSUPCASENODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSUpCaseNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSUpCaseNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif