#include "CharDeviceNode.hh"
#include <drivers/Disk/Disk.hh>
#include <Utils/utility>
#include <kernel/Filesystem/Devices/DeviceTarget.hh>


namespace Kernel { namespace FS {
	
	CharDeviceNode::CharDeviceNode(DeviceTarget* device, const Utils::string& name, const NodeType t) : DeviceNode(device, name, NodeType::Char | t)
	{
		this->_type |= (t | NodeType::Char);
		this->_name = name;
	}
	
	CharDeviceNode::CharDeviceNode(const Utils::string& name, const NodeType t) : CharDeviceNode(nullptr, name, t)
	{
		
	}
	
	CharDeviceNode::CharDeviceNode(DeviceTarget* device, const NodeType t) : CharDeviceNode(device, "", t)
	{
		
	}
	
	CharDeviceNode::CharDeviceNode(const NodeType t) : CharDeviceNode(nullptr, t)
	{
		
	}
	
	CharDeviceNode::CharDeviceNode(DeviceTarget* device, const Utils::string& name) : CharDeviceNode(device, name, NodeType::Char)
	{
		
	}
	
	CharDeviceNode::CharDeviceNode(const Utils::string& name) : CharDeviceNode(nullptr, name)
	{
		
	}
	
	CharDeviceNode::CharDeviceNode(DeviceTarget* device) : CharDeviceNode(device, "")
	{
		
	}
	
	CharDeviceNode::CharDeviceNode() : CharDeviceNode(nullptr)
	{
		
	}
	
	
	CharDeviceNode::CharDeviceNode(const Utils::string& name, uint32_t major_target, uint32_t minor_target) : CharDeviceNode(nullptr, name)
	{
		// TODO
	}
	
	CharDeviceNode::CharDeviceNode(uint32_t major_target, uint32_t minor_target) : CharDeviceNode("", major_target, minor_target)
	{
		
	}
	
	
	
	
	
	
	uint64_t CharDeviceNode::read(uint64_t start, uint64_t len, uint8_t* buf)
	{
		auto dev = device();
		assert(dev);
		if (!dev)
		{
			// TODO: Handle error
			return 0;
		}
		return dev->read(start, len, buf);
	}
	
	uint64_t CharDeviceNode::write(uint64_t start, uint64_t len, const uint8_t* buf)
	{
		auto dev = device();
		assert(dev);
		if (!dev)
		{
			// TODO: Handle error
			return 0;
		}
		return dev->write(start, len, buf);
	}
	
	void CharDeviceNode::open()
	{
		
	}
	
	void CharDeviceNode::close()
	{
		
	}
	
}
}