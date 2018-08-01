#ifndef INCLUDED_FILESYSTEM_BLOCKDEVICENODE_HH
#define INCLUDED_FILESYSTEM_BLOCKDEVICENODE_HH

#include <Common.h>
#include "DeviceNode.hh"


namespace Kernel { namespace FS {
	
	
	
	class BlockDeviceNode : public DeviceNode
	{
		protected:
		
		BlockDeviceNode(DeviceTarget* device, const NodeType);
		BlockDeviceNode(const NodeType);
		BlockDeviceNode(DeviceTarget* device, const Utils::string& name, const NodeType);
		BlockDeviceNode(const Utils::string& name, const NodeType);
		
		public:
		BlockDeviceNode(DeviceTarget* disk);
		BlockDeviceNode();
		BlockDeviceNode(DeviceTarget* device, const Utils::string& name);
		BlockDeviceNode(const Utils::string& name);
		
		
		BlockDeviceNode(const Utils::string& name, uint32_t major_target, uint32_t minor_target);
		BlockDeviceNode(uint32_t major_target, uint32_t minor_target);
		
		
		virtual ~BlockDeviceNode() = default;
		
		
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		virtual Filesystem* get_filesystem() const noexcept override
		{ return nullptr; }
		
		
		virtual const BlockDeviceNode* as_block_device() const noexcept
		{ return this; }
		virtual BlockDeviceNode* as_block_device() noexcept
		{ return this; }
	};
	
}
}

#endif