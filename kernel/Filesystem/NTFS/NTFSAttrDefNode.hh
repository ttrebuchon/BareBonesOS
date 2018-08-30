#ifndef INCLUDED_NTFSATTRDEFNODE_HH
#define INCLUDED_NTFSATTRDEFNODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSAttrDefNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSAttrDefNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif