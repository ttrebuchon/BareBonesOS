#include "BlockDeviceNode.hh"
#include <drivers/Disk/Disk.hh>
#include <Utils/utility>


namespace Kernel { namespace FS {
	
	BlockDeviceNode::BlockDeviceNode(Drivers::Disk* device, const Utils::string& name, const NodeType t) : Node(NodeType::Block | t), _device(device)
	{
		this->_type |= (t | NodeType::Block);
		this->_name = name;
	}
	
	BlockDeviceNode::BlockDeviceNode(const Utils::string& name, const NodeType t) : BlockDeviceNode(nullptr, name, t)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(Drivers::Disk* device, const NodeType t) : BlockDeviceNode(device, "", t)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(const NodeType t) : BlockDeviceNode(nullptr, t)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(Drivers::Disk* device, const Utils::string& name) : BlockDeviceNode(device, name, NodeType::Block)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(const Utils::string& name) : BlockDeviceNode(nullptr, name)
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode(Drivers::Disk* device) : BlockDeviceNode(device, "")
	{
		
	}
	
	BlockDeviceNode::BlockDeviceNode() : BlockDeviceNode(nullptr)
	{
		
	}
	
	
	
	
	Drivers::Disk* BlockDeviceNode::device() noexcept
	{
		return _device;
	}
	
	const Drivers::Disk* BlockDeviceNode::device() const noexcept
	{
		return _device;
	}
	
	Drivers::Disk* BlockDeviceNode::mount(Drivers::Disk* nDisk) noexcept
	{
		Utils::swap(nDisk, _device);
		return nDisk;
	}
	
	bool BlockDeviceNode::has_device() const noexcept
	{
		return (device() != nullptr);
	}
	
	
	uint64_t BlockDeviceNode::read(uint64_t start, uint64_t len, uint8_t* buf)
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
	
	uint64_t BlockDeviceNode::write(uint64_t start, uint64_t len, const uint8_t* buf)
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
	
	void BlockDeviceNode::open()
	{
		
	}
	
	void BlockDeviceNode::close()
	{
		
	}
	
	DirEnt* BlockDeviceNode::readDir(uint32_t)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	Node* BlockDeviceNode::findDir(const char* name)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	
}
}