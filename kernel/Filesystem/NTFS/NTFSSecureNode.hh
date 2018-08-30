#ifndef INCLUDED_NTFSSECURENODE_HH
#define INCLUDED_NTFSSECURENODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSSecureNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSSecureNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif