#ifndef INCLUDED_NTFSVOLUMENODE_HH
#define INCLUDED_NTFSVOLUMENODE_HH

#include "NTFSSystemNode.hh"

namespace Kernel::FS
{
	class NTFSVolumeNode : public NTFSSystemNode
	{
		public:
		
		protected:
		NTFSVolumeNode(NTFS* v, ptr<ntfs_mft_record_t> r, ptr<ntfs_stdinfo_t> i)
		 : NTFSSystemNode(v, r, i)
		{}
		
		public:
		
		
		friend class NTFS;
	};
}

#endif