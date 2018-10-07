#ifndef INCLUDED_FAT32_FILENODE_HH
#define INCLUDED_FAT32_FILENODE_HH


#include <kernel/Filesystem/FileNode.hh>
#include "Node.hh"

namespace Kernel::FS
{
	class FAT32FileNode : public FileNode, public FAT32Node
	{
		
		protected:
		
		public:
		FAT32FileNode(FAT32&, FAT32DirEnt_t*, DirectoryNode_v* parent);
		
		virtual uint64_t read(uint64_t, uint64_t, void*) override;
		virtual uint64_t write(uint64_t, uint64_t, const void*) override;
	};
	
}

#endif