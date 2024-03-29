#include "BlockDeviceNode.hh"
#include <drivers/Disk/Disk.hh>
#include <Utils/utility>
#include <kernel/Filesystem/Devices/DeviceTarget.hh>


namespace Kernel { namespace FS {
	
	BlockDeviceNode::BlockDeviceNode(const devtarget_t& device, const Utils::string& name, const NodeType t) : DeviceNode(device, name, NodeType::Block | t)
	{
		this->_type |= (t | NodeType::Block);
		this->_name = name;
	}
	
	BlockDeviceNode::BlockDeviceNode(const Utils::string& name, const NodeType t) : BlockDeviceNode(nullptr, name, t)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(const devtarget_t& device, const NodeType t) : BlockDeviceNode(device, "", t)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(const NodeType t) : BlockDeviceNode((devtarget_t)nullptr, t)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(const devtarget_t& device, const Utils::string& name) : BlockDeviceNode(device, name, NodeType::Block)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(const Utils::string& name) : BlockDeviceNode(nullptr, name)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(const devtarget_t& device) : BlockDeviceNode(device, "")
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode() : BlockDeviceNode((devtarget_t)nullptr)
	{
		
	}
	
	
	BlockDeviceNode::BlockDeviceNode(const Utils::string& name, uint32_t major_target, uint32_t minor_target) : BlockDeviceNode(nullptr, name)
	{
		// TODO
	}
	
	BlockDeviceNode::BlockDeviceNode(uint32_t major_target, uint32_t minor_target) : BlockDeviceNode("", major_target, minor_target)
	{
		
	}
	
	
	
	
	
	
	uint64_t BlockDeviceNode::read(uint64_t start, uint64_t len, void* buf)
	{
		auto dev = device();
		assert(dev);
		if (!dev)
		{
			// TODO: Handle error
			return 0;
		}
		return dev->read(start, len, (uint8_t*)buf);
	}
	
	uint64_t BlockDeviceNode::write(uint64_t start, uint64_t len, const void* buf)
	{
		auto dev = device();
		assert(dev);
		if (!dev)
		{
			// TODO: Handle error
			return 0;
		}
		return dev->write(start, len, (const uint8_t*)buf);
	}
	
	void BlockDeviceNode::open()
	{
		
	}
	
	void BlockDeviceNode::close()
	{
		
	}
	
}
}