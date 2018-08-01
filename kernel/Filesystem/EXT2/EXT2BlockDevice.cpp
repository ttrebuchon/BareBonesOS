#include "EXT2BlockDevice.hh"

namespace Kernel::FS
{
	EXT2BlockDeviceNode::EXT2BlockDeviceNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index, DeviceTarget* target) : EXT2Node(fs, node, name, inode_index), BlockDeviceNode(target, name)
	{
		init(parent);
	}
	
	
	void EXT2BlockDeviceNode::init(DirectoryNode_v* parent)
	{
		this->_name = this->EXT2Node::inode_name;
		this->parent = parent;
	}
	
}