#ifndef INCLUDED_FILESYSTEM_CHARDEVICENODE_HH
#define INCLUDED_FILESYSTEM_CHARDEVICENODE_HH

#include <Common.h>
#include "DeviceNode.hh"


namespace Kernel { namespace FS {
	
	
	
	class CharDeviceNode : public DeviceNode
	{
		protected:
		
		CharDeviceNode(const devtarget_t& device, const NodeType);
		CharDeviceNode(const NodeType);
		CharDeviceNode(const devtarget_t& device, const Utils::string& name, const NodeType);
		CharDeviceNode(const Utils::string& name, const NodeType);
		
		public:
		CharDeviceNode(const devtarget_t& disk);
		CharDeviceNode();
		CharDeviceNode(const devtarget_t& device, const Utils::string& name);
		CharDeviceNode(const Utils::string& name);
		
		
		CharDeviceNode(const Utils::string& name, uint32_t major_target, uint32_t minor_target);
		CharDeviceNode(uint32_t major_target, uint32_t minor_target);
		
		
		virtual ~CharDeviceNode() = default;
		
		
		virtual uint64_t read(uint64_t, uint64_t, void*) override;
		virtual uint64_t write(uint64_t, uint64_t, const void*) override;
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