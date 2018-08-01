#ifndef INCLUDED_FILESYSTEM_CHARDEVICENODE_HH
#define INCLUDED_FILESYSTEM_CHARDEVICENODE_HH

#include <Common.h>
#include "DeviceNode.hh"


namespace Kernel { namespace FS {
	
	
	
	class CharDeviceNode : public DeviceNode
	{
		protected:
		
		CharDeviceNode(DeviceTarget* device, const NodeType);
		CharDeviceNode(const NodeType);
		CharDeviceNode(DeviceTarget* device, const Utils::string& name, const NodeType);
		CharDeviceNode(const Utils::string& name, const NodeType);
		
		public:
		CharDeviceNode(DeviceTarget* disk);
		CharDeviceNode();
		CharDeviceNode(DeviceTarget* device, const Utils::string& name);
		CharDeviceNode(const Utils::string& name);
		
		
		CharDeviceNode(const Utils::string& name, uint32_t major_target, uint32_t minor_target);
		CharDeviceNode(uint32_t major_target, uint32_t minor_target);
		
		
		virtual ~CharDeviceNode() = default;
		
		
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		virtual Filesystem* get_filesystem() const noexcept override
		{ return nullptr; }
		
		
		virtual const CharDeviceNode* as_char_device() const noexcept
		{ return this; }
		virtual CharDeviceNode* as_char_device() noexcept
		{ return this; }
	};
	
}
}

#endif