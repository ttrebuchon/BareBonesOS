#ifndef INCLUDED_FS_NTFS_FACTORY_HH
#define INCLUDED_FS_NTFS_FACTORY_HH

#include "../NodeFactory.hh"


namespace Kernel::FS
{
	class NTFS;
	
	class NTFSFactory : public Factory
	{
		public:
		
		protected:
		NTFS& fs;
		
		
		NTFSFactory(NTFS&) noexcept;
		
		
		public:
		
		
		
		virtual DirectoryNode_v* create_directory(DirectoryNode_v* parent, const Utils::string& name) noexcept override;
		
		virtual FileNode_v* create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept override;
		
		virtual LinkNode* create_link(DirectoryNode_v* parent, const Utils::string&, const Utils::string&) noexcept override;
		
		virtual BlockDeviceNode* create_block_device(DirectoryNode_v* parent, const Utils::string&, DeviceTarget*) noexcept override;
		
		virtual CharDeviceNode* create_char_device(DirectoryNode_v* parent, const Utils::string&, DeviceTarget*) noexcept override;
		
		
		friend class NTFS;
	};
}


#endif