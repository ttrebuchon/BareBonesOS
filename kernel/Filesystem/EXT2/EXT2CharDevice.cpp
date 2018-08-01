#include "EXT2CharDevice.hh"

namespace Kernel::FS
{
	EXT2CharDeviceNode::EXT2CharDeviceNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index, DeviceTarget* target) : EXT2Node(fs, node, name, inode_index), CharDeviceNode(target, name)
	{
		init(parent);
	}
	
	
	void EXT2CharDeviceNode::init(DirectoryNode_v* parent)
	{
		this->_name = this->EXT2Node::inode_name;
		this->parent = parent;
	}
	
}