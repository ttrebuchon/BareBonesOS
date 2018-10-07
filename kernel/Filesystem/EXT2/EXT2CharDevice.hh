#ifndef INCLUDED_FS_EXT2CHARDEVICE_HH
#define INCLUDED_FS_EXT2CHARDEVICE_HH

#include "EXT2Node.hh"
#include <kernel/Filesystem/CharDeviceNode.hh>

namespace Kernel::FS
{
	class EXT2CharDeviceNode : public EXT2Node, public CharDeviceNode
	{
		public:
		typedef FS::EXT2 filesystem_type;
		
		protected:
		
		
		private:
		void init(DirectoryNode_v* parent);
		
		public:
		
		EXT2CharDeviceNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index, const devtarget_t&);
	};
	
	
}

#endif