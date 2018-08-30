#ifndef INCLUDED_NTFSBADCLUSNODE_HH
#define INCLUDED_NTFSBADCLUSNODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSBadClusNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSBadClusNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif