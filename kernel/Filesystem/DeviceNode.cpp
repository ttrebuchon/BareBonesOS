#include "DeviceNode.hh"
#include <Std/errno.h>

namespace Kernel::FS
{
	DeviceNode::DeviceNode(DeviceTarget* device, const Utils::string& name, const NodeType t) : Node(t), _device(device)
	{
		this->_name = name;
	}
	
	DeviceNode::DeviceNode(DeviceTarget* device, const NodeType t) : DeviceNode(device, "", t)
	{
		
	}
	
	DeviceNode::DeviceNode(DeviceTarget* device, const Utils::string& name) : DeviceNode(device, name, NodeType::Unknown)
	{
		
	}
	
	DeviceNode::DeviceNode(DeviceTarget* device) : DeviceNode(device, "")
	{
		
	}
	
	
	
	DeviceTarget* DeviceNode::device() noexcept
	{
		if (!_device)
		{
			this->open();
		}
		return _device;
	}
	
	const DeviceTarget* DeviceNode::device() const noexcept
	{
		if (!_device)
		{
			tset_error(EBADF, "Device has not been opened yet.");
		}
		return _device;
	}
	
	/*Drivers::Disk* DeviceNode::mount(Drivers::Disk* nDisk) noexcept
	{
		Utils::swap(nDisk, _device);
		return nDisk;
	}*/
	
	bool DeviceNode::has_device() const noexcept
	{
		return (device() != nullptr);
	}
	
}