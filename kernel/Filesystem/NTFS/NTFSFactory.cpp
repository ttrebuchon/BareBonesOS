#include "NTFSFactory.hh"


namespace Kernel::FS
{
	NTFSFactory::NTFSFactory(NTFS& fs) noexcept : Factory(), fs(fs)
	{
		
	}
	
	
	
	
	
	node_ptr<DirectoryNode_v> NTFSFactory::create_directory(DirectoryNode_v* parent, const Utils::string& name) noexcept
	{
		assert(NOT_IMPLEMENTED);
		return nullptr;
	}
		
	node_ptr<FileNode_v> NTFSFactory::create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept
	{
		assert(NOT_IMPLEMENTED);
		return nullptr;
	}
		
	node_ptr<LinkNode> NTFSFactory::create_link(DirectoryNode_v* parent, const Utils::string&, const Utils::string&) noexcept
	{
		assert(NOT_IMPLEMENTED);
		return nullptr;
	}
		
	node_ptr<BlockDeviceNode> NTFSFactory::create_block_device(DirectoryNode_v* parent, const Utils::string&, const devtarget_t&) noexcept
	{
		assert(NOT_IMPLEMENTED);
		return nullptr;
	}
	
	node_ptr<CharDeviceNode> NTFSFactory::create_char_device(DirectoryNode_v* parent, const Utils::string&, const devtarget_t&) noexcept
	{
		assert(NOT_IMPLEMENTED);
		return nullptr;
	}
}