#ifndef INCLUDED_FS_EXT2BLOCKDEVICE_HH
#define INCLUDED_FS_EXT2BLOCKDEVICE_HH

#include "EXT2Node.hh"
#include <kernel/Filesystem/BlockDeviceNode.hh>

namespace Kernel::FS
{
	class EXT2BlockDeviceNode : public EXT2Node, public BlockDeviceNode
	{
		public:
		typedef FS::EXT2 filesystem_type;
		
		protected:
		
		
		private:
		void init(DirectoryNode_v* parent);
		
		public:
		
		EXT2BlockDeviceNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index, DeviceTarget*);
	};
	
	
}

#endif