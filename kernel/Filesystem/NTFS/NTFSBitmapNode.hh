#ifndef INCLUDED_NTFSBITMAPNODE_HH
#define INCLUDED_NTFSBITMAPNODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSBitmapNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSBitmapNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif