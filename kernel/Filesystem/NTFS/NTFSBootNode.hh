#ifndef INCLUDED_NTFSBOOTNODE_HH
#define INCLUDED_NTFSBOOTNODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSBootNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSBootNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif