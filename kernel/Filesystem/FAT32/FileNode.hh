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
		FAT32FileNode(FAT32&, FAT32DirEnt_t*, DirectoryNode* parent);
		
		virtual uint32_t read(uint32_t, uint32_t, uint8_t*) override;
		virtual uint32_t write(uint32_t, uint32_t, const uint8_t*) override;
	};
	
}

#endif