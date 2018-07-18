#ifndef INCLUDED_FAT32_NODE_HH
#define INCLUDED_FAT32_NODE_HH

#include <kernel/Filesystem/Node.hh>
#include "FAT32.h"

namespace Kernel::FS
{
	
	class FAT32Node : public virtual Node
	{
		protected:
		FAT32* fs;
		FAT32ClusterNumber_t cluster_no;
		FAT32DirEnt_t* entity;
		
		FAT32Node(FAT32&, const NodeType, FAT32DirEnt_t*, DirectoryNode_v* parent);
		
		public:
		
		
	};
}

#endif